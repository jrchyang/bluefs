#pragma once

#include <string>
#include <vector>
#include <algorithm>

#include "bluefs/namespace.h"
#include "likely.h"
#include "assert.h"
#include "subsys_types.h"

namespace BLUEFS_NAMESPACE {
namespace logging {

class SubsystemMap {
  // Access to the current gathering levels must be *FAST* as they are
  // read over and over from all places in the code (via should_gather()
  // by i.e. dout).
  std::array<uint8_t, subsys_get_num()> m_gather_levels;

  // The rest. Should be as small as possible to not unnecessarily
  // enlarge md_config_t and spread it other elements across cache
  // lines. Access can be slow.
  std::vector<subsys_item_t> m_subsys;

  friend class Log;

public:
  SubsystemMap() {
    constexpr auto s = subsys_get_as_array();
    m_subsys.reserve(s.size());

    std::size_t i = 0;
    for (const subsys_item_t& item : s) {
      m_subsys.emplace_back(item);
      m_gather_levels[i++] = std::max(item.log_level, item.gather_level);
    }
  }

  constexpr static std::size_t get_num() {
    return subsys_get_num();
  }

  constexpr static std::size_t get_max_subsys_len() {
    return subsys_max_name_length();
  }

  int get_log_level(unsigned subsys) const {
    if (subsys >= get_num())
      subsys = 0;
    return m_subsys[subsys].log_level;
  }

  int get_gather_level(unsigned subsys) const {
    if (subsys >= get_num())
      subsys = 0;
    return m_subsys[subsys].gather_level;
  }

  // TODO(rzarzynski): move to string_view?
  constexpr const char* get_name(unsigned subsys) const {
    if (subsys >= get_num())
      subsys = 0;
    return subsys_get_as_array()[subsys].name;
  }

  template <unsigned SubV, int LvlV>
  bool should_gather() const {
    static_assert(SubV < get_num(), "wrong subsystem ID");
    static_assert(LvlV >= -1 && LvlV <= 200);

    if constexpr (LvlV <= 0) {
      // handle the -1 and 0 levels entirely at compile-time.
      // Such debugs are intended to be gathered regardless even
      // of the user configuration.
      return true;
    } else {
      // we expect that setting level different than the default
      // is rather unusual.
      return expect(LvlV <= static_cast<int>(m_gather_levels[SubV]),
                    LvlV <= subsys_get_max_default_level(SubV));
    }
  }

  bool should_gather(const unsigned sub, int level) const {
    bluefs_assert(sub < m_subsys.size());
    return level <= static_cast<int>(m_gather_levels[sub]);
  }

  void set_log_level(unsigned subsys, uint8_t log)
  {
    bluefs_assert(subsys < m_subsys.size());
    m_subsys[subsys].log_level = log;
    m_gather_levels[subsys] = std::max(log, m_subsys[subsys].gather_level);
  }

  void set_gather_level(unsigned subsys, uint8_t gather)
  {
    bluefs_assert(subsys < m_subsys.size());
    m_subsys[subsys].gather_level = gather;
    m_gather_levels[subsys] = std::max(m_subsys[subsys].log_level, gather);
  }
};

}
}

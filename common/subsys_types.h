#pragma once

#include <algorithm>
#include <array>
#include <cstdint>

#include "bluefs/namespace.h"

namespace BLUEFS_NAMESPACE {

enum subsys_id_t {
  subsys_,   // default
#define SUBSYS(name, log, gather) \
  subsys_##name,
#define DEFAULT_SUBSYS(log, gather)
#include "subsys.h"
#undef SUBSYS
#undef DEFAULT_SUBSYS
  subsys_max
};

constexpr static std::size_t subsys_get_num() {
  return static_cast<std::size_t>(subsys_max);
}

struct subsys_item_t {
  const char* name;
  uint8_t log_level;
  uint8_t gather_level;
};

constexpr static std::array<subsys_item_t, subsys_get_num()>
subsys_get_as_array() {
#define SUBSYS(name, log, gather) \
  subsys_item_t{ #name, log, gather },
#define DEFAULT_SUBSYS(log, gather) \
  subsys_item_t{ "none", log, gather },

  return {
#include "subsys.h"
  };
#undef SUBSYS
#undef DEFAULT_SUBSYS
}

constexpr static std::uint8_t
subsys_get_max_default_level(const std::size_t subidx) {
  const auto item = subsys_get_as_array()[subidx];
  return std::max(item.log_level, item.gather_level);
}

// Compile time-capable version of std::strlen. Resorting to own
// implementation only because C++17 doesn't mandate constexpr
// on the standard one.
constexpr static std::size_t strlen_ct(const char* const s) {
  std::size_t l = 0;
  while (s[l] != '\0') {
    ++l;
  }
  return l;
}

constexpr static std::size_t subsys_max_name_length() {
  return std::max({
#define SUBSYS(name, log, gather) \
  strlen_ct(#name),
#define DEFAULT_SUBSYS(log, gather) \
  strlen_ct("none"),
#include "subsys.h"
#undef SUBSYS
#undef DEFAULT_SUBSYS
  });
}

}

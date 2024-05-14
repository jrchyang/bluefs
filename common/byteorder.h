#pragma once

#include <linux/types.h>
#include <boost/endian/conversion.hpp>

#include "bluefs/namespace.h"

namespace BLUEFS_NAMESPACE {

template<typename T>
inline T swab(T val) {
  return boost::endian::endian_reverse(val);
}

template<typename T>
struct bluefs_le {
private:
  T v;
public:
  bluefs_le() = default;
  explicit bluefs_le(T nv)
    : v{boost::endian::native_to_little(nv)}
  {}
  bluefs_le<T>& operator=(T nv) {
    v = boost::endian::native_to_little(nv);
    return *this;
  }
  operator T() const { return boost::endian::little_to_native(v); }
  friend inline bool operator==(bluefs_le a, bluefs_le b) {
    return a.v == b.v;
  }
} __attribute__ ((packed));

using bluefs_le64 = bluefs_le<__u64>;
using bluefs_le32 = bluefs_le<__u32>;
using bluefs_le16 = bluefs_le<__u16>;

using bluefs_les64 = bluefs_le<__s64>;
using bluefs_les32 = bluefs_le<__s32>;
using bluefs_les16 = bluefs_le<__s16>;

inline bluefs_les64 init_les64(__s64 x) {
  bluefs_les64 v;
  v = x;
  return v;
}
inline bluefs_les32 init_les32(__s32 x) {
  bluefs_les32 v;
  v = x;
  return v;
}
inline bluefs_les16 init_les16(__s16 x) {
  bluefs_les16 v;
  v = x;
  return v;
}

#define __le16 bluefs_le16
#define __le32 bluefs_le32
#define __le64 bluefs_le64

}

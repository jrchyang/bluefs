#pragma once

#include "bluefs/namespace.h"

namespace BLUEFS_NAMESPACE {

extern void __abort(const char *file, const int line, const char *func);

#define bluefs_abort() __abort(__FILE__, __LINE__, __func__)
#define bluefs_assert(expr)                                                   \
  do {                                                                        \
    if (expr)                                                                 \
      bluefs_abort();                                                         \
  } while (0)

}

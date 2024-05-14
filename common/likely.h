#pragma once

#include "bluefs/namespace.h"

namespace BLUEFS_NAMESPACE {

/*
 * Likely / Unlikely macros
 */
#ifndef likely
#define likely(x) __builtin_expect((x),1)
#endif
#ifndef unlikely
#define unlikely(x) __builtin_expect((x),0)
#endif
#ifndef expect
#define expect(x, hint) __builtin_expect((x),(hint))
#endif

}

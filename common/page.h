#pragma once

#include <unistd.h>
#include "bluefs/namespace.h"

namespace BLUEFS_NAMESPACE {

// these are in common/page.cc
extern unsigned _page_size;
extern unsigned long _page_mask;
extern unsigned _page_shift;

#define BLUEFS_PAGE_SIZE _page_size
#define BLUEFS_PAGE_MASK _page_mask
#define BLUEFS_PAGE_SHIFT page_shift

}

#pragma once

#include <string>
#include "bluefs/namespace.h"

namespace BLUEFS_NAMESPACE {

void dout_emergency(const char * const str);
void dout_emergency(const std::string &str);

}

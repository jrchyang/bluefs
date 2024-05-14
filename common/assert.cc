#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <execinfo.h>
#include <cxxabi.h>
#include <iostream>
#include <sstream>
#include <string>

#include "dout.h"
#include "assert.h"

namespace BLUEFS_NAMESPACE {

std::string demangle(const char* name) {
  static constexpr char OPEN = '(';
  const char *begin = nullptr;
  const char *end = nullptr;

  for (const char *j = name; *j; ++j) {
    if (*j == OPEN) {
      begin = j + 1;
    } else if (*j == '+') {
      end = j;
    }
  }

  if (begin && end && begin < end) {
    std::string mangled(begin, end);
    int status;
    if (mangled.compare(0, 2, "_Z") == 0) {
      char *demangled = abi::__cxa_demangle(mangled.c_str(), nullptr, nullptr, &status);
      if (!status) {
        std::string full_name{OPEN};
        full_name += demangled;
        full_name += end;
        free(demangled);
        return full_name;
      }
    }
    return mangled + "()";
  } else {
    return name;
  }
}

void __abort(const char *file, const int line, const char *func) {
  const static int max = 32;
  size_t size;
  size_t i;
  void *array[max]{};
  char **strings;

  char assert_msg[8096] = {0};
  char bt_msg[1024] = {0};
  unsigned long long thread = (unsigned long long)pthread_self();

  time_t rawtime;
  struct tm *info;

  std::ostringstream oss;

  time(&rawtime);
  info = localtime(&rawtime);

  snprintf(assert_msg, sizeof(assert_msg),
           "%s: In function '%s' thread %llx time %s\n"
           "%s: %d: rocksdb abort\n",
           file, func, thread, asctime(info),
           file, line);
  dout_emergency(assert_msg);

  size = backtrace(array, max);
  strings = backtrace_symbols(array, size);

  for (i = 1; i < size; ++i) {
    std::string bt_info = demangle(strings[i]);
    snprintf(bt_msg, sizeof(bt_msg), " %lu : %s", i, bt_info.c_str());
    oss << bt_msg;
  }
  dout_emergency(oss.str());

  std::abort();
}

}

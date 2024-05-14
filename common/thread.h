#pragma once

#include <functional>
#include <string_view>
#include <system_error>
#include <thread>
#include <pthread.h>
#include <sys/types.h>

#include "bluefs/namespace.h"

namespace BLUEFS_NAMESPACE {

#define bluefs_pthread_setname pthread_setname_np
#define bluefs_pthread_getname pthread_getname_np

extern pid_t bluefs_gettid();

class Thread {
private:
  pthread_t thread_id;
  pid_t pid;
  int cpuid;
  std::string thread_name;

  void *entry_wrapper();

public:
  Thread(const Thread&) = delete;
  Thread& operator=(const Thread&) = delete;
  Thread();
  virtual ~Thread();

protected:
  virtual void *entry() = 0;

private:
  static void *_entry_func(void *arg);

public:
  const pthread_t &get_thread_id() const;
  pid_t get_pid() const { return pid; }
  bool is_started() const;
  bool am_self() const;
  int kill(int signal);
  int try_create(size_t statcksize);
  void create(const char *name, size_t stacksize = 0);
  int join(void **prval = nullptr);
  int detach();
  int set_affinity(int cpuid);
};

void set_thread_name(std::thread& t, const std::string& s);
std::string get_thread_name(const std::thread& t);
void kill(std::thread& t, int signal);

template<typename Fun, typename... Args>
std::thread make_named_thread(std::string_view n,
                              Fun&& fun,
                              Args&& ...args) {

  return std::thread([n = std::string(n)](auto&& fun, auto&& ...args) {
                      bluefs_pthread_setname(pthread_self(), n.data());
                      std::invoke(std::forward<Fun>(fun),
                      std::forward<Args>(args)...);
                    }, std::forward<Fun>(fun), std::forward<Args>(args)...);
}

}

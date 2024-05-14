#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/syscall.h>

#include "bluefs/namespace.h"
#include "dout.h"
#include "assert.h"
#include "page.h"
#include "signal.h"
#include "thread.h"

namespace BLUEFS_NAMESPACE {

pid_t bluefs_gettid(void)
{
  return syscall(SYS_gettid);
}

static int _set_affinity(int id)
{
  if (id >= 0 && id < CPU_SETSIZE) {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(id, &cpuset);

    if (sched_setaffinity(0, sizeof(cpuset), &cpuset) < 0)
      return -errno;
    /* guaranteed to take effect immediately */
    sched_yield();
  }

  return 0;
}

Thread::Thread()
  : thread_id(0),
    pid(0),
    cpuid(-1)
{
}

Thread::~Thread()
{
}

void *Thread::_entry_func(void *arg) {
  void *r = ((Thread*)arg)->entry_wrapper();
  return r;
}

void *Thread::entry_wrapper()
{
  int p = bluefs_gettid(); // may return -ENOSYS on other platforms
  if (p > 0)
    pid = p;
  if (pid && cpuid >= 0)
    _set_affinity(cpuid);

  bluefs_pthread_setname(pthread_self(), thread_name.c_str());
  return entry();
}

const pthread_t &Thread::get_thread_id() const
{
  return thread_id;
}

bool Thread::is_started() const
{
  return thread_id != 0;
}

bool Thread::am_self() const
{
  return (pthread_self() == thread_id);
}

int Thread::kill(int signal)
{
  if (thread_id)
    return pthread_kill(thread_id, signal);
  else
    return -EINVAL;
}

int Thread::try_create(size_t stacksize)
{
  pthread_attr_t *thread_attr = NULL;
  pthread_attr_t thread_attr_loc;

  stacksize &= BLUEFS_PAGE_MASK;  // must be multiple of page
  if (stacksize) {
    thread_attr = &thread_attr_loc;
    pthread_attr_init(thread_attr);
    pthread_attr_setstacksize(thread_attr, stacksize);
  }

  int r;

  // The child thread will inherit our signal mask.  Set our signal mask to
  // the set of signals we want to block.  (It's ok to block signals more
  // signals than usual for a little while-- they will just be delivered to
  // another thread or delieverd to this thread later.)

  sigset_t old_sigset;
  block_signals(NULL, &old_sigset);
  r = pthread_create(&thread_id, thread_attr, _entry_func, (void*)this);
  restore_sigset(&old_sigset);

  if (thread_attr) {
    pthread_attr_destroy(thread_attr);
  }

  return r;
}

void Thread::create(const char *name, size_t stacksize)
{
  bluefs_assert(strlen(name) < 16);
  thread_name = name;

  int ret = try_create(stacksize);
  if (ret != 0) {
    char buf[256];
    snprintf(buf, sizeof(buf), "Thread::try_create(): pthread_create "
             "failed with error %d", ret);
    dout_emergency(buf);
    bluefs_assert(ret == 0);
  }
}

int Thread::join(void **prval)
{
  if (thread_id == 0) {
    bluefs_abort();
    return -EINVAL;
  }

  int status = pthread_join(thread_id, prval);
  if (status != 0) {
    char buf[256];
    snprintf(buf, sizeof(buf), "Thread::join(): pthread_join "
             "failed with error %d\n", status);
    dout_emergency(buf);
    bluefs_assert(status == 0);
  }

  thread_id = 0;
  return status;
}

int Thread::detach()
{
  return pthread_detach(thread_id);
}

int Thread::set_affinity(int id)
{
  int r = 0;
  cpuid = id;
  if (pid && bluefs_gettid() == pid)
    r = _set_affinity(id);
  return r;
}

// Functions for std::thread
// =========================

void set_thread_name(std::thread& t, const std::string& s) {
  int r = bluefs_pthread_setname(t.native_handle(), s.c_str());
  if (r != 0) {
    throw std::system_error(r, std::generic_category());
  }
}

std::string get_thread_name(const std::thread& t) {
  char s[256] = {0};

  int r = bluefs_pthread_getname(const_cast<std::thread&>(t).native_handle(),
                               s, sizeof(s));
  if (r != 0) {
    throw std::system_error(r, std::generic_category());
  }
  std::string rv = s;
  return s;
}

void kill(std::thread& t, int signal)
{
  auto r = pthread_kill(t.native_handle(), signal);
  if (r != 0) {
    throw std::system_error(r, std::generic_category());
  }
}

}

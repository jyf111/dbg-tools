#pragma once

#include <unistd.h>

#include <fstream>
#include <sstream>
#include <string>

namespace gdb {
inline bool being_traced() {
  std::ifstream fin("/proc/self/status");
  std::string s;
  while (fin >> s) {
    if (s == "TracerPid:") {
      int pid;
      fin >> pid;
      return pid != 0;
    }
    std::getline(fin, s);
  }
  return false;
}

inline bool ensure_gdb_attached() {
  static int pid = 0;
  if (pid > 0 && being_traced()) return true;
  pid = fork();
  if (pid < 0) {
    perror("fork");
    return false;
  } else if (!pid) {
    int ppid = getppid();
    std::ostringstream oss;
    oss << "--pid=" << ppid;
    execlp("gdb", "gdb", "-q", "-ex", "continue", oss.str().c_str(), NULL);
    perror("execlp");
    return false;
  } else {
    const int RETRY = 500;
    for (int i = 0; i < RETRY; i++) {
      if (being_traced()) {
        return true;
      }
      usleep(10'000);  // 10ms
    }
    return false;
  }
}
}  // namespace gdb

#define BREAKPOINT()                  \
  do {                                \
    if (gdb::ensure_gdb_attached()) { \
      __asm__ volatile("int $0x03");  \
    }                                 \
  } while (false)

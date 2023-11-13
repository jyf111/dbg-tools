#pragma once

#include <sys/wait.h>
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace gdb {
inline bool which_gdb() {
  FILE *fp;
  char buf[64];
  fp = popen("which gdb", "r");
  if (fp) {
    bool find = fgets(buf, sizeof(buf) - 1, fp) != NULL;
    pclose(fp);
    return find;
  }
  return false;
}

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

/**
 * @return int the GDB status. This is to avoid this function appearing in the stack frame.
 * @retval <0 indicates an error.
 *          0 indicates that GDB is attached, and a breakpoint needs to be set later.
 *         >0 indicate that GDB is running in batch mode, and we need to wait for it later.
 */
inline int ensure_gdb_attached(const std::initializer_list<std::string> &commands) {
  static int pid = 0;
  if (!which_gdb()) {
    std::cerr << "[gdb.h] gdb is not found in your env!\n";
    return -1;
  }
  if (being_traced()) {
    if (pid > 0) {
      if (commands.size()) {
        std::cerr << "[gdb.h] gdb is already running. commands: {";
        for (auto iter = commands.begin(); iter != commands.end(); iter++) {
          std::cerr << *iter;
          if (std::next(iter) != commands.end()) std::cerr << ", ";
        }
        std::cerr << "} are ignored.\n";
      }
      return 0;
    } else {
      return -1;
    }
  }
  pid = fork();
  if (pid < 0) {
    perror("[gdb.h] fork");
    return -1;
  } else if (!pid) {
    int ppid = getppid();
    std::ostringstream oss;
    oss << "--pid=" << ppid;
    if (commands.size()) {
      std::vector<const char *> argv;
      argv.push_back("gdb");
      argv.push_back("--batch");
      for (auto iter = commands.begin(); iter != commands.end(); iter++) {
        argv.push_back("-ex");
        argv.push_back(iter->c_str());
      }
      argv.push_back(oss.str().c_str());
      argv.push_back(nullptr);
      execvp("gdb", const_cast<char *const *>(argv.data()));
    } else {
      execlp("gdb", "gdb", "-q", "-ex", "continue", oss.str().c_str(), nullptr);
    }
    perror("[gdb.h] execlp");
    return -1;
  } else {
    if (commands.size()) {
      return pid;
    } else {
      const int RETRY = 500;
      for (int i = 0; i < RETRY; i++) {
        if (being_traced()) {
          return 0;
        }
        usleep(10'000);  // 10ms
      }
    }
    return -1;
  }
}
}  // namespace gdb

#define BREAKPOINT(...)                                     \
  do {                                                      \
    int status = gdb::ensure_gdb_attached({ __VA_ARGS__ }); \
    if (status == 0) {                                      \
      __asm__ volatile("int $0x03");                        \
    } else if (status > 0) {                                \
      waitpid(status, nullptr, 0);                          \
    }                                                       \
  } while (false)

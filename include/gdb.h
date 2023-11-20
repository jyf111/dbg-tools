#pragma once

#include <sys/wait.h>
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

namespace gdb {
inline bool which_gdb() {
  char buf[64];
  FILE *fp;
  fp = popen("which gdb", "r");
  if (fp) {
    bool find = fgets(buf, sizeof(buf) - 1, fp) != nullptr;
    pclose(fp);
    return find;
  }
  return false;
}

inline bool being_traced() {
  std::ifstream fin("/proc/self/status");
  std::string s;
  int pid;
  while (fin >> s) {
    if (s == "TracerPid:") {
      fin >> pid;
      return pid != 0;
    }
    std::getline(fin, s);
  }
  return false;
}

class GdbCommand {
 public:
  static GdbCommand bt() { return GdbCommand("backtrace"); }

  static GdbCommand c() { return GdbCommand("continue"); }

  static GdbCommand up(size_t i) { return GdbCommand("up " + std::to_string(i)); }
  static GdbCommand down(size_t i) { return GdbCommand("down " + std::to_string(i)); }
  static GdbCommand frame(size_t i) { return GdbCommand("frame " + std::to_string(i)); }

  static GdbCommand mappings() { return GdbCommand("info proc mappings"); }
  static GdbCommand registers() { return GdbCommand("info registers"); }
  static GdbCommand locals() { return GdbCommand("info locals"); }

  [[nodiscard]] std::string_view command() const { return command_; }

  [[nodiscard]] bool read_only() const { return command_ != "continue"; }

 private:
  explicit GdbCommand(const char *command) : command_(command) {}
  explicit GdbCommand(const std::string &command) : command_(command) {}
  explicit GdbCommand(std::string &&command) : command_(std::move(command)) {}

  std::string command_;
};

/**
 * @return set `global_gdb_status`. This is to avoid a local variable and this function appearing in the stack frame.
 * @retval <0 indicates an error.
 *          0 indicates that GDB is attached, and a breakpoint needs to be set later.
 *         >0 indicate that GDB is running in batch mode, and we need to wait for it later.
 */
static int global_gdb_status;
inline void ensure_gdb_attached(const std::initializer_list<GdbCommand> &commands) {
  static int pid = 0;
  if (!which_gdb()) {
    std::cerr << "[gdb.h] GDB is not found in your env!\n";
    global_gdb_status = -1;
    return;
  }
  if (being_traced()) {
    if (pid > 0) {
      if (commands.size()) {
        std::cerr << "[gdb.h] GDB is already running! commands: {";
        for (auto iter = commands.begin(); iter != commands.end(); iter++) {
          std::cerr << iter->command();
          if (std::next(iter) != commands.end()) std::cerr << ", ";
        }
        std::cerr << "} are ignored!\n";
      }
      global_gdb_status = 0;
      return;
    }
    global_gdb_status = -1;
    return;
  }
  pid = fork();
  if (pid < 0) {
    perror("[gdb.h] fork");
    global_gdb_status = -1;
    return;
  } else if (!pid) {
    int ppid = getppid();
    std::ostringstream oss;
    oss << "--pid=" << ppid;
    if (commands.size()) {
      dup2(2, 1);  // redirect stdout to stderr
      std::vector<const char *> argv;
      argv.push_back("gdb");
      argv.push_back("--batch");
      argv.push_back("-ex");
      argv.push_back("up");  // remove the inner wait function by default
      for (const auto &command : commands) {
        if (command.read_only()) {  // allow only read-only commands in batch mode to avoid weird issues
          argv.push_back("-ex");
          argv.push_back(command.command().data());
        }
      }
      argv.push_back(oss.str().c_str());
      argv.push_back(nullptr);
      execvp("gdb", const_cast<char *const *>(argv.data()));
    } else {
      execlp("gdb", "gdb", "-q", "-ex", "continue", oss.str().c_str(), nullptr);
    }
    perror("[gdb.h] execlp");
    global_gdb_status = -1;
    return;
  } else {
    if (commands.size()) {
      global_gdb_status = pid;
      return;
    } else {
      const int RETRY = 500;
      for (int i = 0; i < RETRY; i++) {
        if (being_traced()) {
          global_gdb_status = 0;
          return;
        }
        usleep(10'000);  // 10ms
      }
    }
    global_gdb_status = -1;
    return;
  }
}
}  // namespace gdb

#define GDB(...)                                   \
  do {                                             \
    gdb::ensure_gdb_attached({ __VA_ARGS__ });     \
    if (gdb::global_gdb_status == 0) {             \
      __asm__ volatile("int $0x03");               \
    } else if (gdb::global_gdb_status > 0) {       \
      waitpid(gdb::global_gdb_status, nullptr, 0); \
    }                                              \
  } while (false)

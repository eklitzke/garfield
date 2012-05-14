#include "./logging.h"

#include <cstdarg>
#include <cstdio>
#include <memory>
#include <string>

namespace {
enum { LOG_BUF_SIZE = 4096 };
bool initialized = false;
garfield::Logger active_logger;
}

namespace garfield {
Logger GetLogger() {
  return active_logger;
}

void SetLogger(Logger logger) {
  active_logger = logger;
  initialized = true;
}

void Log(LogLevel level, const char *fmt, ...) {
  if (!initialized) {
    return;
  }

  std::unique_ptr<char []> buf(new char[LOG_BUF_SIZE]);

  va_list ap;
  va_start(ap, fmt);
  int bytes = vsprintf(buf.get(), fmt, ap);
  va_end(ap);

  active_logger(level, buf.get(), bytes);
}

void StdErrLogger(LogLevel level, const char *msg, int bytes) {
  std::string cmsg = (level == ERROR ? "ERROR:  " : "ACCESS: ");
  cmsg += std::string(msg, bytes);
  if (cmsg[cmsg.length() - 1] != '\n') {
    cmsg += '\n';
  }
  fwrite(cmsg.c_str(), sizeof(char), cmsg.length(), stderr);
}
}

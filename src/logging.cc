#include "./logging.h"

#include <cstdarg>
#include <cstdio>
#include <memory>
#include <sstream>
#include <string>

namespace {
enum { LOG_BUF_SIZE = 4096 };
bool initialized = false;
garfield::Logger active_logger;

std::string FormatMsg(const char *msg, int bytes) {
  std::string formatted = std::string(msg, bytes);
  if (formatted[formatted.size() - 1] != '\n') {
    formatted += '\n';
  }
  return formatted;
}

FILE *err_stream = nullptr;
FILE *access_stream = nullptr;

void FileWrite(const std::string &msg, FILE *out) {
  fwrite(msg.c_str(), sizeof(char), msg.length(), out);
  fflush(out);
}
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
  std::string cmsg = FormatMsg(msg, bytes);
  cmsg = (level == ERROR ? "[ERROR]  " : "[ACCESS] ") + cmsg;
  FileWrite(cmsg, stderr);
}

void FileLogger(LogLevel level, const char *msg, int bytes) {
  std::string cmsg = FormatMsg(msg, bytes);
  FILE *stream;
  if (level == ERROR) {
    if (err_stream == nullptr) {
      err_stream = fopen("error.log", "a");
    }
    stream = err_stream;
  } else {
    if (access_stream == nullptr) {
      access_stream = fopen("access.log", "a");
    }
    stream = access_stream;
  }
  FileWrite(cmsg, stream);
}
}

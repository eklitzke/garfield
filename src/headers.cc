#include <ctype.h>

#include "./headers.h"

namespace {
std::string ToLowercase(const std::string &input) {
  std::string out;
  for (char c : input) {
    out += tolower(c);
  }
  return out;
}
}

namespace garfield {
std::string HeadersDict::GetHeader(const std::string &name) const {
  std::string lower = ToLowercase(name);
  for (auto pair : headers_) {
    if (lower == ToLowercase(pair.first)) {
      return pair.second;
    }
  }
  return "";
}

void HeadersDict::SetHeader(const std::string &name, const std::string &val) {
  std::string lower = ToLowercase(name);
  for (std::pair<std::string, std::string> &pair : headers_) {
    if (lower == ToLowercase(pair.first)) {
      pair.second = val;
      return;
    }
  }
  headers_.push_back(std::make_pair(name, val));
}

void HeadersDict::AddHeader(const std::string &name, const std::string &val) {
  headers_.push_back(std::make_pair(name, val));
}

std::string HeadersDict::GetHeadersAsString() const {
  std::string output;
  for (auto pair : headers_) {
    output += pair.first;
    output += ": ";
    output += pair.second;
    output += "\r\n";
  }
  return output;
}
}

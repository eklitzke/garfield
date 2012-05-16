#include "./headers.h"

#include <utility>

namespace garfield {
std::string HeadersDict::GetHeader(const std::string &name) const {
  auto it = headers_.find(name);
  if (it == headers_.end()) {
    return "";
  }
  return it->second;
}

void HeadersDict::SetHeader(const std::string &name, const std::string &val) {
  auto pos = headers_.lower_bound(name);
  if (pos != headers_.end() && pos->first == name) {
    pos->second = val;
  } else {
    headers_.insert(pos, std::make_pair(name, val));
  }
}

void HeadersDict::AddHeader(const std::string &name, const std::string &val) {
  headers_.insert(std::make_pair(name, val));
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

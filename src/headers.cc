#include "./headers.h"

#include <ctype.h>  // for tolower()
#include <memory>
#include <utility>

namespace garfield {
std::string HeadersDict::GetHeader(const std::string &name) const {
  HeaderKey norm(name);
  auto it = headers_.find(norm);
  if (it == headers_.end()) {
    return "";
  }
  return it->second;
}

void HeadersDict::SetHeader(const HeaderKey &key, const std::string &val) {
  auto pos = headers_.lower_bound(key);
  if (pos != headers_.end() && pos->first == key) {
    pos->second = val;
  } else {
    headers_.insert(pos, std::make_pair(key, val));
  }
}

void HeadersDict::SetHeader(const std::string &key, const std::string &val) {
  HeaderKey norm_key(key);
  SetHeader(norm_key, val);
}

void HeadersDict::AddHeader(const HeaderKey &key, const std::string &val) {
  headers_.insert(std::make_pair(key, val));
}

void HeadersDict::AddHeader(const std::string &key, const std::string &val) {
  HeaderKey norm_key(key);
  AddHeader(norm_key, val);
}

std::string HeadersDict::GetHeadersAsString() const {
  std::string output;
  for (auto pair : headers_) {
    output += pair.first.raw_key;
    output += ": ";
    output += pair.second;
    output += "\r\n";
  }
  return output;
}

std::string NormalizeHeaderName(const std::string &name) {
  std::unique_ptr<char[]> chars(new char[name.length()]);
  size_t i = 0;
  for (char c : name) {
    chars[i++] = tolower(c);
  }
  return std::string(chars.get(), name.length());
}
}

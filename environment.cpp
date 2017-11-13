#include "wlisp.hpp"
#include <sstream>
#include <unordered_map>

struct Environment_base::Impl final {
  std::unordered_map<std::string, Variant> map = std::unordered_map<std::string, Variant>();
  Environment parent = nullptr;
};

Environment_base::Environment_base() noexcept : impl(std::make_shared<Impl>()) {}

Environment_base::Environment_base(Environment parent) noexcept : Environment_base() { impl->parent = parent; }

auto Environment_base::parent(Environment parent_value) noexcept -> void { impl->parent = parent_value; }

auto Environment_base::has(const std::string &key) const noexcept -> bool
{
  if (impl->map.find(key) != std::cend(impl->map)) {
    return true;
  }
  if (impl->parent) {
    return impl->parent->has(key);
  }
  return false;
}

const Variant &Environment_base::get(const std::string &key) const
{
  if (has(key)) {
    if (impl->map.find(key) != std::cend(impl->map)) {
      return impl->map.at(key);
    }
    if (impl->parent) {
      return impl->parent->get(key);
    }
  }
  throw std::runtime_error("Key does not exist in environment.");
}

auto Environment_base::set(std::string key, Variant value) noexcept -> void
{
  if (!has(key) || (impl->map.find(key) != std::cend(impl->map))) {
    impl->map[key] = value;
  }
  else if (impl->parent) {
    impl->parent->set(key, value);
  }
}

auto Environment_base::to_string() const noexcept -> std::string
{
  if (impl->map.empty() && impl->parent) {
    return std::string("{},") + impl->parent->to_string();
  }
  if (impl->map.empty()) {
    return "{}";
  }
  auto os = std::ostringstream();
  auto i = std::cbegin(impl->map);
  os << "{" << i->first << "=" << string_from(i->second) << ",";
  ++i;
  for (auto j = std::cend(impl->map); i != j; ++i) {

    os << "," << i->first << "=" << string_from(i->second);
  }
  os << "}";
  if (impl->parent) {
    os << "," << impl->parent->to_string();
  }
  return os.str();
}

auto create_environment(Environment parent) -> Environment
{
  return std::make_shared<Environment_base>(Environment_base(parent));
}

auto create_environment() -> Environment { return std::make_shared<Environment_base>(Environment_base(nullptr)); }

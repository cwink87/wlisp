#include "wlisp.hpp"

auto string_from(const Variant_type &variant_type) -> std::string
{
  switch (variant_type) {
  case Variant_type::nil:
    return "nil";
  case Variant_type::number:
    return "number";
  case Variant_type::string:
    return "string";
  case Variant_type::boolean:
    return "boolean";
  case Variant_type::list:
    return "list";
  case Variant_type::function:
    return "function";
  }
  return "unknown";
}

struct Variant::Impl final {
  double number_value = 0.0;
  std::string string_value = "";
  Variant_list list_value = Variant_list();
  Variant_function function_value = Variant_function();
  Variant_type variant_type = Variant_type::nil;
  bool boolean_value = false;
  char padding[3] = {0};
};

Variant::Variant() : impl(std::make_shared<Impl>()) {}

Variant::Variant(const double number_value) : Variant()
{
  impl->variant_type = Variant_type::number;
  impl->number_value = number_value;
}

Variant::Variant(std::string string_value) : Variant()
{
  impl->variant_type = Variant_type::string;
  impl->string_value = std::move(string_value);
}

Variant::Variant(const bool boolean_value) : Variant()
{
  impl->variant_type = Variant_type::boolean;
  impl->boolean_value = std::move(boolean_value);
}

Variant::Variant(Variant_list list_value) : Variant()
{
  impl->variant_type = Variant_type::list;
  impl->list_value = std::move(list_value);
}

Variant::Variant(Variant_function function_value) : Variant()
{
  impl->variant_type = Variant_type::function;
  impl->function_value = std::move(function_value);
}

auto Variant::type() const noexcept -> Variant_type { return impl->variant_type; }

auto Variant::number() const -> double
{
  if (type() != Variant_type::number) {
    throw std::runtime_error("Variant is not of type number.");
  }
  return impl->number_value;
}

const std::string &Variant::string() const
{
  if (type() != Variant_type::string) {
    throw std::runtime_error("Variant is not of type string.");
  }
  return impl->string_value;
}

auto Variant::boolean() const -> bool
{
  if (type() != Variant_type::boolean) {
    throw std::runtime_error("Variant is not of type boolean.");
  }
  return impl->boolean_value;
}

const Variant_list &Variant::list() const
{
  if (type() != Variant_type::list) {
    throw std::runtime_error("Variant is not of type list.");
  }
  return impl->list_value;
}

const Variant_function &Variant::function() const
{
  if (type() != Variant_type::function) {
    throw std::runtime_error("Variant is not of type function.");
  }
  return impl->function_value;
}

auto string_from(const Variant &variant) -> std::string
{
  switch (variant.type()) {
  case Variant_type::nil:
    return "nil";
  case Variant_type::number:
    return std::to_string(variant.number());
  case Variant_type::string:
    return variant.string();
  case Variant_type::boolean:
    return variant.boolean() ? "true" : "false";
  case Variant_type::list:
    return "[list]";
  case Variant_type::function:
    return "[function]";
  }
  return "unknown";
}

auto operator==(const Variant &left, const Variant &right) -> bool
{
  if (left.type() != right.type()) {
    return false;
  }
  switch (left.type()) {
  case Variant_type::nil:
    return true;
  case Variant_type::number:
    return std::abs(left.number() - right.number()) < 0.00001;
  case Variant_type::boolean:
    return left.boolean() == right.boolean();
  case Variant_type::string:
    return left.string() == right.string();
  case Variant_type::list:
    return left.list() == right.list();
  case Variant_type::function:
    return true;
  }
  return false;
}

auto operator!=(const Variant &left, const Variant &right) -> bool { return !(left == right); }

auto operator+(const Variant &left, const Variant &right) -> Variant { return Variant(left.number() + right.number()); }

auto operator-(const Variant &left, const Variant &right) -> Variant { return Variant(left.number() - right.number()); }

auto operator*(const Variant &left, const Variant &right) -> Variant { return Variant(left.number() * right.number()); }

auto operator/(const Variant &left, const Variant &right) -> Variant
{
  if (right.number() == 0.0) {
    throw std::runtime_error("Divide by zero.");
  }
  return Variant(left.number() / right.number());
}

auto operator<(const Variant &left, const Variant &right) -> Variant { return Variant(left.number() < right.number()); }

auto operator>(const Variant &left, const Variant &right) -> Variant { return Variant(left.number() > right.number()); }

auto operator<=(const Variant &left, const Variant &right) -> Variant
{
  return Variant(left.number() <= right.number());
}

auto operator>=(const Variant &left, const Variant &right) -> Variant
{
  return Variant(left.number() >= right.number());
}

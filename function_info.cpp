#include "internal.hpp"
#include <sstream>

struct Function_info::Impl final {
  std::string name = "";
  std::size_t line_number = 0;
  std::size_t column_number = 0;
};

Function_info::Function_info(std::string name, const std::size_t line_number, const std::size_t column_number) noexcept
    : impl(std::make_shared<Impl>())
{
  impl->name = std::move(name);
  impl->line_number = line_number;
  impl->column_number = column_number;
}

const std::string &Function_info::name() const noexcept { return impl->name; }

auto Function_info::line_number() const noexcept -> std::size_t { return impl->line_number; }

auto Function_info::column_number() const noexcept -> std::size_t { return impl->column_number; }

auto function_info_from(const Token &token) -> Function_info
{
  if (token.type() != Token_type::identifier) {
    auto os = std::ostringstream();
    os << "Token '" << token.value() << "' must be an identifier to create function information from it.";
    throw std::runtime_error(os.str());
  }
  return Function_info(token.value(), token.line_number(), token.column_number());
}

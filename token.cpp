#include "internal.hpp"

auto string_from(const Token_type &token_type) -> std::string
{
  switch (token_type) {
  case Token_type::nil:
    return "nil";
  case Token_type::number:
    return "number";
  case Token_type::string:
    return "string";
  case Token_type::boolean:
    return "boolean";
  case Token_type::identifier:
    return "identifier";
  case Token_type::left_parenthesis:
    return "left_parenthesis";
  case Token_type::right_parenthesis:
    return "right_parenthesis";
  }
  return "unknown";
}

struct Token::Impl final {
  std::string token_value = "";
  Token_type token_type = Token_type::nil;
  char padding[4] = {0};
};

Token::Token() : impl(std::make_shared<Impl>()) {}

Token::Token(const Token_type token_type, std::string token_value) : Token()
{
  impl->token_value = std::move(token_value);
  impl->token_type = token_type;
}

const Token_type &Token::type() const noexcept { return impl->token_type; }

const std::string &Token::value() const noexcept { return impl->token_value; }

auto consume_from(Token_list &token_list) -> Token
{
  auto token = token_list.front();
  token_list.erase(std::begin(token_list));
  return token;
}

auto string_from(const Token &token) noexcept -> std::string
{
  return std::string("{") + string_from(token.type()) + "," + token.value() + "}";
}

auto variant_from(const Token &token) -> Variant
{
  switch (token.type()) {
  case Token_type::nil:
    return Variant();
  case Token_type::number:
    return Variant(std::stod(token.value()));
  case Token_type::string:
    return Variant(std::string(std::cbegin(token.value()) + 1, std::cend(token.value()) - 1));
  case Token_type::boolean:
    return Variant(token.value() == "#t" ? true : false);
  case Token_type::identifier:
  case Token_type::left_parenthesis:
  case Token_type::right_parenthesis:
    break;
  }
  throw std::runtime_error("Can't convert token to variant.");
}

auto operator==(const Token &left, const Token &right) -> bool
{
  return left.type() == right.type() && left.value() == right.value();
}

auto operator!=(const Token &left, const Token &right) -> bool { return !(left == right); }

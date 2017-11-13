#include "internal.hpp"

auto parse_begin_from(Token_list &token_list) -> AST
{
  auto token = consume_from(token_list);
  auto ast_list = AST_list();
  while (token_list.front().type() != Token_type::right_parenthesis) {
    ast_list.emplace_back(parse_from(token_list));
  }
  consume_from(token_list);
  return List(ast_list).clone();
}

auto parse_lambda_from(Token_list &token_list) -> AST
{
  auto token = consume_from(token_list);
  auto parameters = Token_list();
  consume_from(token_list);
  while (token_list.front().type() != Token_type::right_parenthesis) {
    parameters.emplace_back(consume_from(token_list));
  }
  if (consume_from(token_list).type() != Token_type::right_parenthesis) {
    throw std::runtime_error("Syntax error.");
  }
  auto body = parse_from(token_list);
  if (consume_from(token_list).type() != Token_type::right_parenthesis) {
    throw std::runtime_error("Syntax error.");
  }
  return Lambda(parameters, body).clone();
}

auto parse_if_from(Token_list &token_list) -> AST
{
  auto token = consume_from(token_list);
  auto test = parse_from(token_list);
  auto consequent = parse_from(token_list);
  auto alternate = parse_from(token_list);
  if (consume_from(token_list).type() != Token_type::right_parenthesis) {
    throw std::runtime_error("Syntax error.");
  }
  return If(test, consequent, alternate).clone();
}

auto parse_set_from(Token_list &token_list) -> AST
{
  auto token = consume_from(token_list);
  auto identifier = consume_from(token_list);
  auto value = parse_from(token_list);
  if (consume_from(token_list).type() != Token_type::right_parenthesis) {
    throw std::runtime_error("Syntax error.");
  }
  return Set(identifier, value).clone();
}

auto parse_operation_from(Token_list &token_list) -> AST
{
  auto token = consume_from(token_list);
  auto left = parse_from(token_list);
  auto right = parse_from(token_list);
  if (consume_from(token_list).type() != Token_type::right_parenthesis) {
    throw std::runtime_error("Syntax error.");
  }
  return Operator(token, left, right).clone();
}

auto parse_print_line_from(Token_list &token_list) -> AST
{
  auto token = consume_from(token_list);
  auto parameter = parse_from(token_list);
  if (consume_from(token_list).type() != Token_type::right_parenthesis) {
    throw std::runtime_error("Syntax error.");
  }
  return Print_line(parameter).clone();
}

auto parse_procedure_from(Token_list &token_list) -> AST
{
  auto token = consume_from(token_list);
  auto ast_list = AST_list();
  while (token_list.front().type() != Token_type::right_parenthesis) {
    ast_list.emplace_back(parse_from(token_list));
  }
  consume_from(token_list);
  auto arguments = List(ast_list).clone();
  return Procedure(token, arguments).clone();
}

auto parse_from(Token_list &token_list) -> AST
{
  if (token_list.front().type() == Token_type::left_parenthesis) {
    consume_from(token_list);
    const auto &identifier = token_list.front().value();
    if (identifier == "begin") {
      return parse_begin_from(token_list);
    }
    if (identifier == "lambda") {
      return parse_lambda_from(token_list);
    }
    if (identifier == "if") {
      return parse_if_from(token_list);
    }
    if (identifier == "set") {
      return parse_set_from(token_list);
    }
    if (identifier == "+" || identifier == "-" || identifier == "*" || identifier == "/" || identifier == "<" ||
        identifier == ">" || identifier == "<=" || identifier == ">=" || identifier == "=") {
      return parse_operation_from(token_list);
    }
    if (identifier == "print-line") {
      return parse_print_line_from(token_list);
    }
    return parse_procedure_from(token_list);
  }
  auto token = consume_from(token_list);
  if (token.type() == Token_type::identifier) {
    return Variable(token).clone();
  }
  if (token.type() == Token_type::number || token.type() == Token_type::boolean || token.type() == Token_type::string ||
      token.type() == Token_type::nil) {
    return Atomic(token).clone();
  }
  throw std::runtime_error("Unknown token type.");
}

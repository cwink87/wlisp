#include "internal.hpp"
#include <algorithm>
#include <regex>

/*!
 * \todo Break this out into multiple functions.
 */

auto lexical_analysis(const std::string &input) -> Token_list
{
  auto input_copy = input;
  auto token_list = Token_list();
  auto match = std::smatch();
  auto left_parenthesis_count = static_cast<std::size_t>(0);
  auto right_parenthesis_count = static_cast<std::size_t>(0);
  auto line_number = static_cast<std::size_t>(0);
  auto column_number = static_cast<std::size_t>(1);
  for (;;) {
    if (input_copy.empty()) {
      break;
    }
    if (std::regex_search(input_copy, match, std::regex(R"(\n|\r)")) && match.position() == 0) {
      input_copy = match.suffix();
      ++line_number;
      column_number = 1;
      continue;
    }
    if (std::regex_search(input_copy, match, std::regex(R"(\s|\t)")) && match.position() == 0) {
      input_copy = match.suffix();
      ++column_number;
      continue;
    }
    if (std::regex_search(input_copy, match, std::regex(R"("(\\"|\\r|\\n|\\t|[^"])*")")) && match.position() == 0) {
      token_list.emplace_back(Token(Token_type::string, match[0].str(), line_number, column_number));
      column_number += static_cast<std::size_t>(match[0].length());
      input_copy = match.suffix();
      continue;
    }
    if (std::regex_search(input_copy, match, std::regex(R"(-?[.]?[0-9]+[.]?[0-9]*)")) && match.position() == 0) {
      token_list.emplace_back(Token(Token_type::number, match[0].str(), line_number, column_number));
      column_number += static_cast<std::size_t>(match[0].length());
      input_copy = match.suffix();
      continue;
    }
    if (std::regex_search(input_copy, match, std::regex(R"(#t|#f)")) && match.position() == 0) {
      token_list.emplace_back(Token(Token_type::boolean, match[0].str(), line_number, column_number));
      column_number += static_cast<std::size_t>(match[0].length());
      input_copy = match.suffix();
      continue;
    }
    if (std::regex_search(input_copy, match, std::regex(R"(nil)")) && match.position() == 0) {
      token_list.emplace_back(Token(Token_type::nil, match[0].str(), line_number, column_number));
      column_number += static_cast<std::size_t>(match[0].length());
      input_copy = match.suffix();
      continue;
    }
    if (std::regex_search(input_copy, match, std::regex(R"([(])")) && match.position() == 0) {
      ++left_parenthesis_count;
      token_list.emplace_back(Token(Token_type::left_parenthesis, match[0].str(), line_number, column_number));
      column_number += static_cast<std::size_t>(match[0].length());
      input_copy = match.suffix();
      continue;
    }
    if (std::regex_search(input_copy, match, std::regex(R"([)])")) && match.position() == 0) {
      ++right_parenthesis_count;
      token_list.emplace_back(Token(Token_type::right_parenthesis, match[0].str(), line_number, column_number));
      column_number += static_cast<std::size_t>(match[0].length());
      input_copy = match.suffix();
      continue;
    }
    if (std::regex_search(input_copy, match, std::regex(R"(<=|>=|<|>|=|\+|-|\*|\/)")) && match.position() == 0) {
      token_list.emplace_back(Token(Token_type::identifier, match[0].str(), line_number, column_number));
      column_number += static_cast<std::size_t>(match[0].length());
      input_copy = match.suffix();
      continue;
    }
    if (std::regex_search(input_copy, match, std::regex(R"([a-z]+-?[a-z]+|[a-z])")) && match.position() == 0) {
      token_list.emplace_back(Token(Token_type::identifier, match[0].str(), line_number, column_number));
      column_number += static_cast<std::size_t>(match[0].length());
      input_copy = match.suffix();
      continue;
    }
    auto os = std::ostringstream();
    os << "Syntax error occured at line " << line_number << " column " << column_number << ".";
    throw std::runtime_error(os.str());
  }
  if (left_parenthesis_count != right_parenthesis_count) {
    throw std::runtime_error("For every '(' there must be a ')'.");
  }
  return token_list;
}

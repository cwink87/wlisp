#include "internal.hpp"

auto interpret(Environment environment, const std::string &input) -> Variant
{
  auto tokens = lexical_analysis(input);
  auto parsed = parse_from(tokens);
  return parsed->execute(environment, empty_variant_list);
}

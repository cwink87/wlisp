#include "internal.hpp"
#include <iostream>
#include <sstream>

AST_base::~AST_base() noexcept = default;

struct If::Impl final {
  AST test = AST();
  AST consequent = AST();
  AST alternate = AST();
};

If::If(AST test, AST consequent, AST alternate) noexcept : impl(std::make_shared<Impl>())
{
  impl->test = std::move(test);
  impl->consequent = std::move(consequent);
  impl->alternate = std::move(alternate);
}

auto If::clone() const noexcept -> AST { return std::make_shared<If>(*this); }

auto If::execute(Environment environment, const Variant_list &variant_list) const -> Variant
{
  auto tested = impl->test->execute(environment, variant_list);
  if (tested.boolean()) {
    return impl->consequent->execute(environment, variant_list);
  }
  return impl->alternate->execute(environment, variant_list);
}

If::~If() noexcept = default;

struct Procedure::Impl final {
  Token identifier = Token();
  AST arguments = AST();
};

Procedure::Procedure(Token identifier, AST arguments) noexcept : impl(std::make_shared<Impl>())
{
  impl->identifier = std::move(identifier);
  impl->arguments = std::move(arguments);
}

auto Procedure::clone() const noexcept -> AST { return std::make_shared<Procedure>(*this); }

auto Procedure::execute(Environment environment, const Variant_list &variant_list) const -> Variant
{
  if (!environment->has(impl->identifier.value())) {
    auto os = std::ostringstream();
    os << "Could not find procedure '" << impl->identifier.value() << "' at line " << impl->identifier.line_number()
       << " column " << impl->identifier.column_number() << " in environment.";
    throw std::runtime_error(os.str());
  }
  auto function = environment->get(impl->identifier.value()).function();
  auto evaluated = function(function_info_from(impl->identifier), environment,
                            impl->arguments->execute(environment, variant_list).list());
  return evaluated;
}

Procedure::~Procedure() noexcept = default;

struct Lambda::Impl final {
  Token_list parameters = Token_list();
  AST body = AST();
};

Lambda::Lambda(Token_list parameters, AST body) : impl(std::make_shared<Impl>())
{
  impl->parameters = std::move(parameters);
  impl->body = std::move(body);
}

auto Lambda::clone() const noexcept -> AST { return std::make_shared<Lambda>(*this); }

auto Lambda::execute(Environment, const Variant_list &) const -> Variant
{
  auto parameters = impl->parameters;
  auto body = impl->body;
  return Variant([parameters, body](const Function_info &info, Environment environment, const Variant_list &arguments) {
    auto new_environment = create_environment();
    if (arguments.size() != parameters.size()) {
      auto os = std::ostringstream();
      os << "Procedure '" + info.name() + "' at line " << info.line_number() << " column " << info.column_number()
         << " has an invalid number of arguments.";
      throw std::runtime_error(os.str());
    }
    auto k = std::cbegin(arguments);
    for (auto i = std::cbegin(parameters), j = std::cend(parameters); i != j; ++i, ++k) {
      new_environment->set(i->value(), *k);
    }
    new_environment->parent(environment);
    return body->execute(new_environment, arguments);
  });
}

Lambda::~Lambda() noexcept = default;

struct List::Impl final {
  AST_list ast_list;
};

List::List(AST_list ast_list) : impl(std::make_shared<Impl>()) { impl->ast_list = std::move(ast_list); }

auto List::clone() const noexcept -> AST { return std::make_shared<List>(*this); }

auto List::execute(Environment environment, const Variant_list &variant_list) const -> Variant
{
  auto list = Variant_list();
  for (const auto &item : impl->ast_list) {
    list.emplace_back(item->execute(environment, variant_list));
  }
  return Variant(list);
}

List::~List() noexcept = default;

struct Operator::Impl final {
  Token operation = Token();
  AST left = AST();
  AST right = AST();
};

Operator::Operator(Token operation, AST left, AST right) : impl(std::make_shared<Impl>())
{
  impl->operation = std::move(operation);
  impl->left = std::move(left);
  impl->right = std::move(right);
}

auto Operator::clone() const noexcept -> AST { return std::make_shared<Operator>(*this); }

auto Operator::execute(Environment environment, const Variant_list &variant_list) const -> Variant
{
  if (impl->operation.value() == "+") {
    return impl->left->execute(environment, variant_list) + impl->right->execute(environment, variant_list);
  }
  if (impl->operation.value() == "-") {
    return impl->left->execute(environment, variant_list) - impl->right->execute(environment, variant_list);
  }
  if (impl->operation.value() == "*") {
    return impl->left->execute(environment, variant_list) * impl->right->execute(environment, variant_list);
  }
  if (impl->operation.value() == "/") {
    return impl->left->execute(environment, variant_list) / impl->right->execute(environment, variant_list);
  }
  if (impl->operation.value() == "<") {
    return impl->left->execute(environment, variant_list) < impl->right->execute(environment, variant_list);
  }
  if (impl->operation.value() == ">") {
    return impl->left->execute(environment, variant_list) > impl->right->execute(environment, variant_list);
  }
  if (impl->operation.value() == "<=") {
    return impl->left->execute(environment, variant_list) <= impl->right->execute(environment, variant_list);
  }
  if (impl->operation.value() == ">=") {
    return impl->left->execute(environment, variant_list) >= impl->right->execute(environment, variant_list);
  }
  if (impl->operation.value() == "=") {
    return Variant(impl->left->execute(environment, variant_list) == impl->right->execute(environment, variant_list));
  }
  auto os = std::ostringstream();
  os << "Invalid operation '" << impl->operation.value() << "' at line " << impl->operation.line_number() << " column "
     << impl->operation.column_number() << ".";
  throw std::runtime_error(os.str());
}

Operator::~Operator() noexcept = default;

struct Print_line::Impl final {
  AST expression = AST();
};

Print_line::Print_line(AST expression) : impl(std::make_shared<Impl>()) { impl->expression = std::move(expression); }

auto Print_line::clone() const noexcept -> AST { return std::make_shared<Print_line>(*this); }

auto Print_line::execute(Environment environment, const Variant_list &variant_list) const -> Variant
{
  std::cout << string_from(impl->expression->execute(environment, variant_list)) << std::endl;
  return Variant();
}

Print_line::~Print_line() noexcept = default;

struct Variable::Impl final {
  Token token = Token();
};

Variable::Variable(Token token) : impl(std::make_shared<Impl>()) { impl->token = std::move(token); }

auto Variable::clone() const noexcept -> AST { return std::make_shared<Variable>(*this); }

auto Variable::execute(Environment environment, const Variant_list &) const -> Variant
{
  if (!environment->has(impl->token.value())) {
    auto os = std::ostringstream();
    os << "Could not find variable '" << impl->token.value() << "' at line " << impl->token.line_number() << " column "
       << impl->token.column_number() << " in environment.";
    throw std::runtime_error(os.str());
  }
  return environment->get(impl->token.value());
}

Variable::~Variable() noexcept = default;

struct Set::Impl final {
  Token identifier = Token();
  AST value = AST();
};

Set::Set(Token identifier, AST value) : impl(std::make_shared<Impl>())
{
  impl->identifier = std::move(identifier);
  impl->value = std::move(value);
}

auto Set::clone() const noexcept -> AST { return std::make_shared<Set>(*this); }

auto Set::execute(Environment environment, const Variant_list &variant_list) const -> Variant
{
  environment->set(impl->identifier.value(), impl->value->execute(environment, variant_list));
  return Variant();
}

Set::~Set() noexcept = default;

struct Atomic::Impl final {
  Token token = Token();
};

Atomic::Atomic(Token token) : impl(std::make_shared<Impl>()) { impl->token = std::move(token); }

auto Atomic::clone() const noexcept -> AST { return std::make_shared<Atomic>(*this); }

auto Atomic::execute(Environment, const Variant_list &) const -> Variant { return variant_from(impl->token); }

Atomic::~Atomic() noexcept = default;

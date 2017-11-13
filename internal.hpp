#ifndef INTERNAL_HPP
#define INTERNAL_HPP

/*!
 * \todo Document this file.
 */

#include "wlisp.hpp"

enum class Token_type { nil, number, string, boolean, identifier, left_parenthesis, right_parenthesis };

auto string_from(const Token_type &token_type) -> std::string;

class Token final {
public:
  Token();
  Token(const Token_type token_type, std::string token_value);
  Token(const Token_type token_type, std::string token_value, const std::size_t line_number,
        const std::size_t column_number);

  ~Token() noexcept = default;
  Token(const Token &) = default;
  Token(Token &&) noexcept = default;
  Token &operator=(const Token &) = default;
  Token &operator=(Token &&) noexcept = default;

  const Token_type &type() const noexcept;
  const std::string &value() const noexcept;
  auto line_number() const noexcept -> std::size_t;
  auto column_number() const noexcept -> std::size_t;

private:
  struct Impl;
  std::shared_ptr<Impl> impl;
};

using Token_list = std::vector<Token>;

auto consume_from(Token_list &token_list) -> Token;
auto string_from(const Token &token) noexcept -> std::string;
auto variant_from(const Token &token) -> Variant;

auto operator==(const Token &left, const Token &right) -> bool;
auto operator!=(const Token &left, const Token &right) -> bool;

auto lexical_analysis(const std::string &input) -> Token_list;

class AST_base;

using AST = std::shared_ptr<AST_base>;

using AST_list = std::vector<AST>;

class AST_base {
public:
  AST_base() noexcept = default;
  virtual ~AST_base() noexcept;
  AST_base(const AST_base &) noexcept = default;
  AST_base(AST_base &&) noexcept = default;
  AST_base &operator=(const AST_base &) noexcept = default;
  AST_base &operator=(AST_base &&) noexcept = default;

  virtual auto clone() const noexcept -> AST = 0;
  virtual auto execute(Environment environment, const Variant_list &) const -> Variant = 0;
};

class If : public AST_base {
public:
  If(AST test, AST consequent, AST alternate) noexcept;

  If() noexcept = delete;
  virtual ~If() noexcept;
  If(const If &) noexcept = default;
  If(If &&) noexcept = default;
  If &operator=(const If &) noexcept = default;
  If &operator=(If &&) noexcept = default;

  auto clone() const noexcept -> AST;
  auto execute(Environment environment, const Variant_list &) const -> Variant;

private:
  struct Impl;
  std::shared_ptr<Impl> impl;
};

class Procedure final : public AST_base {
public:
  Procedure(Token identifier, AST arguments) noexcept;

  Procedure() = delete;
  virtual ~Procedure() noexcept;
  Procedure(const Procedure &) = default;
  Procedure(Procedure &&) noexcept = default;
  Procedure &operator=(const Procedure &) = default;
  Procedure &operator=(Procedure &&) noexcept = default;

  auto clone() const noexcept -> AST;
  auto execute(Environment environment, const Variant_list &variant_list) const -> Variant;

private:
  struct Impl;
  std::shared_ptr<Impl> impl;
};

class Lambda final : public AST_base {
public:
  Lambda(Token_list parameters, AST body);

  Lambda() = delete;
  virtual ~Lambda() noexcept;
  Lambda(const Lambda &) = default;
  Lambda(Lambda &&) noexcept = default;
  Lambda &operator=(const Lambda &) = default;
  Lambda &operator=(Lambda &&) noexcept = default;

  auto clone() const noexcept -> AST;
  auto execute(Environment environment, const Variant_list &) const -> Variant;

private:
  struct Impl;
  std::shared_ptr<Impl> impl;
};

class List final : public AST_base {
public:
  explicit List(AST_list ast_list);

  List() noexcept = delete;
  virtual ~List() noexcept;
  List(const List &) = default;
  List(List &&) noexcept = default;
  List &operator=(const List &) = default;
  List &operator=(List &&) noexcept = default;

  auto clone() const noexcept -> AST;
  auto execute(Environment environment, const Variant_list &variant_list) const -> Variant;

private:
  struct Impl;
  std::shared_ptr<Impl> impl;
};

class Operator final : public AST_base {
public:
  explicit Operator(Token operation, AST left, AST right);

  Operator() = delete;
  virtual ~Operator() noexcept;
  Operator(const Operator &) = default;
  Operator(Operator &&) noexcept = default;
  Operator &operator=(const Operator &) = default;
  Operator &operator=(Operator &&) = default;

  auto clone() const noexcept -> AST;
  auto execute(Environment environment, const Variant_list &variant_list) const -> Variant;

private:
  struct Impl;
  std::shared_ptr<Impl> impl;
};

class Print_line final : public AST_base {
public:
  explicit Print_line(AST expression);

  Print_line() = delete;
  virtual ~Print_line() noexcept;
  Print_line(const Print_line &) noexcept = default;
  Print_line(Print_line &&) noexcept = default;
  Print_line &operator=(const Print_line &) noexcept = default;
  Print_line &operator=(Print_line &&) noexcept = default;

  auto clone() const noexcept -> AST;
  auto execute(Environment environment, const Variant_list &variant_list) const -> Variant;

private:
  struct Impl;
  std::shared_ptr<Impl> impl;
};

class Variable final : public AST_base {
public:
  explicit Variable(Token token);

  Variable() = delete;
  virtual ~Variable() noexcept;
  Variable(const Variable &) = default;
  Variable(Variable &&) noexcept = default;
  Variable &operator=(const Variable &) = default;
  Variable &operator=(Variable &&) = default;

  auto clone() const noexcept -> AST;
  auto execute(Environment environment, const Variant_list &) const -> Variant;

private:
  struct Impl;
  std::shared_ptr<Impl> impl;
};

class Set final : public AST_base {
public:
  explicit Set(Token identifier, AST value);

  Set() = delete;
  virtual ~Set() noexcept;
  Set(const Set &) = default;
  Set(Set &&) noexcept = default;
  Set &operator=(const Set &) = default;
  Set &operator=(Set &&) = default;

  auto clone() const noexcept -> AST;
  auto execute(Environment environment, const Variant_list &variant_list) const -> Variant;

private:
  struct Impl;
  std::shared_ptr<Impl> impl;
};

class Atomic final : public AST_base {
public:
  explicit Atomic(Token token);

  Atomic() = delete;
  virtual ~Atomic() noexcept;
  Atomic(const Atomic &) = default;
  Atomic(Atomic &&) noexcept = default;
  Atomic &operator=(const Atomic &) = default;
  Atomic &operator=(Atomic &&) = default;

  auto clone() const noexcept -> AST;
  auto execute(Environment environment, const Variant_list &) const -> Variant;

private:
  struct Impl;
  std::shared_ptr<Impl> impl;
};

auto parse_from(Token_list &token_list) -> AST;

auto function_info_from(const Token &token) -> Function_info;

#endif // INTERNAL_HPP

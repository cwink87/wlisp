#ifndef WLISP_HPP
#define WLISP_HPP

#include <functional>
#include <memory>
#include <vector>

/*!
 * \brief The Function_info class contains information on a function (name, line number, and column number).
 */
class Function_info final {
public:
  Function_info(std::string name, const std::size_t line_number, const std::size_t column_number) noexcept;

  Function_info() = delete;
  ~Function_info() noexcept = default;
  Function_info(const Function_info &) noexcept = default;
  Function_info(Function_info &&) noexcept = default;
  Function_info &operator=(const Function_info &) noexcept = default;
  Function_info &operator=(Function_info &&) noexcept = default;

  const std::string &name() const noexcept;
  auto line_number() const noexcept -> std::size_t;
  auto column_number() const noexcept -> std::size_t;

private:
  struct Impl;
  std::shared_ptr<Impl> impl;
};

class Environment_base;

/*!
 * \brief Environment objects (stores key value pairs std::string and Variant).
 *        Note: don't call this directly, use the create_environment methods.
 */
using Environment = std::shared_ptr<Environment_base>;

enum class Variant_type { nil, number, string, boolean, list, function };

/*!
 * \brief Returns a string representation of the Variant type.
 * \param variant_type The variant type.
 * \return The string representation.
 */
auto string_from(const Variant_type &variant_type) -> std::string;

class Variant;

using Variant_list = std::vector<Variant>;

/*!
 * \brief Represents a function variant. These can be used to extend the environment. For instance,
 *        you can add your own functionality by running:
 *
 *          auto env = create_environment();
 *          env.set("addition",
 *            [](const Function &info, Environment environment, const Variant_list &arguments) {
 *              if (arguments.size() != 2) {
 *                auto os = std::ostringstream();
 *                os << "Invalid number of arguments for '" << identifier.value() << "' on line "
 *                   << identifier.line_number() << " column " << identifier.column_number() << ".";
 *                throw std::runtime_error(os.str());
 *              return arguments[0] + arguments[1];
 *          });
 *
 *        where info is a Function_info object that contains the name of the function and its location in the
 *        code, environment is the current environment passed, and arguments is the arguments for
 *        your function.
 *
 *        This peice of code would be called like so,
 *          (addition 5 4)
 */
using Variant_function = std::function<Variant(const Function_info &, Environment, const Variant_list &)>;

/*!
 * \brief The Variant class is used to communicate between the interpreter and
 *        the developer, i.e. Variants are passed to the lisp code and are also
 *        returned from the lisp code. Operations for equivalence are universal
 *        to all types in the Variant. Other operations are specific to the number
 *        variant.
 */
class Variant final {
public:
  Variant();
  explicit Variant(const double number_value);
  explicit Variant(std::string string_value);
  explicit Variant(const bool boolean_value);
  explicit Variant(Variant_list list_value);
  explicit Variant(Variant_function function_value);

  ~Variant() noexcept = default;
  Variant(const Variant &) = default;
  Variant(Variant &&) noexcept = default;
  Variant &operator=(const Variant &) = default;
  Variant &operator=(Variant &&) noexcept = default;

  auto type() const noexcept -> Variant_type;
  auto number() const -> double;
  const std::string &string() const;
  auto boolean() const -> bool;
  const Variant_list &list() const;
  const Variant_function &function() const;

private:
  struct Impl;
  std::shared_ptr<Impl> impl;
};

/*!
 * \brief A general empty variant list to use (so you don't have to allocate every time).
 */
static const auto empty_variant_list = Variant_list();

/*!
 * \brief Returns the string representation of the given variant.
 * \param variant Variant to get the string representation of.
 * \return The string representation.
 */
auto string_from(const Variant &variant) -> std::string;

auto operator==(const Variant &left, const Variant &right) -> bool;
auto operator!=(const Variant &left, const Variant &right) -> bool;
auto operator+(const Variant &left, const Variant &right) -> Variant;
auto operator-(const Variant &left, const Variant &right) -> Variant;
auto operator*(const Variant &left, const Variant &right) -> Variant;
auto operator/(const Variant &left, const Variant &right) -> Variant;
auto operator<(const Variant &left, const Variant &right) -> Variant;
auto operator>(const Variant &left, const Variant &right) -> Variant;
auto operator<=(const Variant &left, const Variant &right) -> Variant;
auto operator>=(const Variant &left, const Variant &right) -> Variant;

/*!
 * \brief The Environment_base class is the base class for the Environment object. It
 *        contains all current variables in the environment. The local scope is a map
 *        in the object and a link to the parent is made (to keep scope).
 *        Note 1: All methods traverse the entire tree, there is no "local" methods. If you want
 *        localized variables, create a new environment with no parent, set the values,
 *        then link the parent afterwards.
 *        Note 2: Do not use this class directly, use the Environment object through the
 *        create_environment methods.
 */
class Environment_base final {
public:
  Environment_base() noexcept;
  Environment_base(Environment parent) noexcept;

  ~Environment_base() noexcept = default;
  Environment_base(const Environment_base &) noexcept = default;
  Environment_base(Environment_base &&) noexcept = default;
  Environment_base &operator=(const Environment_base &) noexcept = default;
  Environment_base &operator=(Environment_base &&) noexcept = default;

  auto parent(Environment parent_value) noexcept -> void;
  auto has(const std::string &key) const noexcept -> bool;
  const Variant &get(const std::string &key) const;
  auto set(std::string key, Variant value) noexcept -> void;
  auto to_string() const noexcept -> std::string;

private:
  struct Impl;
  std::shared_ptr<Impl> impl;
};

/*!
 * \brief Create a new environment linking to the given parent.
 * \param parent The parent environment to link to.
 * \return The new environment.
 */
auto create_environment(Environment parent) -> Environment;

/*!
 * \brief Create a new environment with no parent.
 * \return The new environment.
 */
auto create_environment() -> Environment;

/*!
 * \brief Interpret the given string using the given environment.
 * \param environment The environment to use when interpreting.
 * \param input The lisp code to interpret.
 * \return A Variant result from the interpretation.
 */
auto interpret(Environment environment, const std::string &input) -> Variant;

#endif // WLISP_HPP

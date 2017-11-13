#include "wlisp.hpp"

/*!
 * \todo Add line and column information to the Token object.
 * \todo Ensure that all AST based objects accept only AST or Token (to assist with tracking).
 * \todo Create an exception that accepts a token and prints out line/column information from the token object.
 * \todo Implement tail end recursion optimization.
 * \todo Add while implementation.
 */

/*
=======================================================================================================================

  Main

=======================================================================================================================
*/
int main()
{
  auto env = create_environment();
  interpret(env, "(begin (set a 10) (set b 2) (set c (+ a b)) (print-line c))");
  interpret(env, "(begin (print-line (+ (- 10.4 5) 2.1)) (set a 4) (print-line (+ a b)))");
  interpret(env, "(begin (set a (lambda (x y) (+ x y))) (print-line (a 5 4)))");
  interpret(env, "(begin (print-line (if (= 3 2) (+ 1 3) nil)))");

  interpret(env, R"(
             (set f
               (lambda (n)
                 (if (= n 0) 0
                   (if (= n 1) 1
                     (+ (f (- n 2)) (f (- n 1)))
                   )
                 )
               )
             )
  )");
  interpret(env, "(print-line (f 8))");
  return 0;
}

cmake_minimum_required(VERSION 2.8)

project(wlisp)
add_executable(${PROJECT_NAME} "main.cpp" "wlisp.hpp" "token.cpp" "variant.cpp" "parser.cpp" "ast.cpp" "lexer.cpp" "wlisp.cpp" "internal.hpp" "environment.cpp" "function_info.cpp" "functions.cpp")

target_compile_options(${PROJECT_NAME} PUBLIC -Wall -Wextra -O0 -g -pedantic-errors -std=c++14)

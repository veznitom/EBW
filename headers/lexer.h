#ifndef __LEXER__H
#define __LEXER__H
#include <cstddef>
#include <cstdlib>
#include <set>
#include <string>
#include <type_traits>
#include <vector>
#include <iostream>
#include <ostream>

#include "customs.h"

using namespace Customs;

class Lexer {
private:
  bool debug;
  std::set<char> symbols = {'+', '-', '*', '/', '%', '=', ':','"', ','};
  std::set<std::string> functions = {"sum", "sin", "cos", "avg", "pow"};
  void match_funct(Token &token);

public:
  Lexer();
  Lexer(bool debug);
  void print_tokens(std::vector<Token> &tokens);
  std::vector<Token> analyze(std::string sequence);
};
#endif
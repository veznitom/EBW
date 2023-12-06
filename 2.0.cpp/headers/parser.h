#ifndef __PARSER__H
#define __PARSER__H

#include "customs.h"
#include "lexer.h"
#include "tree.h"
#include <cstddef>
#include <vector>

using namespace Customs;

class Parser {
  std::vector<Token> tokens;
  size_t index = 0;

  Lexer lexer = Lexer(true);

  Token peek();
  Token pop();
  void compare(Token token);
  bool ccompare(Token token);

  std::unique_ptr<AST> Value();
  std::unique_ptr<AST> Expresion();
  std::unique_ptr<AST> Term();
  std::unique_ptr<AST> Factor();
  std::unique_ptr<AST> Function();
  std::unique_ptr<AST> Cells();

public:
  std::unique_ptr<AST> Parse(std::string sequence);
};
#endif
#ifndef __PARSER__H
#define __PARSER__H

#include "customs.h"
#include "lexer.h"
#include "tree.h"
#include <cstddef>
#include <typeinfo>
#include <vector>

using namespace Customs;

class Parser {
  std::vector<Token> tokens;

  Lexer lexer = Lexer(true);

  Token peek();
  Token pop();
  Token compare(Token token);

  AST *Value();
  AST *Expresion();
  AST *Term();
  AST *Factor();
  AST *Function();
  AST *Cells();

public:
  AST *Parse(std::string sequence);
};
#endif
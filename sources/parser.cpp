#include "../headers/parser.h"
#include <exception>
#include <string>

Token Parser::peek() {}
Token Parser::pop() {}
Token Parser::compare(Token token) {}

AST *Parser::Value() {}
AST *Parser::Expresion() {}
AST *Parser::Term() {}
AST *Parser::Factor() {}
AST *Parser::Function() {}
AST *Parser::Cells() {}

AST *Parser::Parse(std::string sequence) {
  tokens = lexer.analyze(sequence);
  if (peek().type == SYMBOL && peek().text == "=") {
    pop();
    try {
      return Expresion();
    } catch (std::exception exception) {
      exception.what();
      return nullptr;
    }
  } else
    return Value();
}
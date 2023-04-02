#include "../headers/parser.h"
#include <exception>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

Token Parser::peek() {
  if (index >= tokens.size())
    return Token();
  return tokens[index];
}

Token Parser::pop() {
  if (index >= tokens.size())
    return Token();
  return tokens[index++];
}

void Parser::compare(Token token) {
  if (tokens[index].type == token.type)
    pop();
  else
    throw std::runtime_error("Mismatch with token" +
                             std::to_string(token.position));
}

bool Parser::ccompare(Token token) {
  if (tokens.empty())
    return false;
  return tokens[index].type == token.type;
}

std::unique_ptr<AST> Parser::Value() {
  //std::cout << "Value: " << peek() << std::endl;
  if (peek().type == NUMBER)
    return std::make_unique<Number>(pop().number);
  else
    return std::make_unique<Text>(tokens);
}

std::unique_ptr<AST> Parser::Expresion() {
  //std::cout << "Expression: " << peek() << std::endl;
  auto result = Term();
  while (ccompare(SYMBOL)) {
    if (peek().text != "+" && peek().text != "-")
      return result;
    Token oper = pop();
    if (oper.text == "+" || oper.text == "-") {
      auto term = Term();
      result = std::make_unique<Binary>(std::move(result), oper.text[0],
                                        std::move(term));
    } else
      throw std::runtime_error(
          "Invalid operator:'" + std::to_string(oper.text[0]) +
          "' at position:" + std::to_string(oper.position));
  }
  return result;
}

std::unique_ptr<AST> Parser::Term() {
  //std::cout << "Term: " << peek() << std::endl;
  auto result = Factor();
  while (ccompare(SYMBOL)) {
    if (peek().text != "*" && peek().text != "/")
      return result;
    Token oper = pop();
    if (oper.text == "*" || oper.text == "/") {
      auto factor = Factor();
      result = std::make_unique<Binary>(std::move(result), oper.text[0],
                                        std::move(factor));
    } else
      throw std::runtime_error(
          "Invalid operator:'" + std::to_string(oper.text[0]) +
          "' at position:" + std::to_string(oper.position));
  }
  return result;
}

std::unique_ptr<AST> Parser::Factor() {
  //std::cout << "Factor: " << peek() << std::endl;
  std::vector<Token> accumulator = std::vector<Token>();
  std::unique_ptr<AST> result;
  switch (peek().type) {
  case Customs::NUMBER:
    return std::make_unique<Number>(pop().number);
    break;

  case Customs::FUNCTION:
    return Function();
    break;

  case Customs::SYMBOL:
    if (pop().text[0] == '"') {
      while ((peek().type != SYMBOL && peek().text[0] != '"') ||
             !(peek().type == END || peek().type == UNDEF)) {
        accumulator.push_back(pop());
      }
      if (peek().type == END || peek().type == UNDEF)
        throw std::runtime_error("Unclosed string quotation marks.");
      else
        return std::make_unique<Text>(accumulator);
    }
    break;

  case Customs::LBRACK:
    pop();
    result = Expresion();
    compare(RBRACK);
    return result;
    break;

    /*case Customs::TEXT:
      return Cells();
      break;*/

  default:
    throw std::runtime_error("Invalid syntax");
    break;
  }
  return nullptr;
}

std::unique_ptr<AST> Parser::Function() {
  //std::cout << "Function: " << peek() << std::endl;
  auto funct = std::make_unique<class Function>();
  funct->function = peek().text;
  funct->children = std::vector<std::unique_ptr<AST>>();
  pop();
  compare(Token(LBRACK));
  while (peek().type != RBRACK && peek().type != END && peek().type != UNDEF) {
    funct->children.push_back(Expresion());
    if (ccompare(Token(SYMBOL)) && peek().text[0] == ',') {
      pop();
    } else if (ccompare(Token(RBRACK)))
      break;
    else {
      throw std::runtime_error("Missing colon at:" +
                               std::to_string(peek().position));
    }
  }
  if (peek().type == END || peek().type == UNDEF)
    throw std::runtime_error("Invalid syntax");
  return funct;
}

// std::unique_ptr<AST> Parser::Cells() { return std::make_unique<Cell>(); }

std::unique_ptr<AST> Parser::Parse(std::string sequence) {
  tokens = lexer.analyze(sequence);
  //std::cout << "Parse: " << peek() << std::endl;
  if (peek().type == SYMBOL && peek().text[0] == '=') {
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
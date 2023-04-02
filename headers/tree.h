#ifndef __AST__H
#define __AST__H

#include "table.h"
#include "customs.h"
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <tuple>
#include <vector>
#include <cstddef>
#include <limits>

using namespace Customs;

class AST {
protected:
  EvalState state{};

public:
  virtual Value get_value() = 0;
  virtual void print() = 0;
};

class Number : public AST {
public:
  int number;
  Number(int number);
  virtual Value get_value() override;
  virtual void print() override;
};

class Text : public AST {
public:
  std::string text;
  Text(std::string text);
  Text(std::vector<Token> tokens);
  virtual Value get_value() override;
  virtual void print() override;
};

class Binary : public AST {
public:
  std::unique_ptr<AST> left_child, right_child;
  char symbol;
  Binary(std::unique_ptr<AST> left, char oper ,std::unique_ptr<AST> right);
  virtual Value get_value() override;
  virtual void print() override;
};

/*class Cell : public AST {
public:
  Table *table;
  std::vector<Coordinates> indexes;
  Value get_value() override;
  void print() override;
};*/

class Function : public AST {
public:
  std::vector<std::unique_ptr<AST>> children;
  std::string function;
  virtual Value get_value() override;
  virtual void print() override;
};

#endif
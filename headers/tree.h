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
  EvalState state;

public:
  virtual Value get_value();
};

class Number : public AST {
public:
  int number;
  Number(int number);
  Value get_value() override;
};

class Text : public AST {
public:
  std::string text;
  Text(std::string text);
  Value get_value() override;
};

class Binary : public AST {
public:
  std::unique_ptr<AST> left_child, right_child;
  char symbol;
  Value get_value() override;
};

class Cell : public AST {
public:
  Table *table;
  std::vector<Coordinates> indexes;
  Value get_value() override;
};

class Function : public AST {
public:
  std::vector<std::unique_ptr<AST>> children;
  std::string function;
  Value get_value() override;
};

#endif
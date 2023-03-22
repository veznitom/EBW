#include "types.cpp"
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <string>

#define PI 3.1415926535897932384626433

class AST {
protected:
  Customs::EvalState state;

public:
  virtual Customs::Value get_value();
};

class Number : AST {
  int number;
  Customs::Value get_value() override {
    Customs::Value value;
    value.type = Customs::NUM;
    value.number = number;
    return value;
  }
};

class Text : AST {
  std::string text;
  Customs::Value get_value() override {
    Customs::Value value;
    value.type = Customs::NUM;
    value.text = text;
    return value;
  }
};

class Binary : AST {
  AST left_child, right_child;
  char symbol;
  Customs::Value get_value() override {
    Customs::Value left_val, right_val, result;
    left_val = left_child.get_value();
    right_val = right_child.get_value();
    if (left_val.type != Customs::NUM || right_val.type != Customs::NUM) {
      result.type = Customs::ERR;
      return result;
    }
    result.type = Customs::NUM;
    switch (symbol) {
    case '+':
      result.number = left_val.number + right_val.number;
      break;
    case '-':
      result.number = left_val.number - right_val.number;
      break;
    case '*':
      result.number = left_val.number * right_val.number;
      break;
    case '/':
      if (right_val.number == 0)
        result.number = INT32_MAX;
      else
        result.number = left_val.number / right_val.number;
      break;
    case '%':
      result.number = left_val.number % right_val.number;
      break;
    }
    return result;
  }
};

class Cell : AST {};

class Function : AST {
  AST child;
  std::string function;
  Customs::Value get_value() override {
    Customs::Value child_val, result;
    child_val = child.get_value();
    if (child_val.type != Customs::NUM || child_val.type != Customs::RNG) {
      result.type = Customs::ERR;
      return result;
    }
    result.type = Customs::NUM;
    if (child_val.type == Customs::NUM) {
      if (function == "sin")
        result.number = std::sin((child_val.number * PI) / 180);
      if (function == "cos")
        result.number = std::cos((child_val.number * PI) / 180);
    }
    if (child_val.type == Customs::RNG) {
      if (function == "sum") {
        int sum = 0;
        for (size_t i = 0; i < child_val.cells.size(); i++) {
          sum += child_val.cells[i].number;
        }
        result.number = sum;
      }
      if (function == "avg") {
        int sum = 0;
        for (size_t i = 0; i < child_val.cells.size(); i++) {
          sum += child_val.cells[i].number;
        }
        result.number = sum / child_val.cells.size();
      }
      if (function == "pow") {
        if (child_val.cells.size() == 2) {
          result.number =
              std::pow(child_val.cells[0].number, child_val.cells[1].number);
        } else
          result.type = Customs::ERR;
      }
    }
    return result;
  }
};
#include "../headers/tree.h"
#include <cstddef>
#include <iostream>
#include <string>
#include <utility>

using namespace Customs;

Number::Number(int number) {
  state = DONE;
  this->number = number;
}

Value Number::get_value() {
  Value value;
  value.type = NUM;
  value.number = number;
  return value;
}

void Number::print() { std::cout << number << std::endl; }

Text::Text(std::string text) {
  state = DONE;
  this->text = text;
}

Text::Text(std::vector<Token> tokens) {
  std::string tmp;
  for (size_t i = 0; i < tokens.size(); i++) {
    if (tokens[i].type == NUMBER)
      tmp.append(std::to_string(tokens[i].number));
    else
      tmp.append(tokens[i].text);
  }
}

Value Text::get_value() {
  Value value;
  value.type = STR;
  value.text = text;
  return value;
}

void Text::print() { std::cout << text << std::endl; }

Binary::Binary(std::unique_ptr<AST> left, char oper,
               std::unique_ptr<AST> right) {
  left_child = std::move(left);
  right_child = std::move(right);
  symbol = oper;
}

Value Binary::get_value() {
  Value left_val, right_val, result;
  state = RUNNING;
  left_val = left_child->get_value();
  right_val = right_child->get_value();
  state = DONE;
  if (left_val.type != NUM || right_val.type != NUM) {
    result.type = ERR;
    return result;
  }
  result.type = NUM;
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
      result.number = std::numeric_limits<double>::infinity();
    else
      result.number = left_val.number / right_val.number;
    break;
  case '%':
    result.number = std::fmod(left_val.number, right_val.number);
    break;
  }
  return result;
}

void Binary::print() {
  left_child->print();
  std::cout << symbol << std::endl;
  right_child->print();
}

/*Value Cell::get_value() {
  Value result;
  if (indexes.empty())
    result.type = ERR;
  if (indexes.size() == 1) {
    result = table->evaluate(indexes[0]);
  } else {
    result.type = RNG;
    result.cells = std::vector<Value>();
    for (size_t i = 0; i < indexes.size(); i++) {
      result.cells.push_back(table->evaluate(indexes[i]));
    }
  }
  return result;
}

void Cell::print() {
  for (auto &index : indexes)
    std::cout << index.x << ":" << index.y << std::endl;
}*/

Value Function::get_value() {
  Value result{};
  std::vector<Value> children_vals = std::vector<Value>();
  double PI = 3.1415926535897932384626433;

  for (size_t i = 0; i < children.size(); i++)
    children_vals.push_back(children[i]->get_value());

  for (size_t i = 0; i < children_vals.size(); i++)
    std::cout << children_vals[i] << std::endl;

  if (children_vals.empty()) {
    result.type = ERR;
    return result;
  }

  if (function == "sin") {
    if (children_vals[0].type != NUM)
      result.type = ERR;
    else {
      result.type = NUM;
      result.number = std::sin((PI * children_vals[0].number) / 180);
    }
  }

  if (function == "cos") {
    if (children_vals[0].type != NUM)
      result.type = ERR;
    else {
      result.type = NUM;
      result.number = std::cos((PI * children_vals[0].number) / 180);
    }
  }

  if (function == "pow") {
    if (children_vals.size() >= 2 &&
        !(children_vals[0].type != NUM || children_vals[1].type != NUM)) {
      result.type = NUM;
      result.number =
          std::pow(children_vals[0].number, children_vals[1].number);
    } else
      result.type = ERR;
  }

  if (function == "sum") {
    double sum = 0;
    for (auto &val : children_vals) {
      if (val.type == NUM)
        sum += val.number;
    }
    result.type = NUM;
    result.number = sum;
  }

  if (function == "avg") {
    double sum = 0;
    size_t count = 0;
    for (auto &val : children_vals) {
      if (val.type == NUM) {
        sum += val.number;
        count++;
      }
    }
    result.type = NUM;
    result.number = sum / count;
  }

  return result;
}

void Function::print() {
  std::cout << function << std::endl;
  for (auto &child : children)
    child->print();
}
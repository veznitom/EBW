#ifndef __CUSTOMS__H
#define __CUSTOMS__H
#include <cstddef>
#include <memory>
#include <new>
#include <ostream>
#include <string>
#include <utility>
#include <variant>
#include <vector>

namespace Customs {

#define BIGFUCKINGLINE                                                         \
  "--------------------------------------------------------------------------" \
  "---------------------------"

enum Type : int { NUMBER, TEXT, FUNCTION, SYMBOL, LBRACK, RBRACK, UNDEF, END };
enum ValueType : int { NUM, STR, RNG, ERR };
enum EvalState : int { RUNNING, DONE, ERROR };

struct Coordinates {
  size_t x, y;
};

struct Value {
  ValueType type;
  double number{};
  std::string text{};
  std::vector<Value> cells{};

  Value() { type = ERR; }

  friend std::ostream &operator<<(std::ostream &os, const Value &value) {
    os << "\n\tType: " << value.type << "\n\tNumber: " << value.number
       << "\n\tText: " << value.text << "\n";
    return os;
  }
};

struct Token {
  Type type;
  double number{};
  size_t position{};
  std::string text{};

  Token() { type = UNDEF; }
  Token(Type tp) { type = tp; }
  Token(Type tp, double num) {
    type = tp;
    number = num;
  }
  Token(Type tp, std::string str) {
    type = tp;
    text = str;
  }

  friend std::ostream &operator<<(std::ostream &os, const Token &value) {
    os << "\n\tType: " << value.type << "\n\tNumber: " << value.number
       << "\n\tText: " << value.text << "\n";
    return os;
  }
};
}; // namespace Customs

#endif
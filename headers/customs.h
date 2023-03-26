#ifndef __CUSTOMS__H
#define __CUSTOMS__H
#include <cstddef>
#include <memory>
#include <new>
#include <string>
#include <utility>
#include <variant>
#include <vector>

namespace Customs {

#define BIGFUCKINGLINE "-----------------------------------------------------------------------------------------------------"

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
};

struct Token {
  Type type;
  double number{};
  std::string text{};

  Token() { type = UNDEF; }

  Token(Type tp) { type = tp; }
};
}; // namespace Customs

#endif
#include <memory>
#include <new>
#include <string>
#include <utility>
#include <variant>
#include <vector>

class Customs {
public:
  enum Type : int {
    NUMBER,
    TEXT,
    FUNCTION,
    SYMBOL,
    LBRACK,
    RBRACK,
    UNDEF,
    END
  };

  enum ValueType : int { NUM, STR, RNG, ERR };

  struct Value {
    ValueType type;
    union {
      int number;
      std::string text;
      std::vector<Value> cells;
    };

    Value() { type = ERR; }

    Value(const Value &other) {
      type = other.type;
      switch (type) {
      case NUM:
        number = other.number;
        break;
      case STR:
        text = other.text;
        break;
      case RNG:
        cells = other.cells;
        break;
      case ERR:
        break;
      }
    }

    Value &operator=(const Value &other) {
      type = other.type;
      switch (type) {
      case NUM:
        number = other.number;
        break;
      case STR:
        text = other.text;
        break;
      case RNG:
        cells = other.cells;
        break;
      case ERR:
        break;
      }
      return *this;
    }

    ~Value() {
      using namespace std;
      if (type == STR) {
        (&text)->~string();
      }
      if (type == RNG) {
        (&cells)->~vector<Value>();
      }
    }
  };

  enum EvalState : int { RUNNING, DONE, ERROR };
};

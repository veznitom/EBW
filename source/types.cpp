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

    Value(Value &value){
      type = value.type;
      switch (type) {
      case NUM:
        number = value.number;
      break;
      case STR:
        text = value.text;
      break;
      case RNG:
        cells = value.cells;
      break;
      case ERR:
        break;
      }
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

  enum EvalState:int{
    RUNNING,
    DONE,
    ERROR
  };
};

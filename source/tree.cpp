#include "types.cpp"
#include <string>

class AST{
  virtual Customs::Value get_value();
};

class Number:AST{
  int number;
  Customs::Value get_value(){
    Customs::Value value;
    value.type = Customs::NUM;
    value.number = number;
    return value;
  }
};

class Text:AST{
  std::string text;
  Customs::Value get_value(){
    Customs::Value value;
    value.type = Customs::NUM;
    value.text = text;
    return value;
  }
};

class Expresion:AST{
  Customs::EvalState state;
};

class Binary:Expresion{};

class Cell:Expresion{};

class Function:Expresion{};
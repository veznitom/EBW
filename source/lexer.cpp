#include "types.cpp"
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <set>
#include <string>
#include <vector>

struct Token {
  Customs::Type type;
  union {
    int number;
    std::string text;
  };

  Token() {
    type = Customs::UNDEF;
    number = 0;
  }

  Token(Customs::Type _type) { type = _type; }

  ~Token() {
    using namespace std;
    if (type != number) {
      (&text)->~string();
    }
  }
};

class Lexer {
private:
  bool debug;

  std::set<char> symbols = {'+', '-', '*', '/', '%', '=', ':'};
  std::set<std::string> functions = {"sum", "sin", "cos", "avg", "pow"};

  void match_funct(Token &token) {
    if (functions.find(token.text) != functions.end())
      token.type = Customs::FUNCTION;
  }

  void print_tokens(std::vector<Token> &tokens) {
    for (size_t i = 0; i < tokens.size(); i++) {
      switch (tokens[i].type) {
      case Customs::UNDEF:
        std::cout << "UNDEF";
        break;
      case Customs::NUMBER:
        std::cout << "NUMBER";
        break;
      case Customs::TEXT:
        std::cout << "TEXT";
        break;
      case Customs::FUNCTION:
        std::cout << "FUNCTION";
        break;
      case Customs::SYMBOL:
        std::cout << "SYMBOL";
        break;
      case Customs::LBRACK:
        std::cout << "LBRACK";
        break;
      case Customs::RBRACK:
        std::cout << "RBRACK";
        break;
      case Customs::END:
        std::cout << "END";
        break;
      }
      std::cout << ": ";
      if (tokens[i].type == Customs::NUMBER)
        std::cout << tokens[i].number << std::endl;
      else
        std::cout << tokens[i].text << std::endl;
    }
    std::cout << std::endl;
  }

public:
  Lexer() { debug = false; }

  Lexer(bool _debug) { debug = _debug; }

  std::vector<Token> analyze(std::string sequence) {
    std::vector<Token> tokens = std::vector<Token>();
    Token token;
    for (size_t i = 0; i <= sequence.size(); i++) {
      if (symbols.find(sequence[i]) != symbols.end()) {
        match_funct(token);
        tokens.push_back(token);
        token.type = Customs::SYMBOL;
        token.text = sequence[i];
      } else if (sequence[i] == '(') {
        match_funct(token);
        tokens.push_back(token);
        token.type = Customs::LBRACK;
        token.text = '(';
      } else if (sequence[i] == ')') {
        match_funct(token);
        tokens.push_back(token);
        token.type = Customs::RBRACK;
        token.text = ')';
      } else if ('0' <= sequence[i] && sequence[i] <= '9') {
        if (token.type == Customs::TEXT) {
          token.text.append(1, sequence[i]);
        } else if (token.type != Customs::NUMBER) {
          match_funct(token);
          tokens.push_back(token);
          token.type = Customs::NUMBER;
          token.number = sequence[i] - '0';
        } else
          token.number = token.number * 10 + (sequence[i] - '0');
      } else if (' ' <= sequence[i] && sequence[i] <= '~') {
        if (token.type != Customs::TEXT) {
          match_funct(token);
          tokens.push_back(token);
          token.type = Customs::TEXT;
          token.text = sequence[i];
        } else
          token.text.append(1, sequence[i]);
      } else if (sequence[i] == '\0') {
        match_funct(token);
        tokens.push_back(token);
        token.type = Customs::END;
        token.text = '\0';
      } else {
        match_funct(token);
        tokens.push_back(token);
        token.type = Customs::UNDEF;
        token.text = sequence[i];
      }
    }
    if (!tokens.empty())
      tokens.erase(tokens.begin());
    tokens.push_back(Token(Customs::END));
    if (debug)
      print_tokens(tokens);
    return tokens;
  }
};
#include "../headers/lexer.h"
#include <cstddef>

using namespace Customs;

void Lexer::match_funct(Token &token) {
  if (functions.find(token.text) != functions.end())
    token.type = FUNCTION;
}

void Lexer::print_tokens(std::vector<Token> &tokens) {
  for (size_t i = 0; i < tokens.size(); i++) {
    switch (tokens[i].type) {
    case UNDEF:
      std::cout << "UNDEF";
      break;
    case NUMBER:
      std::cout << "NUMBER";
      break;
    case TEXT:
      std::cout << "TEXT";
      break;
    case FUNCTION:
      std::cout << "FUNCTION";
      break;
    case SYMBOL:
      std::cout << "SYMBOL";
      break;
    case LBRACK:
      std::cout << "LBRACK";
      break;
    case RBRACK:
      std::cout << "RBRACK";
      break;
    case END:
      std::cout << "END";
      break;
    }
    std::cout << ": ";
    if (tokens[i].type == NUMBER)
      std::cout << tokens[i].number << std::endl;
    else
      std::cout << tokens[i].text << std::endl;
  }
  std::cout << std::endl;
}

Lexer::Lexer() { debug = false; }

Lexer::Lexer(bool dbg) { debug = dbg; }

std::vector<Token> Lexer::analyze(std::string sequence) {
  std::vector<Token> tokens = std::vector<Token>();
  Token token;
  for (size_t i = 0; i <= sequence.size(); i++) {
    if (symbols.find(sequence[i]) != symbols.end()) {
      match_funct(token);
      tokens.push_back(token);
      token.type = SYMBOL;
      token.text = sequence[i];
      token.position = i;
    } else if (sequence[i] == '(') {
      match_funct(token);
      tokens.push_back(token);
      token.type = LBRACK;
      token.text = '(';
      token.position = i;
    } else if (sequence[i] == ')') {
      match_funct(token);
      tokens.push_back(token);
      token.type = RBRACK;
      token.text = ')';
      token.position = i;
    } else if ('0' <= sequence[i] && sequence[i] <= '9') {
      if (token.type == TEXT) {
        token.text.append(1, sequence[i]);
      } else if (token.type != NUMBER) {
        match_funct(token);
        tokens.push_back(token);
        token.type = NUMBER;
        token.number = sequence[i] - '0';
        token.position = i;
      } else
        token.number = token.number * 10 + (sequence[i] - '0');
    } else if (' ' <= sequence[i] && sequence[i] <= '~') {
      if (token.type != TEXT) {
        match_funct(token);
        tokens.push_back(token);
        token.type = TEXT;
        token.text = sequence[i];
        token.position = i;
      } else
        token.text.append(1, sequence[i]);
    } else if (sequence[i] == '\0') {
      match_funct(token);
      tokens.push_back(token);
      token.type = END;
      token.text = '\0';
      token.position = i;
    } else {
      match_funct(token);
      tokens.push_back(token);
      token.type = UNDEF;
      token.text = sequence[i];
      token.position = i;
    }
  }
  if (!tokens.empty())
    tokens.erase(tokens.begin());
  tokens.push_back(Token(END));
  if (debug)
    print_tokens(tokens);
  return tokens;
}
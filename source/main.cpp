#include "lexer.cpp"

int main(int argc, char** argv){
  Lexer lex{true};
  lex.analyze("Hello+world!");
  lex.analyze("123");
  lex.analyze("=B5+C6");
  lex.analyze("=sum(B5:C6)");
  lex.analyze("1234Greetings!");
  lex.analyze("-5127");
  return 0;
}
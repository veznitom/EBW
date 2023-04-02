#include "../headers/parser.h"
#include "../headers/tree.h"
#include <memory>
#include <string>

int main(int argc, char **argv) {
  Parser parser = Parser();
  std::string test = "=sin(1,2,3,4,5)";
  auto tmp= parser.Parse(test);
  tmp->print();
  std::cout << std::endl;
  std::cout << tmp->get_value().number << std::endl;
  return 0;
}
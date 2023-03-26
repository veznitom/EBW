#ifndef __TABLE__H
#define __TABLE__H
#include <memory>
#include <vector>
#include "customs.h"

using namespace Customs;

class AST;

class Table {
  std::vector<std::vector<std::unique_ptr<AST>>> cells;
public:
  Value evaluate(Coordinates coords);
};

#endif
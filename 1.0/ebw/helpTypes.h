#ifndef EBW_HELPTYPES_H
#define EBW_HELPTYPES_H
#include <vector>
#include <string>
using std::string;
using std::vector;

//!Simple structure to pair two int values
/**There is no difference between Size and Location exept in naming. This is purely for prettier code.*/
struct Size {
    int width = 0, height = 0;
};

//!Simple structure to pair two int values
/**There is no difference between Size and Location exept in naming. This is purely for prettier code.*/
struct Location {
    int x = 0, y = 0;
};

//!List of all cell types in numerical form
/**Main purpose is clarity of code.*/
enum Type {
    empty,
    text,
    number,
    boolean,
    date,
    expresion,
    function,
    aggregation
};

//!List of all implemented functions
/**Main purpose is clarity of code.*/
enum Function {
    SIN, COS, ABS, STRLEN, CONCAT, TOINT, TOSTR, none
};

//!List of all implemented aggregation functions
/**Main purpose is clarity of code.*/
enum Aggregation {
    MAX, MIN, AVG, SUM, COUNT, NONE
};

//!Special struct that contains necessary information which is used in calculations
struct Carier {
    string text;
    double number;
    Type type, subt;
};

//!Building structure of binary tree
struct Node {
    float number;
    char sign;
    Node *left;
    Node *right;
};

//!List of displaying states of UI
/**Main purpose is clarity of code.*/
enum State{
    MENU,INSERT,DELETE
};
//!Simple check if number is number
//!@param[in] input Text to be checked and parsed
//!@return 0 if not convertible > 0 if ok
int GetNumber(const string &input);
#endif //EBW_HELPTYPES_H

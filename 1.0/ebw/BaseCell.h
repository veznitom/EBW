#ifndef EBW_BASECELL_H
#define EBW_BASECELL_H

#include <string>
#include <vector>
#include <map>
#include <ncurses.h>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <limits>
#include "helpTypes.h"

using std::string;
using std::vector;
using std::map;

//!BaseCell is pure abstract class from which all other cells inherit
class BaseCell {
public:
    //!Destructor is default because there is no manual allocation
    virtual ~BaseCell() = default;

    //!@return Size of cell
    virtual Size GetSize() = 0;

    //!@return Type of cell
    virtual Type GetType() = 0;

    //!@return display
    virtual string Display() const = 0;

    //!@return number
    virtual double GetNumber() const = 0;

    //!Prints it's content to window
    ///@param[in] window Pointer to a window that is currently displayed in terminal
    virtual void Draw(WINDOW *window) = 0;

    //!Takes raw input and parses it to needed information
    //!@return True if parsing went well otherwise false
    virtual bool Update() = 0;

    //!@return Cell's input (original)
    virtual string Input() = 0;

    //!Disables cells ability to show value
    virtual void Kill() = 0;

    //!@return Vector with addresses or range from which it needs values
    virtual vector<string> RequestInfo() = 0;

protected:
    //!Current size of cell
    Size size{},
    //!Max size that cell can have
    /**It's little bit smaler than display window so there is always at least one cell displayed*/
    maxSize{};
    //!String addresses that cell wants values from
    vector<string> linked;
    //!String in which is the text that will be displayed
    string display,
    //!Unparsed text input
    original;
    //!Cells numerical value
    double number;
    //!Killswitch decides if cell will calculate and display it's value or error message
    bool killSwitch;

    //!Calculating length and height of text
    //!@param[in] input
    //!@param[out] Size width = input length
    //!@param[out] Size height = input's number of lines
    /**Going line by line and counting it's length. Number of lines determines height of cell and longest line width of cell.*/
    static Size ParseTextToSize(const string &input);
};

//!Empty cell is placeholder for oder cells
/**Empty cell fills empty space of Table*/
class EmptyCell : public BaseCell {
public:
    EmptyCell();

    Size GetSize() override;

    Type GetType() override;

    string Display() const override;

    double GetNumber() const override;

    void Draw(WINDOW *window) override;

    bool Update() override;

    string Input() override;

    void Kill() override;

    vector<string> RequestInfo() override;
};

//!Cell specialise in linking with other cells and calculating simple equation
class ExpresionCell : public BaseCell {
public:
    //!@param[in] input Unparsed text
    //!@param[in] maxSize Inherited max size of display
    explicit ExpresionCell(const string &input, Size maxSize);

    Size GetSize() override;

    Type GetType() override;

    string Display() const override;

    double GetNumber() const override;

    void Draw(WINDOW *window) override;

    vector<string> RequestInfo() override;

    //!Solves binary equating that was build in GetValues()
    //!@param[in] node Binary tree root
    //!@return Number calculated by binary tree
    double RecursiveSolve(Node *node);

    //!Freeing all allocated memory used in binary tree
    //!@param[in] node Binary tree root
    void RecursiveDelete(Node *node);

    //!Receive values send by Table and creates binary tree with signs and values
    //!\param[in] input All valid values that cells points to
    void GetValues(const vector<double> &input);

    bool Update() override;

    string Input() override;

    void Kill() override;

private:
    vector<char> signs;
};

//!Cell that handles functions
/**For example: sin, cos, abs, strlen, concat, ...*/
class FunctionCell : public BaseCell {
public:
    Type subType;

    //!@param[in] input Unparsed text
    //!@param[in] maxSize Inherited max size of display
    explicit FunctionCell(const string &input, Size maxSize);

    Size GetSize() override;

    Type GetType() override;

    string Display() const override;

    double GetNumber() const override;

    void Draw(WINDOW *window) override;

    vector<string> RequestInfo() override;

    //!Takes information stored in input and depending on used function sets display text
    //!@param[in] input
    void GetValues(const vector<Carier> &input);

    bool Update() override;

    string Input() override;

    void Kill() override;

private:
    Function function;
};

//!Cell that handles aggregation function
/**For example: min, max, sum, avg, count*/
class AggregationCell : public BaseCell {
public:
    //!@param[in] input Unparsed text
    //!@param[in] maxSize Inherited max size of display
    explicit AggregationCell(const string &input, Size maxSize);

    Size GetSize() override;

    Type GetType() override;

    string Display() const override;

    double GetNumber() const override;

    void Draw(WINDOW *window) override;

    vector<string> RequestInfo() override;

    void GetValues(const vector<Carier> &input);

    bool Update() override;

    string Input() override;

    void Kill() override;

private:
    Aggregation function;
};

//!Cell which primary functions is to display plain tex
/**Cell does not do any parsing or calculations it just takes input and makes it output as it is.*/
class TextCell : public BaseCell {
public:
    //!@param[in] input
    //!@param[in] maxSize Inherited max size of display
    explicit TextCell(const string &input, Size maxSize);

    Size GetSize() override;

    Type GetType() override;

    string Display() const override;

    double GetNumber() const override;

    void Draw(WINDOW *window) override;

    bool Update() override;

    string Input() override;

    vector<string> RequestInfo() override;

    void Kill() override;
};

//!Cell that function as a number
class NumberCell : public BaseCell {
public:
    //!@param[in] input Unparsed text
    //!@param[in] maxSize Inherited max size of display
    explicit NumberCell(const string &input, Size maxSize);

    Size GetSize() override;

    Type GetType() override;

    string Display() const override;

    double GetNumber() const override;

    void Draw(WINDOW *window) override;

    //!Parsing test to double
    bool Update() override;

    string Input() override;

    vector<string> RequestInfo() override;

    void Kill() override;
};

//!Cell that handles TRUE and FALSE statements
/**\todo Solving TRUE/FALSE statements*/
class BoolCell : public BaseCell {
public:
    //!@param[in] input Unparsed text
    //!@param[in] maxSize Inherited max size of display
    explicit BoolCell(const string &input, Size maxSize);

    Size GetSize() override;

    Type GetType() override;

    string Display() const override;

    double GetNumber() const override;

    void Draw(WINDOW *window) override;

    bool Update() override;

    string Input() override;

    vector<string> RequestInfo() override;

    void Kill() override;

private:
    //!Contains input values as bool
    bool state;
};

//!Cell that takes form of date
/**\todo Add operations with date*/
class DateCell : public BaseCell {
public:
    //!@param[in] input Unparsed text
    //!@param[in] maxSize Inherited max size of display
    explicit DateCell(const string &input, Size maxSize);

    Size GetSize() override;

    Type GetType() override;

    string Display() const override;

    double GetNumber() const override;

    void Draw(WINDOW *window) override;

    bool Update() override;

    string Input() override;

    vector<string> RequestInfo() override;

    void Kill() override;

private:
    int day, month, year;
};

#endif //EBW_BASECELL_H

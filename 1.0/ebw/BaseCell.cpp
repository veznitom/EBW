//
// Created by tom on 5/10/20.
//
#include <stab.h>
#include "BaseCell.h"

EmptyCell::EmptyCell() {
    display = original = " ";
    number = 0;
    size.width = 1;
    size.height = 1;
}

string EmptyCell::Display() const {
    return display;
}

Size EmptyCell::GetSize() {
    return size;
}

Type EmptyCell::GetType() {
    return Type::empty;
}

double EmptyCell::GetNumber() const {
    return 0;
}

void EmptyCell::Draw(WINDOW *window) {
    wmove(window, 1, 1);
    wprintw(window, display.c_str());
    wborder(window, '|', '|', '-', '-', '+', '+', '+', '+');
    wnoutrefresh(window);
}

bool EmptyCell::Update() {
    return true;
}

string EmptyCell::Input() {
    return original;
}

vector<string> EmptyCell::RequestInfo() { return vector<string>(); }

void EmptyCell::Kill() {

}

ExpresionCell::ExpresionCell(const string &input, Size maxsize) {
    maxSize = maxsize;
    original = input;
    number = 0;
    killSwitch = false;
    if (!ExpresionCell::Update())
        display = "ERROR";
}

Size ExpresionCell::GetSize() {
    size.width = display.length();
    size.height = 1;
    return size;
}

Type ExpresionCell::GetType() {
    return Type::expresion;
}

string ExpresionCell::Display() const {
    return display;
}

double ExpresionCell::GetNumber() const {
    return number;
}

void ExpresionCell::Draw(WINDOW *window) {
    wmove(window, 1, 1);
    wprintw(window, display.c_str());
    wborder(window, '|', '|', '-', '-', '+', '+', '+', '+');
    wnoutrefresh(window);
}

vector<string> ExpresionCell::RequestInfo() {
    return linked;
}


double ExpresionCell::RecursiveSolve(Node *node) {
    //If node has sing its not value node
    if (node->sign == 0)
        return node->number;
    else
        //If it does it calls RecursiveSolve againg to get values from nodes its connected to
        switch (node->sign) {
            case '+':
                return RecursiveSolve(node->left) + RecursiveSolve(node->right);
            case '-':
                return RecursiveSolve(node->left) - RecursiveSolve(node->right);
            case '*':
                return RecursiveSolve(node->left) * RecursiveSolve(node->right);
            case '/':
                return RecursiveSolve(node->left) / RecursiveSolve(node->right);
            default:
                return 0;
        }
}


void ExpresionCell::RecursiveDelete(Node *node) {
    if (node->left != nullptr)
        RecursiveDelete(node->left);
    if (node->right != nullptr)
        RecursiveDelete(node->right);
    delete node;
}

void ExpresionCell::GetValues(const vector<double> &input) {
    std::ostringstream os;
    //If the are no values skip calculation
    if (input.empty()) {
        number = 0;
        display = "EMPTY";
        return;
    }
    //If cycle was detected by Table cell is disabled
    if (killSwitch) {
        display = "CYCLE";
        return;
    }
    vector<char> backup;
    vector<Node *> nodes;
    //For each values in input create value Node
    for (auto &a:input) {
        Node *node = new Node;
        node->sign = 0;
        node->number = (float) a;
        node->left = nullptr;
        node->right = nullptr;
        nodes.push_back(node);
    }
    //Backing up signs so it can be used later again
    backup = signs;
    //Creating tree by taking * and / and adjacent value Nodes
    for (int i = 0; i < (int) signs.size(); ++i) {
        if (signs[i] == '*' || signs[i] == '/') {
            Node *connect = new Node;
            connect->sign = signs[i];
            connect->left = nodes.at(i);
            connect->right = nodes.at(i + 1);
            nodes.erase(nodes.begin() + i);
            nodes.erase(nodes.begin() + i);
            nodes.insert(nodes.begin() + i, connect);
            signs.erase(signs.begin() + i);
        }
    }
    //Creating rest of the nodes from + and -
    while (nodes.size() > 1) {
        Node *connect = new Node;
        connect->sign = signs[0];
        connect->left = nodes.at(0);
        connect->right = nodes.at(1);
        nodes.erase(nodes.begin());
        nodes.erase(nodes.begin());
        nodes.insert(nodes.begin(), connect);
        signs.erase(signs.begin());
    }
    //Calling solve method
    number = RecursiveSolve(nodes.begin().operator*());
    //Freeing memory
    RecursiveDelete(nodes.begin().operator*());
    signs = backup;
    os << std::setprecision(10) << number;
    display = os.str();
}

bool ExpresionCell::Update() {
    //Removing spaces from text
    original.erase(remove(original.begin(), original.end(), ' '), original.end());
    string addr;
    for (size_t i = 0; i <= original.length(); i++) {
        //If char in input is +,-,*,/ it's added to sign vector
        if (original[i] == '+' || original[i] == '-' || original[i] == '*' || original[i] == '/' || original[i] == '\0') {
            if (original[i] != '\0')
                signs.push_back(original[i]);
            //If in the end of text push last address
            if (original[i] == '\0') {
                linked.push_back(addr);
                return true;
            }
            //If sign was detected then i means end of last address so it's inserted
            linked.push_back(addr);
            addr = "";
            continue;
        }
        //Controlling that there is no invalid input in address
        if ((original[i] >= 'A' && original[i] <= 'Z') || (original[i] >= '0' && original[i] <= '9')) {
            addr += original[i];
        } else {
            linked.clear();
            signs.clear();
            return false;
        }
    }
    linked.clear();
    signs.clear();
    return false;
}

string ExpresionCell::Input() {
    return original;
}

void ExpresionCell::Kill() {
    killSwitch = true;
}

FunctionCell::FunctionCell(const string &input, Size maxsize) {
    maxSize = maxsize;
    original = input;
    number = 0;
    killSwitch = false;
    subType = Type::empty;
    function = Function::none;
    if (!FunctionCell::Update()) {
        display = "ERROR";
    }
}

Size FunctionCell::GetSize() {
    if (subType == Type::text)
        return ParseTextToSize(display);
    size.width = display.length();
    size.height = 1;
    return size;
}

Type FunctionCell::GetType() {
    return Type::function;
}

string FunctionCell::Display() const {
    return display;
}

double FunctionCell::GetNumber() const {
    return number;
}

void FunctionCell::Draw(WINDOW *window) {
    wmove(window, 1, 1);
    wprintw(window, display.substr(0, maxSize.width).c_str());
    wborder(window, '|', '|', '-', '-', '+', '+', '+', '+');
    wnoutrefresh(window);
}

vector<string> FunctionCell::RequestInfo() {
    return linked;
}

void FunctionCell::GetValues(const vector<Carier> &input) {
    //If the are no values skip calculation
    if (input.empty()) {
        number = 0;
        display = "EMPTY";
        return;
    }
    //If cycle was detected by Table cell is disabled
    if (killSwitch) {
        display = "CYCLE";
        return;
    }
    std::ostringstream os;
    //If there are more inputs than allowed return
    if ((function != 4 && input.size() > 1) || input.empty()) {
        display = "ERROR";
        return;
    }
    //Choosing which function to use
    switch (function) {
        case 0:/* SIN */
            if (input.at(0).type == Type::number) {
                number = sin(input.at(0).number);
                os << std::setprecision(10) << number;
                display = os.str();
                subType = Type::number;
            } else
                display = "ERROR";
            break;
        case 1:/* COS */
            if (input.at(0).type == Type::number) {
                number = cos(input.at(0).number);
                os << std::setprecision(10) << number;
                display = os.str();
                subType = Type::number;
            } else
                display = "ERROR";
            break;
        case 2:/* ABS */
            if (input.at(0).type == Type::number) {
                number = fabs(input.at(0).number);
                os << std::setprecision(10) << number;
                display = os.str();
                subType = Type::number;
            } else
                display = "ERROR";
            break;
        case 3:/* STRLEN */
            if (input.at(0).type == Type::text) {
                number = input.at(0).text.length();
                os << std::setprecision(10) << number;
                display = os.str();
                subType = Type::number;
            } else
                display = "ERROR";
            break;
        case 4:/* CONCAT */
            if (input.size() != 2) {
                display = "ERROR";
                return;
            }
            display = input.at(0).text + input.at(1).text;
            subType = Type::text;
            return;
        case 5:/* TOINT */
            if (input.at(0).type == Type::text) {
                string text = input.at(0).text;
                bool decFlag, sing;
                sing = false;
                decFlag = false;
                if (text.length() == 1 && (text[0] == '.' || text[0] == '-'))
                    return;
                if (text.length() > 1 && text[0] == '.')
                    decFlag = true;
                if (text[0] == '-') {
                    sing = true;
                }
                for (int j = sing ? 1 : 0; j < (int) text.length() - 1; ++j) {
                    if (text[j] == '.' && !decFlag) {
                        decFlag = true;
                        continue;
                    }
                    if (text[j] < '0' || text[j] > '9')
                        return;
                }
                number = std::stod(text);
                os << std::setprecision(10) << number;
                display = os.str();
                subType = Type::number;
            } else
                display = "ERROR";
            break;
        case 6:/* TOSTR */
            if (input.at(0).type == Type::number) {
                os << std::setprecision(10) << input.at(0).number;
                display = os.str();
                subType = Type::text;
                return;
            }
            display = "ERROR";
            break;
        default:
            display = "ERROR";
            return;
    }
}

bool FunctionCell::Update() {
    string command, address;
    bool flag = false;
    //Removing spaces from text
    original.erase(remove(original.begin(), original.end(), ' '), original.end());
    //Splitting text on command part and address
    for (char &i:original) {
        if (i == '(') {
            flag = true;
            continue;
        }
        if (i == ')')
            break;
        if (!flag)
            command += (char) std::toupper(i);
        else
            address += i;
    }
    //Comparing command to predefined values
    if (command == "SIN") {
        function = Function::SIN;
        linked.push_back(address);
        return true;
    } else if (command == "COS") {
        function = Function::COS;
        linked.push_back(address);
        return true;
    } else if (command == "ABS") {
        function = Function::ABS;
        linked.push_back(address);
        return true;
    } else if (command == "STRLEN") {
        function = Function::STRLEN;
        linked.push_back(address);
        return true;
    } else if (command == "CONCAT") {
        function = Function::CONCAT;
        string first, second;
        flag = false;
        //If function is CONCAT it needs to control if there are two addresses
        for (char &c:address) {
            if (c == ')' || c == '\0')
                break;
            if (c == ',') {
                flag = true;
                continue;
            }
            if (!flag)
                first += c;
            else
                second += c;
        }
        if (second.empty())
            return false;
        linked.push_back(first);
        linked.push_back(second);
        return true;
    } else if (command == "TOSTR") {
        function = Function::TOSTR;
        linked.push_back(address);
        return true;
    } else if (command == "TOINT") {
        function = Function::TOINT;
        linked.push_back(address);
        return true;
    } else
        return false;
}

string FunctionCell::Input() {
    return original;
}

void FunctionCell::Kill() {
    killSwitch = true;
}


AggregationCell::AggregationCell(const string &input, Size maxsize) {
    maxSize = maxsize;
    original = input;
    number = 0;
    killSwitch = false;
    function = Aggregation::NONE;
    if (!AggregationCell::Update())
        display = "ERROR";
}

Size AggregationCell::GetSize() {
    return ParseTextToSize(display);
}

Type AggregationCell::GetType() {
    return Type::aggregation;
}

string AggregationCell::Display() const {
    return display;
}

double AggregationCell::GetNumber() const { return number; }

void AggregationCell::Draw(WINDOW *window) {
    wmove(window, 1, 1);
    wprintw(window, display.c_str());
    wborder(window, '|', '|', '-', '-', '+', '+', '+', '+');
    wnoutrefresh(window);
}

vector<string> AggregationCell::RequestInfo() {
    return linked;
}

void AggregationCell::GetValues(const vector<Carier> &input) {
    //If the are no values skip calculation
    if (input.empty()) {
        number = 0;
        display = "EMPTY";
        return;
    }
    //If cycle was detected by Table cell is disabled

    if (killSwitch) {
        display = "CYCLE";
        return;
    }
    int counter = 0;
    std::ostringstream os;
    switch (function) {
        case 0:/* MAX */
            //Getting lowest value possible
            number = std::numeric_limits<double>::min();
            for (auto &i:input) {
                //If higher value replace current highest
                if (i.type == Type::number || i.type == aggregation || (i.type == Type::function && i.subt == number) || i.type == Type::expresion) {
                    if (number < i.number)
                        number = i.number;
                }
            }
            //If still lowest possible set special case output
            if (number == std::numeric_limits<double>::min()) {
                display = "-inf";
                return;
            }
            break;
        case 1:/* MIN */
            //Getting highest value possible
            number = std::numeric_limits<double>::max();
            for (auto &i:input) {
                //If lower value replace current lowest
                if (i.type == Type::number || i.type == aggregation || (i.type == Type::function && i.subt == number) || i.type == Type::expresion) {
                    if (number > i.number)
                        number = i.number;
                }
            }
            //If still highest possible set special case output
            if (number == std::numeric_limits<double>::min()) {
                display = "+inf";
                return;
            }
            break;
        case 2:/* AVG */
            number = 0;
            for (auto &i:input) {
                //Adds all values and counts them
                if (i.type == Type::number || i.type == aggregation || (i.type == Type::function && i.subt == number) || i.type == Type::expresion) {
                    number += i.number;
                    counter++;
                }
            }
            //Then divides if not 0
            if (counter != 0)
                number /= counter;
            if (input.empty())
                number = 0;
            break;
        case 3:/* SUM */
            number = 0;
            for (auto &i:input) {
                //Adds all values
                if (i.type == Type::number || i.type == aggregation || (i.type == Type::function && i.subt == number) || i.type == Type::expresion) {
                    number += i.number;
                }
            }
            break;
        case 4:/* COUNT */
            number = 0;
            for (auto &i:input) {
                //Counts all valid inputs
                if (i.type != Type::empty) {
                    number++;
                }
            }
            break;
        default:
            display = "ERROR";
            return;
    }
    os << std::setprecision(10) << number;
    display = os.str();
}

bool AggregationCell::Update() {
    string command, address;
    bool flag = false;
    //Removing spaces from text
    original.erase(remove(original.begin(), original.end(), ' '), original.end());
    //Splitting text on command part and address
    for (char &i:original) {
        if (i == '(') {
            flag = true;
            continue;
        }
        if (i == ')')
            break;
        if (!flag)
            command += (char) std::toupper(i);
        else
            address += i;
    }
    //Comparing command to predefined values
    if (command == "MAX") {
        function = Aggregation::MAX;
        linked.push_back(address);
        return true;
    } else if (command == "MIN") {
        function = Aggregation::MIN;
        linked.push_back(address);
        return true;
    } else if (command == "AVG") {
        function = Aggregation::AVG;
        linked.push_back(address);
        return true;
    } else if (command == "SUM") {
        function = Aggregation::SUM;
        linked.push_back(address);
        return true;
    } else if (command == "COUNT") {
        function = Aggregation::COUNT;
        linked.push_back(address);
        return true;
    } else
        return false;
}

string AggregationCell::Input() {
    return original;
}

void AggregationCell::Kill() {
    killSwitch = true;
}

TextCell::TextCell(const string &input, Size maxsize) {
    maxSize = maxsize;
    original = input;
    TextCell::Update();
}

Size TextCell::GetSize() {
    //Making sure text can fit to cell
    if ((int) display.length() >= maxSize.width - 6) {
        return Size{maxSize.width - 6, 1};
    }
    return ParseTextToSize(display);
}

Type TextCell::GetType() {
    return Type::text;
}

string TextCell::Display() const {
    return display;
}

double TextCell::GetNumber() const { return 0; }

void TextCell::Draw(WINDOW *window) {
    wmove(window, 1, 1);
    wprintw(window, display.substr(0, maxSize.width - 6).c_str());
    wborder(window, '|', '|', '-', '-', '+', '+', '+', '+');
    wnoutrefresh(window);
}

bool TextCell::Update() {
    display = original;
    return true;
}

string TextCell::Input() {
    return original;
}

vector<string> TextCell::RequestInfo() {
    return vector<string>();
}

void TextCell::Kill() {

}

NumberCell::NumberCell(const string &input, Size maxsize) {
    maxSize = maxsize;
    original = input;
    number = 0;
    if (!NumberCell::Update())
        display = "ERROR";
}

Size NumberCell::GetSize() {
    return ParseTextToSize(display);
}

Type NumberCell::GetType() {
    return Type::number;
}

string NumberCell::Display() const {
    return display;
}

double NumberCell::GetNumber() const {
    return number;
}

void NumberCell::Draw(WINDOW *window) {
    wmove(window, 1, 1);
    wprintw(window, display.c_str());
    wborder(window, '|', '|', '-', '-', '+', '+', '+', '+');
    wnoutrefresh(window);
}

bool NumberCell::Update() {
    int m = 0, d = 0;
    number = std::strtod(original.c_str(), nullptr);
    std::ostringstream os;
    os << std::setprecision(10) << number;
    display = os.str();
    //If 0 check if it was error or really 0
    if (number == 0) {
        for (auto &i:original) {
            if (i >= '0' && i <= '9')
                continue;
            else if (i == '-')
                m++;
            else if (i == '.')
                d++;
            else
                return false;
        }
        //If there was more than one - and . then input is invalid
        if (m > 1 || d > 1)
            return false;
    }
    return true;
}

string NumberCell::Input() {
    return original;
}

vector<string> NumberCell::RequestInfo() {
    return vector<string>();
}

void NumberCell::Kill() {

}

BoolCell::BoolCell(const string &input, Size maxsize) {
    maxSize = maxsize;
    original = input;
    state = false;
    if (!BoolCell::Update()) {
        display = "ERROR";
    }
}

Size BoolCell::GetSize() {
    size.width = state ? 5 : 6;
    size.height = 1;
    return size;
}

Type BoolCell::GetType() {
    return Type::boolean;
}

string BoolCell::Display() const {
    return display;
}

double BoolCell::GetNumber() const { return state; }

void BoolCell::Draw(WINDOW *window) {
    wmove(window, 1, 1);
    wprintw(window, display.c_str());
    wborder(window, '|', '|', '-', '-', '+', '+', '+', '+');
    wnoutrefresh(window);
}

bool BoolCell::Update() {
    for (auto &i:original) {
        i = std::tolower(i);
    }
    if (original == "0" || original == "false") {
        state = false;
        display = state ? "TRUE" : "FALSE";
        return true;
    } else if (original == "1" || original == "true") {
        state = true;
        display = state ? "TRUE" : "FALSE";
        return true;
    }
    return false;
}

string BoolCell::Input() {
    return original;
}

vector<string> BoolCell::RequestInfo() {
    return vector<string>();
}

void BoolCell::Kill() {

}

DateCell::DateCell(const string &input, Size maxsize) {
    maxSize = maxsize;
    original = input;
    day = month = year = 0;
    if (!DateCell::Update()) {
        size.width = 6;
        size.height = 1;
        display = "ERROR";
    }
}

Size DateCell::GetSize() {
    return size;
}

Type DateCell::GetType() {
    return Type::date;
}

string DateCell::Display() const {
    return display;
}

double DateCell::GetNumber() const { return 0; }

void DateCell::Draw(WINDOW *window) {
    wmove(window, 1, 1);
    wprintw(window, display.c_str());
    wborder(window, '|', '|', '-', '-', '+', '+', '+', '+');
    wnoutrefresh(window);
}

bool DateCell::Update() {
    if (sscanf(original.c_str(), "%d:%d:%d", &day, &month, &year) != 3)
        return false;
    //Making sure date is somewhat valid
    // {
    if (day > 31 || month > 12)
        return false;
    if (day == 31 && (month == 4 || month == 6 || month == 9 || month == 11))
        return false;
    if (day == 30 && (month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12))
        return false;
    if (!((day == 31 || day == 30 || day == 29) && month == 2 && !((year % 4 == 0 && year % 100 != 0) || year % 400 == 0))) {
        std::ostringstream os;
        os << std::setw(2) << std::setfill('0') << std::to_string(day) << ":" << std::setw(2) << std::setfill('0') << std::to_string(month) << ":" << std::setw(4) << std::setfill('0') << std::to_string(year);
        display = os.str();
        size.width = display.length();
        size.height = 1;
        return true;
    }
    // }
    return false;
}

string DateCell::Input() {
    return original;
}

vector<string> DateCell::RequestInfo() {
    return vector<string>();
}

void DateCell::Kill() {

}

Size BaseCell::ParseTextToSize(const string &input) {
    int index = 0;
    Size size;
    size.width = size.height = 1;
    vector<string> Text;
    Text.emplace_back("");
    for (char i : input) {
        if (i == '\n') {
            Text.emplace_back("");
            index++;
            continue;
        }
        Text[index] += i;
    }
    size.height = Text.size();
    for (string &i: Text) {
        if (size.width < (int) i.length())
            size.width = (int) i.length();
    }
    return size;
}
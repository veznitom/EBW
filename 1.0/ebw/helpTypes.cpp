#include "helpTypes.h"

int GetNumber(const string &input) {
    int m = 0, d = 0;
    int number = (int) std::strtol(input.c_str(), nullptr, 10);
    string check = input;
    if (number == 0) {
        for (auto &i:check) {
            if (i >= '0' && i <= '9')
                continue;
            else if (i == '-')
                m++;
            else if (i == '.')
                d++;
            else
                return 0;
        }
        if (m > 1 || d > 1)
            return 0;
    }
    return number;
}

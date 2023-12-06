/*!\author Tomáš Věžník
 * \version 0.33
 * \mainpage Simple Table Editor
 * \section intro_section Introduction
 *
 * */

#include "Interface.h"

int main(int argc, char **argv) {
    int tmp = 0;
    if (argc == 1)
        Interface interface(100);
    if (argc == 2 && (tmp = GetNumber(argv[0])) > 0) {
        Interface interface(tmp);
    }
    return 0;
}

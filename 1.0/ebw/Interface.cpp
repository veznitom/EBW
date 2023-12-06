#include "Interface.h"

Interface::Interface(int rows) {
    //Initialization of new screen in which all windows are displayed
    initscr();
    //Disabling buffering of typed characters
    cbreak();
    //Making input from keyboard reflect what was really pressed with
    raw();
    //Maximum characters in line in terminal and maximum lines in terminal in order
    int xMax, yMax;
    //Getting maximums from terminal
    getmaxyx(stdscr, yMax, xMax);
    //Defining sizes of UI windows based on maximum sizes
    input = newwin(3, xMax - 18, yMax - 4, 9);
    display = newwin(yMax - 9, xMax - 20, 3, 13);
    legend = newwin(yMax - 5, xMax - 16, 1, 8);
    stateBar = newwin(1, xMax - 18, yMax - 1, 10);
    address = newwin(1, xMax - 20, yMax - 5, 10);
    InputReset();
    //Enabling input of special keys
    keypad(input, true);
    //Geting size of display
    getmaxyx(display, displaySize.height, displaySize.width);
    //Initializing table with height of rows
    table = Table(rows, displaySize);
    state = State::MENU;
    Run();
}

void Interface::Run() {
    int c;
    Draw(true);
    CellPrint();
    //Reading input and deciding what will happen next base on key pressed
    while (true) {
        noecho();
        state = State::MENU;
        CellPrint();
        InputReset();
        //Reading key
        c = wgetch(input);
        switch (c) {
            case KEY_F(1):
                Insert();
                break;
            case KEY_F(2):
                Delete();
                break;
            case KEY_F(3):
                Save();
                break;
            case KEY_F(4):
                Load();
                break;
            case KEY_F(10):
                //Exit key was pressed and confirmation from user to leave is needed
                while (true) {
                    wclear(address);
                    wprintw(address, "Do you want to leave? (y/n)");
                    wrefresh(address);
                    InputReset();
                    c = wgetch(input);
                    if (c == 'y' || c == 'Y') {
                        //Exiting program
                        endwin();
                        return;
                    } else if (c == 'n' || c == 'N') {
                        //Returning to menu
                        break;
                    }
                }
            case KEY_UP:
                Move(KEY_UP);
                break;
            case KEY_DOWN:
                Move(KEY_DOWN);
                break;
            case KEY_LEFT:
                Move(KEY_LEFT);
                break;
            case KEY_RIGHT:
                Move(KEY_RIGHT);
                break;
            default:
                break;
        }
    }
}

void Interface::Draw(bool resize) {
    //Remembering last position of cursor
    int inx, iny;
    getyx(input, iny, inx);
    ClearWindows();
    wclear(legend);
    //Getting coordinates from where will render begin
    int x, y, xStart;
    getbegyx(display, y, x);
    xStart = x;
    if (resize)
        table.GetSizes();
    //Getting pointers for all cells that can fit to display
    vector<vector<BaseCell *>> request = table.RequestCells(display, index);
    requestSize.width = request.back().size();
    requestSize.height = request.size();
    //Controlling if currently selected cell is being able to render or not
    // {
    if (cell.x > requestSize.width - 1) {
        cell.x = requestSize.width - 1;
        last = Location{0, 0};
    }
    if (cell.y > requestSize.height - 1) {
        cell.y = requestSize.height - 1;
        last = Location{0, 0};
    }
    //}
    if (request.empty()) {
        wmove(input, iny, inx);
        return;
    }
    //Creating and printing out for every cell in request
    for (int i = 0; i < (int) request.size(); ++i) {
        windows.emplace_back();
        for (int j = 0; j < (int) request.at(i).size(); ++j) {
            //Generating legend
            // {
            if (i == 0) {
                wmove(legend, 0, x - xStart + xStart / 2);
                wprintw(legend, "%c", 'A' + j + index.x);
            }
            if (j == 0) {
                wmove(legend, y, 0);
                wprintw(legend, "%d", i + index.y);
            }
            wrefresh(legend);
            // }
            //Creates window and addes it to windows
            WINDOW *win = newwin(table.RowMax(index.y + i) + 2, table.ColMax(index.x + j) + 2, y, x);
            windows.back().push_back(win);
            //Makes cell print it's content to window
            request.at(i).at(j)->Draw(win);
            //Moving coordinates depending on max size of cell in current column
            x += table.ColMax(index.x + j) + 2;
        }
        //Resetting x axis
        x = xStart;
        //Moving coordinates depending on max size of cell in current row
        y += table.RowMax(index.y + i) + 2;
    }
    //Making all changes in windows show int the memory
    doupdate();
    //Moving cursor from where it was taken
    wmove(input, iny, inx);
}

void Interface::ClearWindows() {
    //Deleting and freeing all unused windows
    for (auto &window : windows) {
        for (auto &j : window) {
            delwin(j);
        }
    }
    windows.clear();
    clear();
    refresh();
}

void Interface::CellPrint() {
    int inx, iny, maxx, maxy, x, y;
    //Remembering cursor position
    getyx(input, iny, inx);
    getmaxyx(address, maxy, maxx);
    wclear(address);
    wclear(stateBar);
    //Depending on state print information
    switch (state) {
        case State::MENU:
            wprintw(address, "MENU: ");
            wprintw(stateBar, "F1 = INSERT | F2 = DELETE | F3 = SAVE | F4 = LOAD | F10 = EXIT");
            break;
        case State::INSERT:
            wprintw(address, "INSERT: ");
            wprintw(stateBar, "ENTER to confirm | F10 = EXIT | (syntax := type=value)");
            break;
        case State::DELETE:
            wprintw(address, "DELETE: ");
            wprintw(stateBar, "F10 = EXIT");
            break;
        default:
            break;
    }
    getyx(address, y, x);
    //Dummy if
    if (y == maxy)
        y++;
    //Printing address of currently selected cell and it's content (only what can fit)
    Type temp = table.GetCell(Location{index.x + cell.x, index.y + cell.y})->GetType();
    wprintw(address, "-%c%d %c=%s", (char) ('A' + index.x + cell.x), index.y + cell.y, Table::GiveTypeFromString(std::to_string(temp)), table.GetCell(Location{index.x + cell.x, index.y + cell.y})->Input().substr(0, maxx - x - 1).c_str());
    wrefresh(address);
    wrefresh(stateBar);
    //Making sure current cell has different border and last cell default
    wborder(windows.at(last.y).at(last.x), '|', '|', '-', '-', '+', '+', '+', '+');
    wborder(windows.at(cell.y).at(cell.x), 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X');
    wrefresh(windows.at(last.y).at(last.x));
    wrefresh(windows.at(cell.y).at(cell.x));
    //Moving cursor from where it was taken
    wmove(input, iny, inx);
}

void Interface::Insert() {
    int c;
    bool read;
    char *text;
    noecho();
    state = State::INSERT;
    read = true;
    while (read) {
        noecho();
        CellPrint();
        InputReset();
        c = wgetch(input);
        switch (c) {
            case 10:
                //Enter pressed waiting for confirmation
                wclear(address);
                wprintw(address, "Insert into -%c%d ?", (char) ('A' + index.x + cell.x), index.y + cell.y);
                wrefresh(address);
                InputReset();
                c = wgetch(input);
                if (c == 10) {
                    wclear(address);
                    wprintw(address, "Input:");
                    wrefresh(address);
                    wmove(input, 1, 1);
                    echo();
                    //Getting input from input window of max size 1000
                    text = new char[1001];
                    wgetnstr(input, text, 1000);
                    InputReset();
                    //Sending cell address and input to table for deconstruction
                    if (table.InsertCell(Location{index.x + cell.x, index.y + cell.y}, text)) {
                        //If insert is successful recalculation will happen
                        table.Update();
                    }
                    delete[] text;
                }
                Draw(true);
                InputReset();
                break;
            case KEY_UP:
                Move(KEY_UP);
                break;
            case KEY_DOWN:
                Move(KEY_DOWN);
                break;
            case KEY_LEFT:
                Move(KEY_LEFT);
                break;
            case KEY_RIGHT:
                Move(KEY_RIGHT);
                break;
            case KEY_F(10):
                read = false;
                break;
            default:
                text += (char) c;
                break;
        }
    }
    InputReset();
}

void Interface::Delete() {
    int c;
    bool read = true;
    state = State::DELETE;
    CellPrint();
    while (read) {
        c = wgetch(input);
        switch (c) {
            case 10:
                //Enter pressed waiting for confirmation
                wclear(address);
                wprintw(address, "Delete -%c%d ?", (char) ('A' + index.x + cell.x), index.y + cell.y);
                wrefresh(address);
                c = wgetch(input);
                if (c == 10) {
                    //Sending table cell address to delete
                    if (table.DeleteCell(Location{index.x + cell.x, index.y + cell.y})) {
                        //If delete is successful recalculation will happen
                        table.Update();
                        Draw(true);
                    }
                }
                InputReset();
                CellPrint();
                break;
            case KEY_UP:
                Move(KEY_UP);
                break;
            case KEY_DOWN:
                Move(KEY_DOWN);
                break;
            case KEY_LEFT:
                Move(KEY_LEFT);
                break;
            case KEY_RIGHT:
                Move(KEY_RIGHT);
                break;
            case KEY_F(10):
                read = false;
                break;
            default:
                break;
        }
    }
}

void Interface::Move(int key) {
    last = cell;
    switch (key) {
        case KEY_LEFT:
            if (cell.x == 0) {
                if (index.x == 0)
                    break;
                index.x--;
                Draw(false);
                break;
            }
            cell.x--;
            if (cell.x < 0)
                cell.x = 0;
            break;

        case KEY_UP:
            if (cell.y == 0) {
                if (index.y == 0)
                    break;
                index.y--;
                Draw(false);
                break;
            }
            cell.y--;
            if (cell.y < 0)
                cell.y = 0;
            break;

        case KEY_RIGHT:
            if (cell.x == requestSize.width - 1) {
                if (index.x + cell.x == table.GetSize().width - 1)
                    break;
                index.x++;
                Draw(false);
                break;
            }
            cell.x++;
            if (cell.x > requestSize.width - 1)
                cell.x = requestSize.width - 1;
            break;

        case KEY_DOWN:
            if (cell.y == requestSize.height - 1) {
                if (index.y + cell.y == table.GetSize().height - 1)
                    break;
                index.y++;
                Draw(false);
                break;
            }
            cell.y++;
            if (cell.y > requestSize.height - 1)
                cell.y = requestSize.height - 1;
            break;
        default:
            break;
    }
    CellPrint();
}

void Interface::Save() {
    int c;
    char *path = nullptr;
    wclear(address);
    wprintw(address, "Save table? (y/n)");
    wrefresh(address);
    InputReset();
    while (true) {
        c = wgetch(input);
        wmove(input, 1, 1);
        if (c == 'y' || c == 'Y') {
            path = new char[501];
            wclear(address);
            wprintw(address, "Enter path with file name");
            wrefresh(address);
            wmove(input, 1, 1);
            echo();
            wgetnstr(input, path, 500);
            InputReset();
            noecho();
            //Sending Table path to which it should save Table
            if (table.Save(path)) {
                Draw(true);
                wmove(input, 1, 1);
                delete[] path;
                wclear(address);
                wprintw(address, "DONE : \"Any\" key to continue");
                wrefresh(address);
                wgetch(input);
                return;
            }
            wclear(address);
            wprintw(address, "ERROR : \"Any\" key to continue");
            wrefresh(address);
            wgetch(input);
        } else if (c == 'n' || c == 'N') {
            delete[] path;
            return;
        } else {
            wclear(address);
            wprintw(address, "Save table? (y/n)");
            wrefresh(address);
        }
    }
}

void Interface::Load() {
    int c;
    char *path = nullptr;
    wclear(address);
    wprintw(address, "Load table? (y/n)");
    wrefresh(address);
    InputReset();
    while (true) {
        c = wgetch(input);
        wmove(input, 1, 1);
        if (c == 'y' || c == 'Y') {
            path = new char[501];
            wclear(address);
            wprintw(address, "Enter path with file name");
            wrefresh(address);
            wmove(input, 1, 1);
            echo();
            wgetnstr(input, path, 500);
            noecho();
            //Sending Table path from which it should load saved Table
            if (table.Load(path)) {
                Draw(true);
                wmove(input, 1, 1);
                delete[] path;
                wclear(address);
                wprintw(address, "DONE : \"Any\" key to continue");
                wrefresh(address);
                wgetch(input);
                InputReset();
                return;
            }
            InputReset();
            wclear(address);
            wprintw(address, "ERROR : \"Any\" key to continue");
            wrefresh(address);
            wgetch(input);
        } else if (c == 'n' || c == 'N') {
            delete[] path;
            return;
        } else {
            wclear(address);
            wprintw(address, "Load table? (y/n)");
            wrefresh(address);
        }
    }
}

void Interface::InputReset() {
    wclear(input);
    box(input, 0, 0);
    wrefresh(input);
    wmove(input, 1, 1);
}

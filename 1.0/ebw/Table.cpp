#include "Table.h"

Table::Table(int rows, Size max) {
    maxCellSize = max;
    size.width = 26;
    //Setting limit fo Table size
    if (rows < 1 || rows > 2000)
        rows = 2000;
    size.height = rows;
    //Initializing and filling widths and heights
    cWidths = vector<int>(size.width);
    rHeights = vector<int>(size.height);
    std::fill(cWidths.begin(), cWidths.end(), 1);
    std::fill(rHeights.begin(), rHeights.end(), 1);
    //Filing height vector with width vectors
    cells = vector<vector<BaseCell *>>(size.height);
    for (auto &a:cells) {
        a = vector<BaseCell *>(size.width);
    }
    //Inserting EmptyCell everywhere
    for (auto &a:cells) {
        for (auto &b:a) {
            b = new EmptyCell();
        }
    }
}

Table::~Table() {
    //Deleting every cell
    for (int i = 0; i < (int) cells.size(); ++i) {
        for (int j = 0; j < (int) cells.at(i).size(); ++j) {
            delete cells.at(i).at(j);
            cells.at(i).at(j) = nullptr;
        }
    }
}

Table &Table::operator=(const Table &table) {
    //Deleting all cells that wont be used anymore
    for (int i = 0; i < (int) cells.size(); ++i) {
        for (int j = 0; j < (int) cells.at(i).size(); ++j) {
            delete cells.at(i).at(j);
            cells.at(i).at(j) = nullptr;
        }
    }
    //Wiping old stuff
    cells.clear();
    cWidths.clear();
    rHeights.clear();
    //Coppying new stuff from table
    for (int i = 0; i < (int) table.cells.size(); ++i) {
        cells.emplace_back();
        for (int j = 0; j < (int) table.cells.at(i).size(); ++j) {
            cells.at(i).push_back(new EmptyCell());
        }
    }
    size = table.size;
    savers = table.savers;
    maxCellSize = table.maxCellSize;
    cWidths = table.cWidths;
    rHeights = table.rHeights;
    return *this;
}

void Table::GetSizes() {
    //Going through all cells and asking them their size
    for (int i = 0; i < size.height; ++i) {
        for (int j = 0; j < size.width; ++j) {
            if (cells.at(i).at(j)->GetSize().height > rHeights.at(i)) {
                rHeights.at(i) = cells.at(i).at(j)->GetSize().height;
            }
            if (cells.at(i).at(j)->GetSize().width > cWidths.at(j)) {
                cWidths.at(j) = cells.at(i).at(j)->GetSize().width;
            }
        }
    }
}

Size Table::GetSize() {
    return size;
}

int Table::RowMax(int input) {
    //If in table
    if (input <= (int) rHeights.size() && input >= 0)
        return (int) rHeights.at(input);
    return 0;
}

int Table::ColMax(int input) {
    //If in table
    if (input <= (int) cWidths.size() && input >= 0)
        return (int) cWidths.at(input);
    return 0;
}

vector<vector<BaseCell *>> Table::RequestCells(WINDOW *window, Location index) {
    int Xmax, Ymax, X = 0, Y = 0;
    vector<vector<BaseCell *>> requestedCells;
    //Getting window size
    getmaxyx(window, Ymax, Xmax);
    //Going from cell at index Location and calculating widht and heigths of cell so they can fit into window
    for (int i = index.y; i < size.height; ++i) {
        if (Y + RowMax(i) + 2 >= Ymax) {
            //Y axis is over limit
            return requestedCells;
        }
        //Inserting new line of cells
        requestedCells.emplace_back();
        for (int j = index.x; j < size.width; ++j) {
            //X axis is over limit
            if (X + ColMax(j) + 2 >= Xmax) {
                break;
            }
            //Saving cell's pointer
            requestedCells.back().push_back(cells.at(i).at(j));
            //Moving along X axis
            X += ColMax(j) + 2;
        }
        //Reseting X axis
        X = 0;
        //Moving along Y axis
        Y += RowMax(i) + 2;
    }
    return requestedCells;
}

bool Table::InsertCell(Location addr, const string &input) {
    char type = 0;
    string value;
    //Control if there is any resonable input
    if (input.empty() || input.length() == 1)
        return false;
    if (input.length() == 2) {
        value = "";
    }
    //Control if input coresponds with syntax
    if (input.length() > 2) {
        type = input[0];
        if (input[1] == '=')
            value = input.substr(2, input.length() - 2);
        else
            return false;
    }
    //Getting type of cell
    type = (char) std::tolower(type);
    switch (type) {
        case 'e':
            //Inserting ExpresinCell with check for loops
            delete cells.at(addr.y).at(addr.x);
            cells.at(addr.y).at(addr.x) = new ExpresionCell(value, maxCellSize);
            if (!CycleCheck(addr))
                cells.at(addr.y).at(addr.x)->Kill();
            if (DataTransfer(cells.at(addr.y).at(addr.x))) {
                savers.push_back(addr);
                updaters.push_back(addr);
                return true;
            }
            return false;
        case 'f':
            //Inserting FunctionCell with check for loops
            delete cells.at(addr.y).at(addr.x);
            cells.at(addr.y).at(addr.x) = new FunctionCell(value, maxCellSize);
            if (!CycleCheck(addr))
                cells.at(addr.y).at(addr.x)->Kill();
            if (DataTransfer(cells.at(addr.y).at(addr.x))) {
                savers.push_back(addr);
                updaters.push_back(addr);
                return true;
            }
            return false;
        case 'a':
            //Inserting AggregationCell with check for loops
            delete cells.at(addr.y).at(addr.x);
            cells.at(addr.y).at(addr.x) = new AggregationCell(value, maxCellSize);
            if (!CycleCheck(addr))
                cells.at(addr.y).at(addr.x)->Kill();
            if (DataTransfer(cells.at(addr.y).at(addr.x))) {
                savers.push_back(addr);
                updaters.push_back(addr);
                return true;
            }
            return false;
        case 't':
            //Inserting TextCell
            delete cells.at(addr.y).at(addr.x);
            cells.at(addr.y).at(addr.x) = new TextCell(value, maxCellSize);
            savers.push_back(addr);
            return true;
        case 'n':
            //Inserting NumberCell
            delete cells.at(addr.y).at(addr.x);
            cells.at(addr.y).at(addr.x) = new NumberCell(value, maxCellSize);
            savers.push_back(addr);
            return true;
        case 'b':
            //Inserting BoolCell
            delete cells.at(addr.y).at(addr.x);
            cells.at(addr.y).at(addr.x) = new BoolCell(value, maxCellSize);
            savers.push_back(addr);
            return true;
        case 'd':
            //Inserting DateCell
            delete cells.at(addr.y).at(addr.x);
            cells.at(addr.y).at(addr.x) = new DateCell(value, maxCellSize);
            savers.push_back(addr);
            return true;
        default:
            //Inserting EmptyCell because input was invalid
            delete cells.at(addr.y).at(addr.x);
            cells.at(addr.y).at(addr.x) = new EmptyCell();
            return true;
    }
}

bool Table::DeleteCell(Location addr) {
    //Skip if EmptyCell
    if (cells.at(addr.y).at(addr.x)->GetType() == Type::empty)
        return false;
    //Reseting size and deleting cell and replacing it with EmptyCell
    cWidths.at(addr.x) = 1;
    rHeights.at(addr.y) = 1;
    delete cells.at(addr.y).at(addr.x);
    cells.at(addr.y).at(addr.x) = new EmptyCell();
    for (int i = 0; i < (int) savers.size(); ++i) {
        if (addr.y == savers[i].y && addr.x == savers[i].x) {
            savers.erase(savers.begin() + i);
            break;
        }
    }
    return true;
}

bool Table::Save(const string &fileLocation) {
    //Opening file and trying to check if permisions allow to save it there
    std::ofstream outFile(fileLocation, std::ofstream::out);
    if (outFile.fail()) {
        outFile.close();
        return false;
    }
    //Saving necessary  parameters
    if (outFile.is_open()) {
        if (!(outFile << size.width << ':' << size.height << endl))
            return false;
        for (auto &a:savers) {
            if (!(outFile << a.x << ':' << a.y << ';' << cells.at(a.y).at(a.x)->GetType() << ';' << cells.at(a.y).at(a.x)->Input() << std::endl)) {
                outFile.close();
                return false;
            }
        }
        outFile.close();
        return true;
    }
    outFile.close();
    return false;
}

bool Table::Load(const string &fileLocation) {
    char c;
    string input;
    Size tempSize, sizeBackup;
    vector<string> rows;
    vector<Location> saversBackup = savers;
    vector<vector<BaseCell *>> cellsbackup = cells;
    std::ifstream ifs(fileLocation, std::ifstream::in);
    //Reading saved file saving by rows
    while (ifs.good()) {
        c = (char) ifs.get();
        if (c == '\n') {
            rows.push_back(input);
            input = "";
            continue;
        }
        input += c;
    }
    if (rows.empty())
        return false;
    //Backing up stuff if load is unsuccessful
    savers.clear();
    sizeBackup = size;
    //Getting saved Table size
    size.width = GiveXYFromString(rows.at(0)).x;
    size.height = GiveXYFromString(rows.at(0)).y;
    vector<int> rBackup = rHeights, cBackup = cWidths;
    cells.clear();
    cWidths.clear();
    rHeights.clear();
    cWidths = vector<int>(size.width);
    rHeights = vector<int>(size.height);
    std::fill(cWidths.begin(), cWidths.end(), 1);
    std::fill(rHeights.begin(), rHeights.end(), 1);
    cells = vector<vector<BaseCell *>>(size.height);
    for (auto &a:cells) {
        a = vector<BaseCell *>(size.width);
    }
    // filing "empty" vectors with empty cells
    for (auto &a:cells) {
        for (auto &b:a) {
            b = new EmptyCell();
        }
    }
    //Dissecting each row for address,type and input string
    for (int i = 1; i < (int) rows.size(); ++i) {
        string loc, type, val;
        bool l = false, t = false;
        for (auto &a:rows[i]) {
            if (!l) {
                if (a == ';') {
                    l = true;
                    continue;
                }
                loc += a;
                continue;
            }
            if (!t) {
                if (a == ';') {
                    t = true;
                    continue;
                }
                type += a;
                continue;
            }
            val += a;
        }
        c = GiveTypeFromString(type);
        val.insert(val.begin(), '=');
        val.insert(val.begin(), c);
        //When row dissected try ti insert
        //If successful continue otherwise delete currently loaded stuff and restore backup.
        if (!InsertCell(GiveXYFromString(loc), val)) {
            size = sizeBackup;
            for (auto &a:cells) {
                for (auto &b:a) {
                    delete b;
                    b = nullptr;
                }
            }
            cells = cellsbackup;
            savers = saversBackup;
            rHeights = rBackup;
            cWidths = cBackup;
            return false;
        }
    }
    //All went well deleting backups
    for (auto &a:cellsbackup) {
        for (auto &b:a) {
            delete b;
            b = nullptr;
        }
    }
    saversBackup.clear();
    rBackup.clear();
    cBackup.clear();
    ifs.close();
    return true;
}

void Table::Update() {
    for (auto &i:updaters) {
        if (DataTransfer(cells.at(i.y).at(i.x))) {
            rHeights.at(i.y) = cells.at(i.y).at(i.x)->GetSize().height;
            cWidths.at(i.x) = cells.at(i.y).at(i.x)->GetSize().width;
        }
    }
}

BaseCell *Table::GetCell(Location location) {
    if (location.x < size.width && location.y < size.height && location.x >= 0 && location.y >= 0)
        return cells.at(location.y).at(location.x);
    return nullptr;
}

Location Table::GiveXYFromString(const string &input) {
    bool flag = false;
    string x, y;
    //add to one value until ':' then to second
    for (char i:input) {
        if (i == ':') {
            flag = true;
            continue;
        }
        if (!flag)
            x += i;
        else
            y += i;
    }
    //convert both to ints and return Location
    return Location{(int) std::strtol(x.c_str(), nullptr, 10), (int) std::strtol(y.c_str(), nullptr, 10)};
}

char Table::GiveTypeFromString(const string &input) {
    if (input.empty())
        return 0;
    switch (input[0]) {
        case 0:
            return 'E';
            break;
        case '1':
            return 't';
            break;
        case '2':
            return 'n';
            break;
        case '3':
            return 'b';
            break;
        case '4':
            return 'd';
            break;
        case '5':
            return 'e';
            break;
        case '6':
            return 'f';
            break;
        case '7':
            return 'a';
            break;
        default:
            return 0;
    }
}

bool Table::Address(const string &addr, Location &loc) const {
    string letter, number;
    //Checking if there are only alloved chracters
    for (auto i:addr) {
        if (i >= '0' && i <= '9')
            number += i;
        if (std::toupper(i) >= 'A' && std::toupper(i) <= 'Z')
            letter += std::toupper(i);
    }//Making sure it's in Table
    if (letter.length() == 1) {
        loc.x = letter[0] - 'A';
        loc.y = (int) strtol(number.c_str(), nullptr, 10);
        if (loc.y == 0 && number.length() != 1) {
            return false;
        }
        if (loc.x < size.width && loc.x >= 0 && loc.y < size.height && loc.y >= 0)
            return true;
    }
    return false;
}

bool Table::DataTransfer(BaseCell *cell) {
    Type type = cell->GetType();
    Location location;
    if (type == Type::expresion) {
        //Cast for GetValues()
        auto *expr = dynamic_cast<ExpresionCell *>(cell);
        //Getting all linked addresses
        vector<string> linked = expr->RequestInfo();
        vector<double> results;
        //Checking if they are in the Table
        for (auto &a:linked) {
            if (Address(a, location)) {
                results.push_back(cells.at(location.y).at(location.x)->GetNumber());
            } else {
                expr->GetValues(vector<double>());
                return false;
            }
        }
        //Telling cell to calculate itself
        expr->GetValues(results);
    } else if (type == Type::function) {
        auto *expr = dynamic_cast<FunctionCell *>(cell);
        //Getting all linked addresses
        vector<string> linked = expr->RequestInfo();
        vector<Carier> results;
        //Checking if they are in the Table
        for (auto &a:linked) {
            if (Address(a, location)) {
                //FunctionCell needs differend types of info. That is why Carier is used
                Carier car;
                car.type = cells.at(location.y).at(location.x)->GetType();
                if (car.type == Type::function) {
                    auto *temp = dynamic_cast<FunctionCell *>(cells.at(location.y).at(location.x));
                    if (temp->subType == Type::number) {
                        car.type = Type::number;
                        car.text = "";
                        car.number = cells.at(location.y).at(location.x)->GetNumber();
                    } else {
                        car.type = Type::text;
                        car.text = cells.at(location.y).at(location.x)->Display();
                        car.number = 0;
                    }
                } else {
                    car.text = cells.at(location.y).at(location.x)->Display();
                    car.number = cells.at(location.y).at(location.x)->GetNumber();
                }
                results.push_back(car);
            } else {
                //Telling cell to calculate itself
                expr->GetValues(vector<Carier>());
                return false;
            }
        }
        expr->GetValues(results);
    } else if (type == Type::aggregation) {
        auto *expr = dynamic_cast<AggregationCell *>(cell);
        //Getting all linked addresses
        vector<string> linked = expr->RequestInfo();
        //Because AggregationCell have ranges DisectLink() is called which can handle multiple types of input
        vector<Location> disected = DissectLink(linked);
        if (disected.empty()) {
            return false;
        }
        vector<Carier> results;
        //For all cells from dissect create Carier
        for (auto &a:disected) {
            Carier car;
            car.type = cells.at(a.y).at(a.x)->GetType();
            if (car.type == Type::empty) {
                car.text = "";
                car.number = 0;
                results.push_back(car);
                continue;
            }
            if (car.type == Type::text) {
                car.text = cells.at(a.y).at(a.x)->Display();
                car.number = 0;
                results.push_back(car);
                continue;
            }
            if (car.type == Type::number) {
                car.text = "";
                car.number = cells.at(a.y).at(a.x)->GetNumber();
                results.push_back(car);
                continue;
            }
            if (car.type == Type::boolean) {
                car.text = "";
                car.number = cells.at(a.y).at(a.x)->GetNumber();
                results.push_back(car);
                continue;
            }
            if (car.type == Type::date) {
                car.text = cells.at(a.y).at(a.x)->Display();
                car.number = 0;
                results.push_back(car);
                continue;
            }
            if (car.type == Type::expresion) {
                car.text = "";
                car.number = cells.at(a.y).at(a.x)->GetNumber();
                results.push_back(car);
                continue;
            }
            if (car.type == Type::function) {
                //Cast for control of subtype
                auto *temp = dynamic_cast<FunctionCell *>(cells.at(a.y).at(a.x));
                if (temp->subType == Type::number) {
                    car.type = Type::number;
                    car.text = "";
                    car.number = cells.at(a.y).at(a.x)->GetNumber();
                    results.push_back(car);
                }
            }
            if (car.type == Type::aggregation) {
                car.text = "";
                car.number = cells.at(a.y).at(a.x)->GetNumber();
                results.push_back(car);
                continue;
            }
        }
        //Telling cell to calculate itself
        expr->GetValues(results);
    }
    return true;
}

vector<Location> Table::DissectLink(vector<string> &linked) const {
    if (linked.empty()) {
        return vector<Location>();
    }
    string addresses = linked[0];
    vector<Location> outLoc;
    bool text = false, number = false, dash = false;
    //Checking for what is in the text
    for (auto &i:addresses) {
        //Contains letter
        if (i >= '0' && i <= '9')
            number = true;
        //Contains numbers
        if (i >= 'A' && i <= 'Z')
            text = true;
        //Contains multiple inputs
        if (i == ',')
            dash = true;
    }
    //Only text means that input should be whole column
    if (text && !number && !dash && addresses.length() == 1) {
        linked.clear();
        //Parsing string to Location for column
        for (int i = 0; i < size.height; ++i) {
            Location x;
            x.x = addresses[0] - 'A';
            x.y = i;
            outLoc.push_back(x);
        }
        return outLoc;
    }
    //Only number means that input should be whole row
    if (!text && number && !dash) {
        linked.clear();
        //Parsing string to Location for row
        for (int i = 0; i < size.width; ++i) {
            Location x;
            x.x = i;
            x.y = (int) std::strtol(addresses.c_str(), nullptr, 10);
            outLoc.push_back(x);
        }
        return outLoc;
    }
    //No dash is one address
    if (text && number && !dash) {
        Location x;
        //Parsing string to Location
        if (Address(addresses, x))
            outLoc.push_back(x);
        return outLoc;
    }
    //Text, number and dash are two addresses
    if (text && number) {
        string first, second;
        Location floc, sloc, temp;
        bool flag = false;
        //Dividing them by ','
        for (char &i:addresses) {
            if (i == ',') {
                flag = true;
                continue;
            }
            if (!flag)
                first += i;
            else second += i;
        }
        //Parsing string to Location
        //Two addresses mean that it is range therefor it must go from one corner to other
        //This is just comparing what is top/bottom and left/right corner of range
        if (Address(first, floc) && Address(second, sloc)) {
            if (floc.x > sloc.x) {
                if (floc.y > sloc.y) {
                    for (int i = sloc.x; i <= floc.x; ++i) {
                        for (int j = sloc.y; j <= floc.y; ++j) {
                            temp.x = i;
                            temp.y = j;
                            outLoc.push_back(temp);
                        }
                    }
                } else {
                    for (int i = sloc.x; i <= floc.x; ++i) {
                        for (int j = floc.y; j <= sloc.y; ++j) {
                            temp.x = i;
                            temp.y = j;
                            outLoc.push_back(temp);
                        }
                    }
                }
            } else {
                if (floc.y > sloc.y) {
                    for (int i = floc.x; i <= sloc.x; ++i) {
                        for (int j = sloc.y; j <= floc.y; ++j) {
                            temp.x = i;
                            temp.y = j;
                            outLoc.push_back(temp);
                        }
                    }
                } else {
                    for (int i = floc.x; i <= sloc.x; ++i) {
                        for (int j = floc.y; j <= sloc.y; ++j) {
                            temp.x = i;
                            temp.y = j;
                            outLoc.push_back(temp);
                        }
                    }
                }
            }
            return outLoc;
        }
        outLoc.clear();
        return outLoc;
    }
    return vector<Location>();
}

bool Table::CycleCheck(Location location) {
    map<string, int> counter;
    vector<vector<string>> aller;
    //Getting all linked strings for cell at locations
    vector<string> all = DisectString(location);
    aller.push_back(all);
    //Parsing location to strin format
    string org = std::to_string('A' + location.x) + std::to_string(location.y);
    //Calling recursive loop reach
    //If it's true origin cell is inserted and conpared to other if there is loop now
    if (RecursiveAddresses(aller)) {
        all.push_back(org);
        for (auto &i:aller) {
            for (auto &j:i) {
                counter[j]++;
            }
        }
        //Counting how many times there are all cells and if its >1 there is loop
        for (auto &i:counter) {
            if (i.second > 1) {
                Location loc;
                Address(i.first, loc);
                if (!cells.at(loc.y).at(loc.x)->RequestInfo().empty()) {
                    return false;
                }
            }
        }
        return true;
    }
    return false;
}

bool Table::RecursiveAddresses(vector<vector<string>> &all) {
    Location loc;
    //For every address that links somewhere
    for (auto &j:all.back()) {
        if (Address(j, loc)) {
            if (cells.at(loc.y).at(loc.x)->RequestInfo().empty()) {
                continue;
            }
            for (auto &k:all) {
                if (&k != &all.back())
                    for (auto &m:k) {
                        if (m == j)
                            return false;
                    }
            }
            //Save where it leads and recursively search in it
            all.push_back(DisectString(loc));
            RecursiveAddresses(all);
        } else {
            return false;
        }
    }
    return true;
}

vector<string> Table::DisectString(Location location) {
    vector<Location> agr;
    vector<string> in = cells.at(location.y).at(location.x)->RequestInfo(), out;
    //AggregationCell has ranges so it needs to be handled differently
    switch (cells.at(location.y).at(location.x)->GetType()) {
        case Type::aggregation:
            //Get all linked cells and get their string address format
            agr = DissectLink(in);
            for (auto &i:agr) {
                out.push_back(LocToStr(i));
            }
            return out;
            break;
        default:
            return cells.at(location.y).at(location.x)->RequestInfo();
            break;
    }
    return vector<string>();
}

string Table::LocToStr(Location location) {
    string x;
    char tmp = (char) ('A' + location.x);
    x += (char) tmp;
    x += std::to_string(location.y);
    return x;
}












#ifndef EBW_TABLE_H
#define EBW_TABLE_H

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include <ncurses.h>
#include <iostream>
#include <fstream>
#include "helpTypes.h"
#include "BaseCell.h"

using std::string;
using std::vector;
using std::cout;
using std::endl;

//!Class managing all cells
class Table {
public:
    //!@param[in] rows Defines Table height.
    //!@param[in] max Defines how big can cell be to fin into display.
    explicit Table(int rows, Size max);

    //!Destructor to free all allocated cells
    ~Table();

    Table &operator=(const Table &table);

    //!Calculating maximum sizes of rows and columns.
    /**Going through all cells and comparing their sizes and storing maximum values for rows and columns.*/
    void GetSizes();

    //!Inserting cell to Table.
    //!@param[in] addr Locations in table
    //!@param[in] input Raw text
    //!@return True if inserted without critical errors(pointing outside Table,...) otherwise false
    bool InsertCell(Location addr, const string &input);

    //!Removing cell from Table.
    //!@param[in] addr Locations in table.
    //!@return True if deleted, false if EmptyCell
    bool DeleteCell(Location addr);

    //!Saving content of table to file.
    //!@param[in] location File path
    //!@return True if save was successful otherwise false
    /**It saves as size of the Table and series of command to restore table to state is was saved from.*/
    bool Save(const string &location);

    //!Loading Table from file
    //!@param[in] location File path
    //!@return True if load was successful otherwise false
    /**It deletes allocated values in Table and then proceeds to resize Table if needed and insert cells stored in file.*/
    bool Load(const string &location);

    //!Returns Table size
    //!@return Size of Table(cells width and height)
    /**Which correspond to cells height and width*/
    Size GetSize();

    //!Gets pointer to cell at certain location in cells
    //!@param[in] location Cell's location in Table
    //!@return Pointer to cell at location
    BaseCell *GetCell(Location location);

    //!Calculating and returning cells that can fit to *window starting from index
    //!@param[in,out] window Display window
    //!@param[in] index Locations of starting cell
    //!@return All cells that can fit to window
    vector<vector<BaseCell *>> RequestCells(WINDOW *window, Location index);

    //!@\return Value from rHeights at input
    int RowMax(int input);

    //!@\return Value from cWidths at input
    int ColMax(int input);

    //!Recalculating signal to all cells that calculate
    void Update();

    //!Simple 1:1 conversion from string(char) to another char
    /**Method to make code cleaner*/
    static char GiveTypeFromString(const string &input);

private:
    //!Asking cell what values it needs to calculate itself
    /**Getting values cell asks for if they are in the Table and transform them to format cell takes and send command to cell to make sense of them.*/
    //!@param[in] cell Points to cell which needs to be calculated
    //!@return True if there were no problem otherwise False
    bool DataTransfer(BaseCell *cell);
    //!Translating cell's string addresses to Table Location addresses
    //!@\param[in] linked Vector of addresses in string format
    //!@\return Location format of addresses in linked
    /**Managing single, multiple and range address inputs and if it's possible translate them to Locations form.*/
    vector<Location> DissectLink(vector<string> &linked) const;

    //!Initial call for search if cell is in a loop with another
    //!@param[in] location Location of cell which is being checked
    //!@return True when no loop was found otherwise false
    bool CycleCheck(Location location);

    //!Going through all successively linked cells and control if it made it's waz back to starting
    //!@param[in] all Vectors of all addresses in string format
    //!@return True if no cycle was found otherwise false
    bool RecursiveAddresses(vector<vector<string>> &all);

    //!Taking cells locations and creating string output what is it pointing to.
    //!\param[in] location Location address format
    //!\return Vector of strings that cell on location uses for calculation
    /**Some cells have different styles of linking to other cells.
     * This method makes sure they are all in same format. Especially for AggregationCell which uses ranges.*/
    vector<string> DisectString(Location location);

    //!Converting Location format back to it's string form ([letter][number])
    //!\param[in] location Location address format
    //!\return String format of location
    static string LocToStr(Location location);

    //!Takes string address format and translates it to Location.
    //!\param[in] input String address format (incorrect syntax => error)
    //!\param[in] location Location format of input.
    //!\return True on successful conversion and check if in Table otherwise False.
    bool Address(const string &input, Location &location) const;

    //!Parsing from string to Location
    /**No checking if address is in the Table.*/
    //!@param[in] input String address format
    //!@return Location format from input
    static Location GiveXYFromString(const string &input);

    //!2D vector of BaseCell pointers that represent all content of the Table
    vector<vector<BaseCell *>> cells;
    //!Locations of all cell that will be saved.
    /**Only correctly inputted cells or killed cells.*/
    vector<Location> savers,
    //!Locations of all cell that require update of value.
    /**If something was inserted or deleted cells that point to others need to update if any value was changed.*/
    updaters;
    //!Vector of maximum widths of columns.
    vector<int> cWidths;
    //!Vector of maximum height of rows.
    vector<int> rHeights;
    //!Contains width and height of cells.
    Size size,
    //!Inherited size from display window
    maxCellSize;
};

#endif //EBW_TABLE_H

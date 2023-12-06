//!Interface functions as bridge between user input and logical structure of a Table.
/**It manages displaying content of Table and provides it with input data. Interface does not do any extreme calculations and operations.
 * It's main purpose is managing ncurses library and and provide user with UI that is not pure text but has a little style and logic to it.
 * */
#ifndef EBW_INTERFACE_H
#define EBW_INTERFACE_H

#include <ncurses.h>
#include "Table.h"

class Interface {

public:
    //!Constructor that initializes ncurses interface and creates table with rows number of lines.
    //!@param[in] rows Defines how many rows will table have
    explicit Interface(int rows);

private:
    //!Control part of interface that decides what will happen next depending on input from user.
    void Run();

    //!Renders contents of Table and additional windows.
    //!@param[in] resize Enables calculation for resizing (not every time it's needed to recalculate cell sizes)
    void Draw(bool resize);

    //!Removes unused windows from memory
    void ClearWindows();
    //!Sending input to Table
    /**It does not try to parse the input to check if it's correct or not. It just takes actual address of cell in which something should be inserted and sends it to Table.
     * */
    void Insert();
    //!Removing content from Table
    /**Delete takes an actual address of selected cell and tells Table to delete it.
     * */
    void Delete();
    //!Selects cell
    //!@param[in] key Ncurses representation of keyboard key
    /**While cell is in range of an requested cells decrease or increase its value depending on input.
    * When it tryes to go out of displayed field it will move index which will cause ne part of Table to display.
    * Tracks keyboard arrow and based on pressed key moves in it's direction.*/
    void Move(int key);

    //!Gives Table command to save itself to file
    /**Needs confirmation or declination for continuing. If any other input issued it's stuck in loop.*/
    void Save();

    //!Gives Table command to load Table from file
    /**Needs confirmation or declination for continuing. If any other input issued it's stuck in loop.*/
    void Load();

    //!Displays currently selected cell
    void CellPrint();

    //!Clearing input window and rendering it anew
    void InputReset();
    //!Index is starting point for rendering content from Table.
    /**Representing the top left corner from which Interface asks Table to get all cells that can fit.*/
    Location index,
    //!Cell represents current location of selected cell in rendered part of Table
    /**In the currently displayed part of table. Combined with index it creates 1:1 location of cell in the table.
     * */
    cell,
    //!Last marks last selected cell.
    /**It's used to set border of cells window to default value.
     * */
    last;
    //!displaySize contains width and height of display window.
    Size displaySize,
    //!requestSize contains width and height of part of the Table.
    requestSize;
    //!State changes depending in which mode user is.
    /**It's purely for cosmetic purposes.*/
    State state;
    //!Table contains all information about cells
    Table table = Table(1, Size{1, 1});
    //!Display is WINDOW that "prints" out all information in cells.
    /**It's mostly area that defines borders for other windows that then prints cell content.
     * */
    WINDOW *display,
    //!Legend is WINDOW that describes cell location
    /**On top it displays corresponding letters for columns and on the side for rows based on index and cell.
     * */
    *legend,
    //!Input is WINDOW that reads and shows user input.
    *input,
    //!StateBar is WINDOW that displays controls and guidelines.
    *stateBar,
    //!Address is WINDOW that displays current state, cell address and it's value.
    *address;
    //!Windows contain all windows that are being used to display cells
    vector<vector<WINDOW *>> windows;
};


#endif //EBW_INTERFACE_H

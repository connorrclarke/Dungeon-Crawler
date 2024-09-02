#include <iostream>
#include <fstream>
#include <string>
#include "logic.h"

using std::cout, std::endl, std::ifstream, std::string;

/**
 * TODO: Student implement this function
 * Load representation of the dungeon level from file into the 2D map.
 * Calls createMap to allocate the 2D array.
 * @param   fileName    File name of dungeon level.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @param   player      Player object by reference to set starting position.
 * @return  pointer to 2D dynamic array representation of dungeon map with player's location., or nullptr if loading fails for any reason
 * @updates  maxRow, maxCol, player
 */
char** loadLevel(const string& fileName, int& maxRow, int& maxCol, Player& player) {

    //open the file
    ifstream ifs(fileName);
    if(!ifs.is_open()){
        cout << "Error - Unable to open the file " << fileName << endl;
        return nullptr;
    }

    //Read the map dimensions
    ifs >> maxRow >> maxCol;
    if (maxRow <= 0 || maxCol <= 0 || maxRow > 999999 || maxCol > 999999 || maxRow > INT32_MAX / maxCol) {
        cout << "Error - Input file provides invalid dimensions" << endl;
        ifs.close();
        maxRow = 0;
        maxCol = 0;
        return nullptr;
    }

    //Read the player starting location
    int startRow;
    int startCol;
    ifs >> startRow >> startCol;
    if (startRow < 0 || startCol < 0 || startRow >= maxRow || startCol >= maxCol){
        cout << "Error - Starting location is out of bounds" << endl;
        ifs.close();
        maxRow = 0;
        maxCol = 0;
        return nullptr;
    }
    //Initialize starting location
    player.row = startRow;
    player.col = startCol;

    //make and fill the map
    char** map = createMap(maxRow, maxCol);
    bool hasDoor = false;
    for (int row = 0; row < maxRow; row++){
        for (int col = 0; col < maxCol; col++){
            
            char tile;
            if(!(ifs >> tile)){
                cout << "Error - Could not read the spot from the file" << endl;
                deleteMap(map, maxRow);
                ifs.close();
                maxRow = 0;
                maxCol = 0;
                return nullptr;
            }

            //checks for ivalid tiles
            if (tile != TILE_OPEN && tile != TILE_PLAYER && tile != TILE_TREASURE && tile != TILE_AMULET && tile != TILE_MONSTER && tile != TILE_PILLAR && tile != TILE_DOOR && tile != TILE_EXIT){
                cout << "Error - There is an invalid tile symbol in the input file" << endl;
                deleteMap(map, maxRow);
                ifs.close();
                maxRow = 0;
                maxCol = 0;
                return nullptr;
            }

            //level needs to have door or exit in order to be valid level
            if (tile == TILE_DOOR || tile == TILE_EXIT){
                hasDoor = true;
            }

            //fill tiles
            if (row == player.row && col == player.col){
                map[row][col] = TILE_PLAYER;
            }
            else{
                map[row][col] = tile;
            }
        }
    }

    //check for extra values in input file
    string extra = "";
    if(ifs >> extra){
        deleteMap(map, maxRow);
        ifs.close();
        maxRow = 0;
        maxCol = 0;
        return nullptr;
    }

    //checks to see if door or exit is in level
    if (!hasDoor){
        deleteMap(map, maxRow);
        ifs.close();
        maxRow = 0;
        maxCol = 0;
        return nullptr;
    }

    ifs.close();

    return map;
}

/**
 * TODO: Student implement this function
 * Translate the character direction input by the user into row or column change.
 * That is, updates the nextRow or nextCol according to the player's movement direction.
 * @param   input       Character input by the user which translates to a direction.
 * @param   nextRow     Player's next row on the dungeon map (up/down).
 * @param   nextCol     Player's next column on dungeon map (left/right).
 * @updates  nextRow, nextCol
 */
void getDirection(char input, int& nextRow, int& nextCol) {
    if (input == MOVE_UP){
        nextRow--;
    }
    else if (input == MOVE_DOWN){
        nextRow++;
    }
    else if (input == MOVE_LEFT){
        nextCol--;
    }
    else if (input == MOVE_RIGHT){
        nextCol++;
    }
}

/**
 * TODO: Student implement this function
 * Allocate the 2D map array.
 * Initialize each cell to TILE_OPEN.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @return  2D map array for the dungeon level, holds char type.
 */
char** createMap(int maxRow, int maxCol) {

    if (maxRow > 999999 || maxCol > 999999 || maxRow <= 0 || maxCol <= 0){
        return nullptr;
    }

    char** map = new char*[maxRow];
    for (int row = 0; row < maxRow; row++){
        map[row] = new char[maxCol];
    }
    for (int row = 0; row < maxRow; row++){
        for (int col = 0; col < maxCol; col++){
            map[row][col] = TILE_OPEN;
        }
    }
    return map;
}

/**
 * TODO: Student implement this function
 * Deallocates the 2D map array.
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @return None
 * @update map, maxRow
 */
void deleteMap(char**& map, int& maxRow) {
    if (map != nullptr){
        for (int row = 0; row < maxRow; row++){
            if (map[row] != nullptr) {
                delete [] map[row];
                map[row] = nullptr;
            }
        }
        delete [] map;
    }
    map = nullptr;
    maxRow = 0;
}

/**
 * TODO: Student implement this function
 * Resize the 2D map by doubling both dimensions.
 * Copy the current map contents to the right, diagonal down, and below.
 * Do not duplicate the player, and remember to avoid memory leaks!
 * You can use the STATUS constants defined in logic.h to help!
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height), to be doubled.
 * @param   maxCol      Number of columns in the dungeon table (aka width), to be doubled.
 * @return  pointer to a dynamically-allocated 2D array (map) that has twice as many columns and rows in size.
 * @update maxRow, maxCol
 */
char** resizeMap(char** map, int& maxRow, int& maxCol) {

    int newRowSize = maxRow * 2;
    int newColSize = maxCol * 2;

    if (map == nullptr || newRowSize > 999999 || newRowSize <= 0 || newColSize > 999999 || newColSize <= 0){
        return nullptr;
    }

    char** resizedMap = createMap(newRowSize, newColSize);

    for (int row = 0; row < maxRow; row++){
        for (int col = 0; col < maxCol; col++){
            resizedMap[row][col] = map[row][col];
            resizedMap[row][col + maxCol] = map[row][col];
            resizedMap[row + maxRow][col] = map[row][col];
            resizedMap[row + maxRow][col + maxCol] = map[row][col];

            if (map[row][col] == TILE_PLAYER){
                resizedMap[row][col + maxCol] = TILE_OPEN;
                resizedMap[row + maxRow][col] = TILE_OPEN;
                resizedMap[row + maxRow][col + maxCol] = TILE_OPEN;
            }
        }
    }

    deleteMap(map, maxRow);

    maxRow = newRowSize;
    maxCol = newColSize;

    return resizedMap;
}

/**
 * TODO: Student implement this function
 * Checks if the player can move in the specified direction and performs the move if so.
 * Cannot move out of bounds or onto TILE_PILLAR or TILE_MONSTER.
 * Cannot move onto TILE_EXIT without at least one treasure. 
 * If TILE_TREASURE, increment treasure by 1.
 * Remember to update the map tile that the player moves onto and return the appropriate status.
 * You can use the STATUS constants defined in logic.h to help!
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @param   player      Player object to by reference to see current location.
 * @param   nextRow     Player's next row on the dungeon map (up/down).
 * @param   nextCol     Player's next column on dungeon map (left/right).
 * @return  Player's movement status after updating player's position.
 * @update map contents, player
 */
int doPlayerMove(char** map, int maxRow, int maxCol, Player& player, int nextRow, int nextCol) {

    //check if in bounds
    if (nextRow >= maxRow || nextRow < 0 || nextCol >= maxCol || nextCol < 0){
        nextRow = player.row;
        nextCol = player.col;
        return STATUS_STAY;
    }

    char nextTile = map[nextRow][nextCol];
    switch (nextTile){
        //if tile is pillar or monster player cant move there
        case TILE_PILLAR:
        case TILE_MONSTER:
            nextRow = player.row;
            nextCol = player.col;
            return STATUS_STAY;

        case TILE_TREASURE:
            map[player.row][player.col] = TILE_OPEN;
            map[nextRow][nextCol] = TILE_PLAYER;
            player.row = nextRow;
            player.col = nextCol;
            player.treasure++;
            return STATUS_TREASURE;

        case TILE_AMULET:
            map[player.row][player.col] = TILE_OPEN;
            map[nextRow][nextCol] = TILE_PLAYER;
            player.row = nextRow;
            player.col = nextCol;
            return STATUS_AMULET;

        case TILE_DOOR:
            map[player.row][player.col] = TILE_OPEN;
            map[nextRow][nextCol] = TILE_PLAYER;
            player.row = nextRow;
            player.col = nextCol;
            return STATUS_LEAVE;

        case TILE_EXIT:
            if (player.treasure > 0){
                map[player.row][player.col] = TILE_OPEN;
                map[nextRow][nextCol] = TILE_PLAYER;
                player.row = nextRow;
                player.col = nextCol;
                return STATUS_ESCAPE;
            }
            else {
                nextRow = player.row;
                nextCol = player.col;
                return STATUS_STAY;
            }

        //player moving normally
        case TILE_OPEN:
            map[player.row][player.col] = TILE_OPEN;
            map[nextRow][nextCol] = TILE_PLAYER;
            player.row = nextRow;
            player.col = nextCol;
            return STATUS_MOVE;

        default:
            nextRow = player.row;
            nextCol = player.col;
            return STATUS_STAY;
    }
}

/**
 * TODO: Student implement this function
 * Update monster locations:
 * We check up, down, left, right from the current player position.
 * If we see an obstacle, there is no line of sight in that direction, and the monster does not move.
 * If we see a monster before an obstacle, the monster moves one tile toward the player.
 * We should update the map as the monster moves.
 * At the end, we check if a monster has moved onto the player's tile.
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @param   player      Player object by reference for current location.
 * @return  Boolean value indicating player status: true if monster reaches the player, false if not.
 * @update map contents
 */
bool doMonsterAttack(char** map, int maxRow, int maxCol, const Player& player) {

    bool isDead = false;

/**
    //up side
    for (int col = player.col - 1; col >= 0; col--){
        if (map[player.row][col] == TILE_PILLAR){
            break;
        }
        if (map[player.row][col] == TILE_MONSTER){
            if (col == player.col - 1){
                isDead = true;
                map[player.row][col] = TILE_OPEN;
                map[player.row][player.col] = TILE_MONSTER;
            }
            else{
                map[player.row][col] = TILE_OPEN;
                map[player.row][col - 1] = TILE_MONSTER;
            }
        }
    }

    //down side
    for (int col = player.col + 1; col < maxCol; col++){
        if (map[player.row][col] == TILE_PILLAR){
            break;
        }
        if (map[player.row][col] == TILE_MONSTER){
            if (col == player.col + 1){
                isDead = true;
                map[player.row][col] = TILE_OPEN;
                map[player.row][player.col] = TILE_MONSTER;
            }
            else{
                map[player.row][col] = TILE_OPEN;
                map[player.row][col + 1] = TILE_MONSTER;
            }
        }
    }

    //left side
    for (int row = player.row - 1; row >= 0; row--){
        if (map[row][player.col] == TILE_PILLAR){
            break;
        }
        if (map[row][player.col] == TILE_MONSTER){
            if (row == player.row - 1){
                isDead = true;
                map[row][player.col] = TILE_OPEN;
                map[player.row][player.col] = TILE_MONSTER;
            }
            else{
                map[row][player.col] = TILE_OPEN;
                map[row + 1][player.col] = TILE_MONSTER;
            }
        }
    }

    //right side
    for (int row = player.row + 1; row < maxRow; row++){
        if (map[row][player.col] == TILE_PILLAR){
            break;
        }
        if (map[row][player.col] == TILE_MONSTER){
            if (row == player.row + 1){
                isDead = true;
                map[row][player.col] = TILE_OPEN;
                map[player.row][player.col] = TILE_MONSTER;
            }
            else{
                map[row][player.col] = TILE_OPEN;
                map[row - 1][player.col] = TILE_MONSTER;
            }
        }
    }
*/

    //up side
    for (int row = player.row - 1; row >= 0; row--){
        if (map[row][player.col] == TILE_PILLAR){
            break;
        }
        if (map[row][player.col] == TILE_MONSTER){
            
            if (map[row+1][player.col] == TILE_PLAYER){
                map[row+1][player.col] = TILE_MONSTER;
                map[row][player.col] = TILE_OPEN;
                isDead = true;
            }
            map[row][player.col] = TILE_OPEN;
            map[row + 1][player.col] = TILE_MONSTER;
        }
    }

    //down side
    for (int row = player.row + 1; row < maxRow; row++){
        if (map[row][player.col] == TILE_PILLAR){
            break;
        }
        if (map[row][player.col] == TILE_MONSTER){

            if (map[row-1][player.col] == TILE_PLAYER){
                map[row-1][player.col] = TILE_MONSTER;
                map[row][player.col] = TILE_OPEN;
                isDead = true;
            }
            map[row][player.col] = TILE_OPEN;
            map[row - 1][player.col] = TILE_MONSTER;
        }
    }
    
    //left side
    for (int col = player.col - 1; col >= 0; col--){
        if (map[player.row][col] == TILE_PILLAR){
            break;
        }
        if (map[player.row][col] == TILE_MONSTER){
            
            if (map[player.row][col+1] == TILE_PLAYER){
                map[player.row][col+1] = TILE_MONSTER;
                map[player.row][col] = TILE_OPEN;
                isDead = true;
            }
            map[player.row][col] = TILE_OPEN;
            map[player.row][col + 1] = TILE_MONSTER;
        }
    }

    //right side
    for (int col = player.col + 1; col < maxCol; col++){
        if (map[player.row][col] == TILE_PILLAR){
            break;
        }
        if (map[player.row][col] == TILE_MONSTER){

            if (map[player.row][col-1] == TILE_PLAYER){
                map[player.row][col-1] = TILE_MONSTER;
                map[player.row][col] = TILE_OPEN;
                isDead = true;
            }
            map[player.row][col] = TILE_OPEN;
            map[player.row][col - 1] = TILE_MONSTER;
        }
    }

    return isDead;
}
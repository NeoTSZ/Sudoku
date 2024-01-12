
#include <stdio.h> 
#include <stdlib.h>
#include <Windows.h>
#include <time.h>
#include <conio.h>

/*============================================================*/

#define SELECTOR_CODE 221

/*============================================================*/

void createBoardString(char *board); 
void fillSudoku(int *cells); 
int fillBox(int startIndex, int number, int *cells); 
void blockRowsAndColumns(int anchor, int *cells); 
void updateBoard(char *board, int *indices, int *cells); 
void moveSelector(int input, int *selector, int *indices, char *board); 
void removeSpots(int difficulty, int *cells);

/*============================================================*/

int main() {
    // Setting randomizer seed 
    srand(time(0));
 
    char board[722]; 
    createBoardString(board); 

    int indices[81] = {
         40,  44,  48,  52,  56,  60,  64,  68,  72,
        116, 120, 124, 128, 132, 136, 140, 144, 148, 
        192, 196, 200, 204, 208, 212, 216, 220, 224,
        268, 272, 276, 280, 284, 288, 292, 296, 300, 
        344, 348, 352, 356, 360, 364, 368, 372, 376,
        420, 424, 428, 432, 436, 440, 444, 448, 452,
        496, 500, 504, 508, 512, 516, 520, 524, 528,
        572, 576, 580, 584, 588, 592, 596, 600, 604, 
        648, 652, 656, 660, 664, 668, 672, 676, 680
    }; 

    // Main Sudoku cells and clone cells 
    int cells[81],
        clone[81],
        difficulty = 12; 
        // EASY: 12, MEDIUM: 24, HARD: 36
    fillSudoku(cells); 
    for (int i = 0; i < 81; i++)
        clone[i] = cells[i]; 
    removeSpots(difficulty, cells);
    updateBoard(board, indices, cells); 

    // Initializing selector 
    int selector = 40;
    board[indices[selector] - 1] = SELECTOR_CODE;

    system("@cls||clear");
    printf("%s", board);

    // Looping game 
    while (1) {
        if (_kbhit()) {
            int input = getch();
            switch (input) {
                case 97: case 100: case 115: case 119:
                    moveSelector(input, &selector, indices, board); 
                    break; 
                case 49: case 50: case 51: 
                case 52: case 53: case 54: 
                case 55: case 56: case 57: 
                    if (cells[selector] == 0 || board[indices[selector] + 1] == '?') {
                        cells[selector] = input - 48; 
                        board[indices[selector]] = (char) input; 
                        board[indices[selector] + 1] = '?'; 
                    }
                    break; 
                case 114: // Reset Sudoku
                    fillSudoku(cells); 
                    for (int i = 0; i < 81; i++) {
                        clone[i] = cells[i]; 
                        board[indices[i] + 1] = (char) 255; 
                    }
                    removeSpots(difficulty, cells);
                    updateBoard(board, indices, cells);
                    break; 
                case 99: // Check Sudoku
                    for (int i = 0; i < 81; i++) {
                        if (!cells[i])
                            board[indices[i]] = '?';
                        else if (board[indices[i] + 1] == '?') {
                            if (cells[i] == clone[i]) 
                                board[indices[i] + 1] = (char) 3; 
                            else 
                                board[indices[i] + 1] = 'X';
                        }
                    }
                    break; 
                case 101: // Exit 
                    return 0; 
            }

            system("@cls||clear");
            printf("%s", board);
        }
    }
    
    return 0;
}

/*============================================================*/

void createBoardString(char *board) {
    // ASCII code sequences 
    int codeSequence[5][11] = {
        {205, 201, 209, 209, 203, 209, 209, 203, 209, 209, 187}, // Top border 
        {205, 200, 207, 207, 202, 207, 207, 202, 207, 207, 188}, // Bottom border 
        {205, 204, 216, 216, 206, 216, 216, 206, 216, 216, 185}, // Thick horizontal borders 
        {196, 199, 197, 197, 215, 197, 197, 215, 197, 197, 182}, // Thin horizontal borders
        {255, 186, 179, 179, 186, 179, 179, 186, 179, 179, 186}  // Cell lines and vertical borders 
    }; 

    // Iterating over board layers 
    int boardIndex = 0; 
    for (int layer = 0; layer < 19; layer++) {
        int sequenceChoice;  

        if (!layer) 
            sequenceChoice = 0; 
        else if (layer == 18) 
            sequenceChoice = 1; 
        else if (layer % 6 == 0)
            sequenceChoice = 2; 
        else if (layer % 2 == 0)
            sequenceChoice = 3; 
        else 
            sequenceChoice = 4; 

        int sequenceIndex = 1; 
        for (int lineChar = 0; lineChar < 19; lineChar++) {
            if (lineChar % 2 == 0)
                board[boardIndex++] = codeSequence[sequenceChoice][sequenceIndex++];
            else {
                for (int filler = 0; filler < 3; filler++)
                    board[boardIndex++] = codeSequence[sequenceChoice][0]; 
            }
        }
        board[boardIndex++] = 10; 
    }

    board[--boardIndex] = 0; 
}

/*============================================================*/

void fillSudoku(int *cells) {
    int boxIndexPattern[9] = {0, 30, 60, 3, 27, 33, 57, 6, 54}; 

    // Attempting to fill cells until success 
    int failedToFillBox; 
    do {
        failedToFillBox = 0; 

        // Clearing cells 
        for (int i = 0; i < 81; i++)
            cells[i] = 0; 

        for (int currentNumber = 1; currentNumber < 10; currentNumber++) {
            // Resetting blocked cells 
            for (int i = 0; i < 81; i++) {
                if (cells[i] == 10)
                    cells[i] = 0; 
            }

            // Filling boxes according to pattern 
            for (int box = 0; box < 9; box++) {
                failedToFillBox = fillBox(boxIndexPattern[box], currentNumber, cells);

                if (failedToFillBox)
                    break; 
            }

            if (failedToFillBox) 
                break; 
        }
    } while (failedToFillBox);
}

/*============================================================*/

int fillBox(int startIndex, int number, int *cells) {
    int boxIndices[9] = {
        startIndex     , startIndex +  1, startIndex +  2, 
        startIndex +  9, startIndex + 10, startIndex + 11, 
        startIndex + 18, startIndex + 19, startIndex + 20
    };

    // Counting number of free spots in box 
    int freeCount = 0; 
    for (int i = 0 ; i < 9; i++) {
        if (cells[boxIndices[i]] == 0)
            freeCount++; 
    }

    // Checking fail condition 
    if (!freeCount)
        return 1; 

    // Picking a random spot from free cells 
    int randomPick = rand() % freeCount; 

    // Filling box with number 
    for (int i = 0; i < 9; i++) {
        if (cells[boxIndices[i]]) 
            continue; 
        else {
            if (randomPick) {
                randomPick--; 
                continue; 
            } else {
                cells[boxIndices[i]] = number; 
                blockRowsAndColumns(boxIndices[i], cells);
                return 0;
            }
        }
    }
}

/*============================================================*/

void blockRowsAndColumns(int anchor, int *cells) {
    int top = anchor % 9,
        bottom = top + 72, 
        left = anchor - top, 
        right = left + 8; 

    // Blocking row 
    for (int i = left; i <= right; i++) {
        if (cells[i] == 0) 
            cells[i] = 10; 
    }

    // Blocking column 
    for (int i = top; i <= bottom; i += 9) {
        if (cells[i] == 0) 
            cells[i] = 10; 
    }
}

/*============================================================*/

void updateBoard(char *board, int *indices, int *cells) {
    for (int i = 0; i < 81; i++) {
        if (cells[i])
            board[indices[i]] = (char) (cells[i] + 48);
        else 
            board[indices[i]] = (char) 255;
    }
}

/*============================================================*/

void moveSelector(int input, int *selector, int *indices, char *board) {
    board[indices[*selector] - 1] = (char) 255; 

    switch (input) {
        case 119: // Up 
            if (*selector > 8)
                *selector -= 9; 
            break; 
        case  97: // Left 
            if (*selector % 9)
                *selector -= 1; 
            break;
        case 115: // Down
            if (*selector < 72)
                *selector += 9; 
            break; 
        case 100: // Right 
            if (*selector % 9 != 8)
                *selector += 1; 
            break; 
    }

    board[indices[*selector] - 1] = (char) SELECTOR_CODE; 
}

/*============================================================*/

void removeSpots(int difficulty, int *cells) {
    while (difficulty) {
        int randomSpot = rand() % 81; 
        if (cells[randomSpot]) {
            cells[randomSpot] = 0; 
            difficulty--; 
        }
    }
}

/*============================================================*/


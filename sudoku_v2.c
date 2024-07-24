// Created by Earl James Williams Aliñgasa (NeoTSZ).

/*
The following members of the Low Level Learning Discord server also helped improve the program:
- [gerogaga] for their suggestions regarding enumeration, notation, and precedence.
- [Ema] for their suggestions of optimization.
*/

// Header File Libraries
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <time.h>

// Color Macros
#define DECOLOR "\x1b[0m"
#define RED "\x1b[91m"
#define GREEN "\x1b[92m"
#define YELLOW "\x1b[93m"
#define PINK "\x1b[95m"
#define CYAN "\x1b[96m"

// Size Macro
#define CELL_COUNT 81

// Control Macros
#define CLEAR_SCREEN "\x1b[2J\x1b[1;1H"
#define AT_HOME "\x1b[2;2H"
#define AT_CONTROLS "\x1b[2;40H"
#define AT_DIFFICULTY "\x1b[18;40H"
#define AT_EXIT "\x1b[20;1H"
#define SAVE_CURSOR "\x1b[s"
#define LOAD_CURSOR "\x1b[u"
#define HIDE_CURSOR "\x1b[?25l"
#define SHOW_CURSOR "\x1b[?25h"

// Cursor Movement Macros
#define CURSOR_UP "\x1b[1A"
#define CURSOR_DOWN "\x1b[1B"
#define CURSOR_RIGHT "\x1b[1C"
#define CURSOR_LEFT "\x1b[1D"

// Line Setup Macros
#define LINKER(X,Y) #X #X #X #Y #X #X #X #Y #X #X #X
#define LINE_SETTER(A,B,C,D,E) #A LINKER(B, C) #D LINKER(B, C) #D LINKER(B, C) #E "\n"

// Grid Line Macros
#define TOP_BORDER LINE_SETTER(\xC9,\xCD,\xD1,\xCB,\xBB)
#define BOTTOM_BORDER LINE_SETTER(\xC8,\xCD,\xCF,\xCA,\xBC)
#define THICK_DIVIDER LINE_SETTER(\xCC,\xCD,\xD8,\xCE,\xB9)
#define THIN_DIVIDER LINE_SETTER(\xC7,\xC4,\xC5,\xD7,\xB6)
#define CELL_LINE LINE_SETTER(\xBA,\xFF,\xB3,\xBA,\xBA)

// Printing Macros
#define CELL_GROUP CELL_LINE THIN_DIVIDER CELL_LINE THIN_DIVIDER CELL_LINE
#define SUDOKU_GRID TOP_BORDER CELL_GROUP THICK_DIVIDER CELL_GROUP THICK_DIVIDER CELL_GROUP BOTTOM_BORDER
#define SELECT SAVE_CURSOR PINK "\xdd" CURSOR_RIGHT "\xde" DECOLOR LOAD_CURSOR
#define DESELECT SAVE_CURSOR "\xff" CURSOR_RIGHT "\xff" LOAD_CURSOR
#define BUTTON(X,Y) SAVE_CURSOR CYAN #X "\x1b[3C" DECOLOR #Y LOAD_CURSOR CURSOR_DOWN

// Difficulty Counts
enum difficulties
{
    TEST =  3,
    EASY = 12,
    MEDIUM = 24,
    HARD = 36
};

// User Movement Inputs
enum userInputs
{
    INPUT_UP = 5,
    INPUT_DOWN = 2,
    INPUT_RIGHT = 3,
    INPUT_LEFT = 1
};

// Game-State Structure
typedef struct Game
{
    int cells[CELL_COUNT];
    int marker[CELL_COUNT];
    int checker[CELL_COUNT];
    int selector;
    int difficulty;
}
Game;

// Prototypes
void pullBlockers(int* cells);
void putBlockers(int index, int* cells);
int putBoxDigit(int boxIndex, int digit, int* cells);
int populateCells(int* cells);
void applyDifficulty(int count, int* cells, int* marker);
void initializeGame(Game* game);
void printDigits(int* cells);
void printButtons();
void enterDigit(int digit, int selector, int* marker, int* cells);
void moveSelector(int move, int* selector);
void resetSudoku(Game* game);
void checkSudoku(Game* game);
void processInput(char input, Game* game);

int main(void)
{
    Game game;

    // Setting up the game.
    game.difficulty = EASY;
    srand(time(0));
    initializeGame(&game);

    // Printing the game.
    printf(CLEAR_SCREEN HIDE_CURSOR SUDOKU_GRID);
    printButtons();
    printDigits(game.cells);
    printf(AT_HOME SELECT HIDE_CURSOR);

    // Looping the game
    while (1)
    {
        if (_kbhit())
            processInput(getch(), &game);
    }

    return 0;
}

// Removes the blockers placed by putBlockers().
void pullBlockers(int* cells)
{
    // The blockers are represented by 10.
    // They get reset to 0.
    for (int i = 0; i < CELL_COUNT; i ++)
        cells[i] -= (cells[i] == 10) ? 10 : 0;

    return;
}

// Blocks the empty cells in the same row and column as a given index.
void putBlockers(int index, int* cells)
{
    // Getting position indices relatives to the index.
    int topEnd = index % 9;
    int leftEnd = index - topEnd;
    int bottomEnd = topEnd + 72;
    int rightEnd = leftEnd + 8;

    // Blocking the empty cells in the same row.
    for (int i = leftEnd; i <= rightEnd; i ++)
        cells[i] += (cells[i] == 0) ? 10 : 0;

    // Blocking the empty cells in the same column.
    for (int i = topEnd; i <= bottomEnd; i += 9)
        cells[i] += (cells[i] == 0) ? 10 : 0;

    return;
}

// Puts a given digit in a given box.
int putBoxDigit(int boxIndex, int digit, int* cells)
{
    int emptyCount = 0;

    // Counting the number of empty cells in the box.
    for (int i = boxIndex; i < boxIndex + 21;)
    {
        emptyCount += (cells[i] == 0) ? 1 : 0;
        i += (i == boxIndex + 2 || i == boxIndex + 11) ? 7 : 1;
    }

    // Returning an error if there are no empty cells.
    if (!emptyCount)
        return 1;

    int randomCount = rand() % emptyCount;

    // Using randomCount to choose an empty cell.
    for (int i = boxIndex; i < boxIndex + 21;)
    {
        // Checking if the empty cell is met.
        if (!cells[i] && !randomCount)
        {
            cells[i] = digit;
            putBlockers(i, cells);

            return 0;
        }
        else if (!cells[i] && randomCount)
            randomCount --;

        i += (i == boxIndex + 2 || i == boxIndex + 11) ? 7 : 1;
    }

    return 0;
}

// Randomly populates grid cells with digits.
int populateCells(int* cells)
{
    // The boxes are chosen in this order to ensure good chances of populating the cells.
    int boxIndices[9] = {0, 30, 60, 3, 33, 6, 27, 57, 54};

    // Resetting all the cells.
    for (int i = 0; i < CELL_COUNT; i ++)
        cells[i] = 0;

    // Filling the digits in sequence.
    for (int i = 1; i < 10; i ++)
    {
        for (int j = 0; j < 9; j ++)
        {
            if (putBoxDigit(boxIndices[j], i, cells))
                return 1;
        }

        pullBlockers(cells);
    }

    return 0;
}

// Randomly removes cells according to difficulty count.
void applyDifficulty(int count, int* cells, int* marker)
{
    while (count)
    {
        int randomIndex = rand() % 81;

        // Making sure the randomly chosen spot is non-empty.
        while (!cells[randomIndex])
            randomIndex = rand() % 81;

        cells[randomIndex] = 0;
        marker[randomIndex] = 0;
        count --;
    }

    return;
}

// Initializes the game structure.
void initializeGame(Game* game)
{
    game->selector = 0;

    // Repeatedly attempting to populate the cells until success is achieved.
    while (populateCells(game->cells));

    // Copying the content of cells[] over to marker[] and checker[].
    for (int i = 0; i < CELL_COUNT; i ++)
    {
        game->marker[i] = game->cells[i];
        game->checker[i] = game->cells[i];
    }

    applyDifficulty(game->difficulty, game->cells, game->marker);

    return;
}

// Prints the cell values onto the grid.
void printDigits(int* cells)
{
    printf(AT_HOME SAVE_CURSOR);

    for (int i = 0; i < CELL_COUNT; i ++)
    {
        // Printing non-zero cell values.
        if (!cells[i])
            printf(CURSOR_RIGHT "\xff");
        else
            printf(CURSOR_RIGHT "%d", cells[i]);

        if (i % 9 == 8)
            printf(LOAD_CURSOR CURSOR_DOWN CURSOR_DOWN SAVE_CURSOR);
        else
            printf(CURSOR_RIGHT CURSOR_RIGHT);
    }
}

// Prints the controls of the game onto the right side of the grid.
void printButtons()
{
    // Printing the number controls.
    printf(AT_CONTROLS SAVE_CURSOR);
    printf(BUTTON(1-9\b\b,Enter Digit) BUTTON(0,Remove Digit));

    // Printing the movement controls.
    printf(CURSOR_DOWN BUTTON(W,Move Up) BUTTON(S,Move Down) BUTTON(D,Move Right) BUTTON(A,Move Left));

    // Printing current Sudoku options.
    printf(CURSOR_DOWN BUTTON(R,Reset Sudoku) BUTTON(C,Check Sudoku));

    // Printing new Sudoku options.
    printf(CURSOR_DOWN BUTTON(N,New Sudoku) BUTTON(X,Change Difficulty));

    // Printing end option and difficulty.
    printf(CURSOR_DOWN BUTTON(E,Exit Game));
    printf(CURSOR_DOWN "Difficulty: " YELLOW "EASY" DECOLOR);

    return;
}

// Prints a digit onto the grid if the cell being selected is empty.
void enterDigit(int digit, int selector, int* marker, int* cells)
{
    // Checking if the cell is nonempty.
    if (marker[selector])
        return;

    // Proceeding if the cell is empty.
    cells[selector] = digit;

    if (digit == 0)
        printf(SAVE_CURSOR CURSOR_RIGHT "\xff" LOAD_CURSOR);
    else
        printf(SAVE_CURSOR CURSOR_RIGHT PINK "%d" DECOLOR LOAD_CURSOR, digit);

    return;
}

// Moves the cell selector according to user input.
void moveSelector(int move, int* selector)
{
    printf(DESELECT);

    // Processing move and adjusting the selector accordingly.
    switch (move)
    {
    case INPUT_UP:
        if (*selector > 8)
        {
            *selector -= 9;
            printf(CURSOR_UP CURSOR_UP);
        }
        break;

    case INPUT_DOWN:
        if (*selector < 72)
        {
            *selector += 9;
            printf(CURSOR_DOWN CURSOR_DOWN);
        }
        break;

    case INPUT_RIGHT:
        if (*selector % 9 != 8)
        {
            ++ *selector;
            printf(CURSOR_RIGHT CURSOR_RIGHT CURSOR_RIGHT CURSOR_RIGHT);
        }
        break;

    case INPUT_LEFT:
        if (*selector % 9 != 0)
        {
            -- *selector;
            printf(CURSOR_LEFT CURSOR_LEFT CURSOR_LEFT CURSOR_LEFT);
        }
        break;
    }

    printf(SELECT);

    return;
}

// Resets all the user-inputted digits.
void resetSudoku(Game* game)
{
    // Resetting the selector.
    game->selector = 0;
    printf(DESELECT AT_HOME SAVE_CURSOR);

    // Removing all the user-inputted digits.
    for (int i = 0; i < CELL_COUNT; i ++)
    {
        if (game->marker[i] == 0)
        {
            game->cells[i] = 0;
            printf(CURSOR_RIGHT "\xff");
        }
        else
            printf(CURSOR_RIGHT CURSOR_RIGHT);

        if (i % 9 == 8)
            printf(LOAD_CURSOR CURSOR_DOWN CURSOR_DOWN SAVE_CURSOR);
        else
            printf(CURSOR_RIGHT CURSOR_RIGHT);
    }

    printf(AT_HOME SELECT SAVE_CURSOR);

    return;
}

// Checks all the user-inputted digits.
void checkSudoku(Game* game)
{
    // Resetting the selector.
    game->selector = 0;
    printf(DESELECT AT_HOME SAVE_CURSOR);

    // Processing all the user-inputted digits.
    for (int i = 0; i < CELL_COUNT; i ++)
    {
        if (game->marker[i] == 0)
        {
            game->marker[i] = 10;

            if (!game->cells[i])
                printf(YELLOW CURSOR_RIGHT "%d" DECOLOR, game->checker[i]);
            else if (game->cells[i] == game->checker[i])
                printf(GREEN CURSOR_RIGHT "%d" DECOLOR, game->cells[i]);
            else
                printf(RED CURSOR_RIGHT "%d" DECOLOR, game->cells[i]);
        }
        else
            printf(CURSOR_RIGHT CURSOR_RIGHT);

        if (i % 9 == 8)
            printf(LOAD_CURSOR CURSOR_DOWN CURSOR_DOWN SAVE_CURSOR);
        else
            printf(CURSOR_RIGHT CURSOR_RIGHT);
    }

    printf(AT_HOME SELECT SAVE_CURSOR);

    return;
}

// Processes all user inputs during the game loop.
void processInput(char input, Game* game)
{
    switch (input)
    {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
        enterDigit((int)(input - 48), game->selector, game->marker, game->cells);
        break;

    case 'w':
    case 'W':
        moveSelector(INPUT_UP, &game->selector);
        break;

    case 's':
    case 'S':
        moveSelector(INPUT_DOWN, &game->selector);
        break;

    case 'd':
    case 'D':
        moveSelector(INPUT_RIGHT, &game->selector);
        break;

    case 'a':
    case 'A':
        moveSelector(INPUT_LEFT, &game->selector);
        break;

    case 'r':
    case 'R':
        resetSudoku(game);
        break;

    case 'c':
    case 'C':
        checkSudoku(game);
        break;

    case 'x':
    case 'X':
        printf(DESELECT AT_DIFFICULTY "Difficulty: " YELLOW );
        if (game->difficulty == EASY)
        {
            game->difficulty = MEDIUM;
            printf("MEDIUM" DECOLOR);
        }
        else if (game->difficulty == MEDIUM)
        {
            game->difficulty = HARD;
            printf("HARD\xff\xff" DECOLOR);
        }
        else
        {
            game->difficulty = EASY;
            printf("EASY\xff\xff" DECOLOR);
        }

    case 'n':
    case 'N':
        printf(DESELECT AT_HOME SAVE_CURSOR);
        initializeGame(game);
        printDigits(game->cells);
        printf(AT_HOME SELECT SAVE_CURSOR);
        break;

    case 'e':
    case 'E':
        printf(DESELECT AT_EXIT SHOW_CURSOR);
        exit(0);
    }

    return;
}

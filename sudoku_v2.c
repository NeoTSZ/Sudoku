// Created by Earl James Williams Aliñgasa (NeoTSZ on GitHub).
// Last updated on July 19, 2024.

// Header file libraries
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <time.h>

// Color macros
#define DECOLOR "\x1b[0m"
#define RED "\x1b[91m"
#define GREEN "\x1b[92m"
#define YELLOW "\x1b[93m"
#define PINK "\x1b[95m"
#define CYAN "\x1b[96m"

// Size macros
#define CELL_COUNT 81
#define GRID_WIDTH 9

// Control macros
#define CLEAR "\x1b[2J\x1b[H"
#define HOME "\x1b[2;2H"
#define SAVE "\x1b[s"
#define LOAD "\x1b[u"
#define HIDE "\x1b[?25l"
#define SHOW "\x1b[?25h"

// Movement macros
#define UP "\x1b[1A"
#define DOWN "\x1b[1B"
#define RIGHT "\x1b[1C"
#define LEFT "\x1b[1D"

// Character codes
// C9[╔] CD[═] D1[╤] CB[╦] BB[╗] BA[║]
// B3[│] C7[╟] C4[─] C5[┼] D7[╫] B6[╢]
// CC[╠] D8[╪] CE[╬] B9[╣] C8[╚] CF[╧]
// CA[╩] BC[╝]

// Setup macros
#define LINK(X, Y) #X #X #X #Y #X #X #X #Y #X #X #X
#define LINE(A, B, C, D, E) #A LINK(B, C) #D LINK(B, C) #D LINK(B, C) #E "\n"

// Line macros
#define TOP LINE(\xC9,\xCD,\xD1,\xCB,\xBB)
#define BOTTOM LINE(\xC8,\xCD,\xCF,\xCA,\xBC)
#define THICK LINE(\xCC,\xCD,\xD8,\xCE,\xB9)
#define CELLS LINE(\xBA,\xFF,\xB3,\xBA,\xBA)
#define THIN LINE(\xC7,\xC4,\xC5,\xD7,\xB6)

// Printing macros
#define GROUP CELLS THIN CELLS THIN CELLS
#define GRID TOP GROUP THICK GROUP THICK GROUP BOTTOM
#define SELECT SAVE PINK "{" RIGHT "}" LOAD DECOLOR
#define DESELECT SAVE "\xff" RIGHT "\xff" LOAD
#define BUTTON(X, Y) SAVE CYAN #X "\xff" DECOLOR #Y LOAD DOWN

enum difficulties
{
    TEST = 3,
    EASY = 12,
    MEDIUM = 24,
    HARD = 36
};

// Structure to hold the state of the game
typedef struct Game
{
    int cells[CELL_COUNT];
    int copy[CELL_COUNT];
    int clone[CELL_COUNT];
    int cursor;
    int difficulty;
} Game;

// Prototypes
void removeBlockers(Game *game);
void blockLines(int index, Game *game);
int fillBox(int box, int digit, Game *game);
int fillCells(Game *game);
void applyDifficulty(Game *game);
void initializeGame(Game *game);
void printDigits(Game game);
void drawGrid(Game game);
void putDigit(int digit, Game *game);
void pullDigit(Game* game);
void processInput(char input, Game *game);
void moveCursor(int move, Game *game);
void checkSolution(Game* game);
void resetGame(Game* game);
void changeDifficulty(Game* game);

int main(void)
{
    Game game;

    // Setting up the game
    game.difficulty = EASY;
    srand(time(0));
    initializeGame(&game);
    drawGrid(game);

    // Looping the game
    while (1)
    {
        if (_kbhit())
            processInput(getch(), &game);
    }

    return 0;
}

// Function to remove all the blockers placed by blockLines()
void removeBlockers(Game *game)
{
    // The blockers are represented by 10
    // They get reset to 0

    for (int i = 0; i < CELL_COUNT; i++)
        *(game->cells + i) -= (*(game->cells + i) == 10) ? 10 : 0;

    return;
}

// Function to block the row and column of a certain index
void blockLines(int index, Game *game)
{
    // Getting position indices relatives to the index
    int top = index % 9;
    int left = index - top;
    int bottom = top + 72;
    int right = left + 8;

    // Blocking the empty slots in the same row
    for (int i = left; i <= right; i++)
        *(game->cells + i) += (*(game->cells + i) == 0) ? 10 : 0;

    // Blocking the empty slots in the same column
    for (int i = top; i <= bottom; i += 9)
        *(game->cells + i) += (*(game->cells + i) == 0) ? 10 : 0;

    return;
}

// Function to fill a box with a certain number
int fillBox(int box, int digit, Game *game)
{
    int emptyCount = 0;

    // Counting the number of empty spaces within a box
    for (int i = box; i < box + 21;)
    {
        emptyCount += (*(game->cells + i) == 0) ? 1 : 0;

        i += (i == box + 2 || i == box + 11) ? 7 : 1;
    }

    // Flagging an error if there are no empty slots in the box
    if (!emptyCount)
        return 1;

    int randomCount = rand() % emptyCount;

    // Using randomCount to choose a spot for the digit
    for (int i = box; i < box + 21;)
    {
        if (*(game->cells + i) == 0)
        {
            // Checking if the chosen spot is reached
            if (randomCount == 0)
            {
                *(game->cells + i) = digit;
                blockLines(i, game);
                return 0;
            }
            else
                randomCount--;
        }

        i += (i == box + 2 || i == box + 11) ? 7 : 1;
    }
}

// Function to distribute a digit in the grid randomly
int fillCells(Game *game)
{
    // Filling the cells with zeros
    for (int i = 0; i < CELL_COUNT; i++)
        *(game->cells + i) = 0;

    for (int i = 1; i < 10; i++)
    {
        int errorFlag = 0;

        // Filling the boxes diagonally
        errorFlag += fillBox(0, i, game);
        errorFlag += fillBox(30, i, game);
        errorFlag += fillBox(60, i, game);

        // Filling the boxes in the top-right corner
        errorFlag += fillBox(3, i, game);
        errorFlag += fillBox(33, i, game);
        errorFlag += fillBox(6, i, game);

        // Filling the boxes in the bottom-left corner
        errorFlag += fillBox(27, i, game);
        errorFlag += fillBox(57, i, game);
        errorFlag += fillBox(54, i, game);

        if (errorFlag)
            return 1;
        else
            removeBlockers(game);
    }

    return 0;
}

// Function to empty out random cells
void applyDifficulty(Game *game)
{
    int count = game->difficulty;

    // Removing as many cells as indicated by count
    while (count)
    {
        int randomIndex = rand() % 81;

        // Making sure the randomly chosen spot is not already empty
        while (*(game->cells + randomIndex) == 0)
            randomIndex = rand() % 81;

        *(game->cells + randomIndex) = 0;
        *(game->copy + randomIndex) = 0;
        count--;
    }

    return;
}

// Function to initialize the game structure
void initializeGame(Game *game)
{
    game->cursor = 0;

    while (fillCells(game));

    // Copying the grid into clone[] and copy[]
    for (int i = 0; i < CELL_COUNT; i++)
    {
        *(game->clone + i) = *(game->cells + i);
        *(game->copy + i) = *(game->cells + i);
    }

    applyDifficulty(game);

    return;
}

// Function to place numbers within the grid
void printDigits(Game game)
{
    printf(HOME SAVE);

    for (int i = 0; i < CELL_COUNT; i++)
    {
        int temp = *(game.cells + i);

        printf(RIGHT);
        if (!temp)
            printf("\xff");
        else
            printf("%d", temp);
        printf(RIGHT RIGHT);

        // Moving to the next line
        if (i % 9 == 8)
            printf(LOAD DOWN DOWN SAVE);
    }

    return;
}

// Function to draw the grid and controls
void drawGrid(Game game)
{
    // Printing the grid
    printf(CLEAR GRID);

    printDigits(game);

    // Printing the controls
    printf("\x1b[2;39H" BUTTON([1-9], Enter Number) BUTTON([ 0 ],Remove Number));
    printf(DOWN SAVE BUTTON([ W ], Move Up) BUTTON([ S ], Move Down) BUTTON([ D ], Move Right) BUTTON([ A ], Move Left) );
    printf(DOWN SAVE BUTTON([ R ],Reset Sudoku) BUTTON([ C ],Check Solution));
    printf(DOWN SAVE BUTTON([ N ],New Sudoku) BUTTON([ X ], Change Difficulty));
    printf(DOWN SAVE BUTTON([ E ], Exit Sudoku));

    // Printing the difficulty and resetting the cursor
    printf("\x1b[18;39H" "Difficulty: " YELLOW "EASY");
    printf(DECOLOR HOME SELECT HIDE);

    return;
}

// Function to put a number onto the grid
void putDigit(int digit, Game *game)
{
    int index = game->cursor;

    if (*(game->copy + index) != 0)
        return;

    *(game->cells + index) = digit;
    printf(SAVE PINK RIGHT "%d" LOAD DECOLOR, digit);

    return;
}

// Function to pull a number from the grid
void pullDigit(Game* game)
{
    if (*(game->copy + game->cursor) == 0)
    {
        *(game->cells + game->cursor) = 0;
        printf(SAVE RIGHT "\xff" LOAD);
    }

    return;
}

// Function to process continuous keyboard inputs via <conio.h>
void processInput(char input, Game *game)
{
    switch (input)
    {
    case 'w': // Move the cursor up
    case 'W':
        moveCursor(5, game);
        break;

    case 's': // Move the cursor down
    case 'S':
        moveCursor(2, game);
        break;

    case 'd': // Move the cursor right
    case 'D':
        moveCursor(3, game);
        break;

    case 'a': // Move the cursor left
    case 'A':
        moveCursor(1, game);
        break;

    case 'e': // Exit the game
    case 'E':
        printf(SHOW "\x1b[20;1H");
        exit(0);

    // Before passing by value, the character is casted to an integer according to its ASCII code
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
        putDigit((int)(input - 48), game);
        break;

    case 'c': // Checking the entered solution
    case 'C':
        checkSolution(game);
        break;

    case 'x': // Changing the difficulty
    case 'X':
        changeDifficulty(game);

    case 'n': // Starting a new game
    case 'N':
        printf(DESELECT);
        initializeGame(game);
        printDigits(*game);
        printf(HOME SELECT);
        break;

    case 'r': // Resetting the game
    case 'R':
        resetGame(game);
        break;

    case '0': // Removing a number from a cell
        pullDigit(game);
        break;
    }

    return;
}

// Function to move the cursor around the grid
void moveCursor(int move, Game *game)
{
    int temp = game->cursor;

    printf(DESELECT);
    switch (move)
    {
    case 5: // Move the cursor up
        if (temp > 8)
        {
            temp -= 9;
            printf(UP UP);
        }
        break;

    case 2: // Move the cursor down
        if (temp < 72)
        {
            temp += 9;
            printf(DOWN DOWN);
        }
        break;

    case 3: // Move the cursor right
        if ((temp % 9) != 8)
        {
            temp++;
            printf(RIGHT RIGHT RIGHT RIGHT);
        }
        break;

    case 1: // Move the cursor left
        if ((temp % 9) != 0)
        {
            temp--;
            printf(LEFT LEFT LEFT LEFT);
        }
        break;
    }

    printf(SELECT);
    game->cursor = temp;

    return;
}

// Function to check solutions entered by the user
void checkSolution(Game* game)
{
    printf(DESELECT HOME SAVE);

    // Printing the numbers in the cells
    for (int i = 0; i < CELL_COUNT; i++)
    {
        int temp = *(game->cells + i);

        printf(RIGHT);
        if (!temp)
            printf(YELLOW "%d", *(game->clone + i));
        else if (temp == *(game->clone + i) && *(game->copy + i) == 0)
            printf(GREEN "%d", temp);
        else if (temp != *(game->clone + i) && *(game->copy + i) == 0)
            printf(RED "%d", temp);
        else
            printf(RIGHT);
        printf(DECOLOR RIGHT RIGHT);

        *(game->copy + i) = 10;

        // Moving to the next line
        if (i % 9 == 8)
            printf(LOAD DOWN DOWN SAVE);
    }

    game->cursor = 0;
    printf(HOME SELECT);

    return;
}

// Function to empty user-played cells
void resetGame(Game* game)
{
    printf(DESELECT HOME SAVE);

    // Printing the numbers in the cells
    for (int i = 0; i < CELL_COUNT; i++)
    {
        int temp = *(game->copy + i);

        printf(RIGHT);
        if (temp == 10)
        {
            game->cursor = 0;
            printf(HOME SELECT);
            return;
        }
        else if (!temp)
        {
            printf("\xff");
            *(game->cells + i) = 0;
        }
        else
            printf(RIGHT);
        printf(DECOLOR RIGHT RIGHT);

        // Moving to the next line
        if (i % 9 == 8)
            printf(LOAD DOWN DOWN SAVE);
    }

    game->cursor = 0;
    printf(HOME SELECT);

    return;
}

// Function to adjust difficulty and start a new game
void changeDifficulty(Game* game)
{
    // Printing the difficulty
    printf(SAVE "\x1b[18;39H" "Difficulty: " YELLOW);

    // Switching to the appropriate enumeration variant in the cycle
    if (game->difficulty == EASY)
    {
        game->difficulty = MEDIUM;
        printf("MEDIUM");
    }
    else if (game->difficulty == MEDIUM)
    {
        game->difficulty = HARD;
        printf("HARD\xff\xff");
    }
    else if (game->difficulty == HARD)
    {
        game->difficulty = TEST;
        printf("TEST\xff\xff");
    }
    else
    {
        game->difficulty = EASY;
        printf("EASY\xff\xff");
    }

    printf(DECOLOR LOAD);

    return;
}

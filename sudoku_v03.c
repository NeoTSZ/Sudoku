
// Earl James Williams Aliñgasa

/*------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <conio.h>

/*------------------------------------------------------------*/

#define EMPTY_SPACE_COUNT 6

/*------------------------------------------------------------*/

void renderGrid(int empty[], int cells[], int selector);
void fillCells(int cells[]);
void moveSelector(int move, int *selector);
void fillBox(int number, int boxIndex, int cells[]);
int giveEmptyCount(int cells[], int boxIndex);
void inputNumber(int number, int cells[], int empty[], int selector);
void reset(int cells[], int empty[]);

/*------------------------------------------------------------*/

int failedAttempt = 0;
int difficulty = 12;
// EASY: 12, MEDIUM: 30, HARD: 48

/*------------------------------------------------------------*/

int main() {
  int cells[81],
  clone[81],
  empty[81],
  selector = 0,
  quit = 0,
  tempRandomPicker,
  result = 1;

  // Setting randomizer seed
  srand(time(0));

  // Checking fail case
  do {
    failedAttempt = 0;
    fillCells(cells);
  } while (failedAttempt);

  // Creating grid clone and empty clone
  for (int i = 0; i < 81; i++)
    clone[i] = cells[i];

  for (int i = 0; i < difficulty; i++) {
    tempRandomPicker = rand() % 81;
    if (cells[tempRandomPicker]) cells[tempRandomPicker] = 0;
    else i--;
  }

  // Generating empty copy
  for (int i = 0; i < 81; i++) {
    if (cells[i]) empty[i] = 0;
    else empty[i] = 1;
  }

  printf("\33[?25l");
  renderGrid(empty, cells, selector);

  while (!quit) {
    // Checking for input
    waitForInput:
    printf("\33[?25l");
    if (_kbhit()) {
      switch (getch()) {
        case 'a': // Left
          moveSelector(1, &selector);
          break;
        case 's': // Down
          moveSelector(2, &selector);
          break;
        case 'd': // Right
          moveSelector(3, &selector);
          break;
        case 'w': // Up
          moveSelector(4, &selector);
          break;
        case '1':
          inputNumber(1, cells, empty, selector);
          break;
        case '2':
          inputNumber(2, cells, empty, selector);
          break;
        case '3':
          inputNumber(3, cells, empty, selector);
          break;
        case '4':
          inputNumber(4, cells, empty, selector);
          break;
        case '5':
          inputNumber(5, cells, empty, selector);
          break;
        case '6':
          inputNumber(6, cells, empty, selector);
          break;
        case '7':
          inputNumber(7, cells, empty, selector);
          break;
        case '8':
          inputNumber(8, cells, empty, selector);
          break;
        case '9':
          inputNumber(9, cells, empty, selector);
          break;
        case 't':
          if (difficulty == 12) difficulty = 30;
          else if (difficulty == 30) difficulty = 48;
          else difficulty = 12;
        case 'n': // New Sudoku
          do {
            failedAttempt = 0;
            fillCells(cells);
          } while (failedAttempt);
          for (int i = 0; i < 81; i++)
            clone[i] = cells[i];
          for (int i = 0; i < difficulty; i++) {
            tempRandomPicker = rand() % 81;
            if (cells[tempRandomPicker]) cells[tempRandomPicker] = 0;
            else i--;
          }
          for (int i = 0; i < 81; i++) {
            if (cells[i]) empty[i] = 0;
            else empty[i] = 1;
          }
          break;
        case 'c': // Checking Sudoku
          result = 1;
          for (int i = 0; i < 81; i++) {
            if (cells[i] != clone[i]) {
              result = 0;
              break;
            }
          }
          renderGrid(empty, cells, selector);
          if (!result) printf("\n\n\tYour Sudoku solution is not quite right.\n\tTry again or load a new one.");
          else printf("\n\n\tYour Sudoku solution is correct! Good work!\n\tFeel free to load a new one if you'd like.");
          goto waitForInput;
          break;
        case 'r': // Reset Sudoku
          reset(cells, empty);
          break;
        case 'e': // Exit program
          quit = 1;
          break;
      }

      printf("\33[?25l");
      renderGrid(empty, cells, selector);
    }
  }

  return 0;
}

/*------------------------------------------------------------*/

void renderGrid(int empty[], int cells[], int selector) {
  int tracker = 0;

  // Clearing terminal
  system("@cls||clear");

  // Printing first line
  printf("\n\n\n\t%c", 201);
  for (int i = 0; i < 5; i++) {
    if (i == 1 || i == 3) printf("%c", 203);
    else {
      for (int j = 0; j < 23; j++) {
        if (j == 7 || j == 15) printf("%c", 209);
        else printf("%c", 205);
      }
    }
  }
  printf("%c\n", 187);

  // Printing middle lines
  for (int i = 0; i < 5; i++) {
    // Box boundaries
    if (i == 1 || i == 3) {
      printf("\t%c", 204);
      for (int j = 0; j < 5; j++) {
        if (j == 1 || j == 3) printf("%c", 206);
        else {
          for (int k = 0; k < 23; k++) {
            if (k == 7 || k == 15) printf("%c", 216);
            else printf("%c", 205);
          }
        }
      }
      printf("%c\n", 185);
    }
    // Within boxes
    else {
      for (int j = 0; j < 5; j++) {
        // Inner boundaries
        if (j == 1 || j == 3) {
          printf("\t%c", 199);

          for (int k = 0; k < 5; k++) {
            if (k == 1 || k == 3) printf("%c", 215);
            else {
              for (int l = 0; l < 23; l++) {
                if (l == 7 || l == 15) printf("%c", 197);
                else printf("%c", 196);
              }
            }
          }

          printf("%c\n", 182);
        }
        // Cell lines
        else {
          for (int k = 0; k < 3; k++) {
            printf("\t%c", 186);

            // Main cell line
            if (k == 1) {
              for (int l = 0; l < 5; l++) {
                if (l == 1 || l == 3) printf("%c", 186);
                else {
                  for (int m = 0; m < 5; m++) {
                    if (m == 1 || m == 3) printf("%c", 179);
                    else {
                      // Left of selector
                      if (selector == tracker) printf("%c", 16);
                      else printf("%c", 255);

                      // Empty indicator
                      if (!cells[tracker]) printf("%c%c", 255, 255);
                      else if (empty[tracker] && cells[tracker]) printf("(%c", 255);
                      else printf("%c%c", 255, 255);

                      // Cell digit
                      if (cells[tracker]) printf("%d", cells[tracker]);
                      else printf("%c", 255);


                      // Empty indicator
                      if (!cells[tracker]) printf("%c%c", 255, 255);
                      else if (empty[tracker] && cells[tracker]) printf("%c)", 255);
                      else printf("%c%c", 255, 255);

                      // Right of selector
                      if (selector == tracker) printf("%c", 17);
                      else printf("%c", 255);

                      tracker++;
                    }
                  }
                }
              }
            }
            // Empty space lines
            else {
              for (int l = 0; l < 5; l++) {
                if (l == 1 || l == 3) printf("%c", 186);
                else {
                  for (int m = 0; m < 23; m++) {
                    if (m == 7 || m == 15) printf("%c", 179);
                    else printf("%c", 255);
                  }
                }
              }
            }

            printf("%c\n", 186);
          }
        }
      }
    }
  }

  // Printing last line
  printf("\t%c", 200);
  for (int i = 0; i < 5; i++) {
    if (i == 1 || i == 3) printf("%c", 202);
    else {
      for (int j = 0; j < 23; j++) {
        if (j == 7 || j == 15) printf("%c", 207);
        else printf("%c", 205);
      }
    }
  }
  printf("%c\n\n\t(t) Set Difficulty\t\t\t\tControls: \n\t(n) New Sudoku\t\t\t\t\tw: UP\n\t(c) Check Sudoku\t\t\t\ta: LEFT\n\t(r) Reset Sudoku\t\t\t\ts: DOWN\n\t(e) Exit Program\t\t\t\td: RIGHT", 188);
  printf("\n\n\tCurrent difficulty: ");
  if (difficulty == 12) printf("EASY");
  else if (difficulty == 30) printf("MEDIUM");
  else printf("HARD");

  return;
}

/*------------------------------------------------------------*/

void fillCells(int cells[]) {
  for (int i = 0; i < 81; i++)
    cells[i] = 0;

  // Iterating through digits
  for (int z = 1; z < 10; z++) {
    // Filling diagonals first
    fillBox(z,  0, cells);
    fillBox(z, 30, cells);
    fillBox(z, 60, cells);

    // Filling edges
    fillBox(z,  3, cells);
    fillBox(z, 27, cells);
    fillBox(z, 33, cells);
    fillBox(z, 57, cells);

    // Filling final corners
    fillBox(z,  6, cells);
    fillBox(z, 54, cells);

    // Removing blockers
    for (int i = 0; i < 81; i++)
      if (cells[i] == 99) cells[i] = 0;

    if (failedAttempt) break;
  }

  return;
}

/*------------------------------------------------------------*/

void moveSelector(int move, int *selector) {
  switch (move) {
    case 1: // Left
      if (*selector % 9 == 0) *selector += 8;
      else *selector -= 1;
      break;
    case 2: // Down
      if (*selector > 71) *selector -= 72;
      else *selector += 9;
      break;
    case 3: // Right
      if (*selector % 9 == 8) *selector -= 8;
      else *selector += 1;
      break;
    case 4: // Up
      if (*selector < 9) *selector += 72;
      else *selector -= 9;
      break;
  }

  return;
}

/*------------------------------------------------------------*/

void fillBox(int number, int boxIndex, int cells[]) {
  int emptyCount,
  tempRandom,
  tempIndex,
  blocker;

  emptyCount = giveEmptyCount(cells, boxIndex);

  // Checking the fail condition for when there is no empty spaces
  if (emptyCount == 0) {
    failedAttempt = 1;
    return;
  }

  // Randomly picking an empty space
  tempRandom = rand() % emptyCount;

  // Finding index that matches empty space
  for (int i = boxIndex; i < boxIndex + 21; i++) {
    if (i == (boxIndex + 3) || i == (boxIndex + 12)) i += 6;

    if (tempRandom && !cells[i]) tempRandom--;
    else if (!tempRandom && !cells[i]) {
      tempIndex = i;
      break;
    }
  }

  // Placing blocker at start of row
  blocker = tempIndex;
  while (blocker % 9 != 0)
		blocker--;

  // Blocking rows
  for (int i = blocker; i < blocker + 9; i++)
		if (cells[i] == 0) cells[i] = 99;

  // Placing blocker at start of column
  blocker = tempIndex;
	while (blocker > 8)
		blocker -= 9;

  // Blocking columns
  for (int i = blocker; i < 81; i += 9)
		if (cells[i] == 0) cells[i] = 99;

  // Blocking box
  for (int i = boxIndex; i < boxIndex + 21; i++) {
		if (i == (boxIndex + 3) || i == (boxIndex + 12)) i += 6;

		if (cells[i] == 0) cells[i] = 99;
	}

  // Filling cell with number
  cells[tempIndex] = number;

  return;
}

/*------------------------------------------------------------*/

int giveEmptyCount(int cells[], int boxIndex) {
  int emptyCount = 0;

  for (int i = boxIndex; i < boxIndex + 21; i++) {
    if (i == (boxIndex + 3) || i == (boxIndex + 12)) i += 6;

    if (!cells[i]) emptyCount++;
  }

  return emptyCount;
}

/*------------------------------------------------------------*/

void inputNumber(int number, int cells[], int empty[], int selector) {
  if (empty[selector])
    cells[selector] = number;

  return;
}

/*------------------------------------------------------------*/

void reset(int cells[], int empty[]) {
  for (int i = 0; i < 81; i++)
    if (empty[i]) cells[i] = 0;

  return;
}
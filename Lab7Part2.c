#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
//#include <lab7part2lib.h>
#define BOARDSIZE 26 //Maximum size of the board for the current test
#define MAXMOVES 64 //(BOARDSIZE*BOARDSIZE)
#define BWDIFF 21 //Difference between the ASCII codes for 'B' and 'W'

int dim;

/* Function Prototypes */
int inputDimension();
void assignPlayerColors(char*, char*, int[]);
int countOfTiles(char[][BOARDSIZE], char);
void initializeBoard(char[][BOARDSIZE], int);
bool positionInBounds(int, int);
bool checkAlongDirection(char[][BOARDSIZE], int, int, int, int, char, char[], int);
void updateLegalMoves(char[][BOARDSIZE], char[][3], char[][3]);
void updateBoard(char[][BOARDSIZE], char, char[], char[][3], char[][3]);
void swap(char[][3], int, int);
void sortArray(char[][3]);
void takeHumanMove(char[][BOARDSIZE], char[], char, char[][3], int[]);
bool heuristic(char[][BOARDSIZE], char[][3], char[2], char, int);
void findAiMove(char[][BOARDSIZE], char[], char, char[][3], char[][3], int[]);
void printBoard(char[][BOARDSIZE]);
void initializeArray(char[][3]);
void printArray(char[][3]);
bool searchArray(char[][3], char, char);
void flip(char [][BOARDSIZE], char, char[]);
void play();
int countLegalMoves(char[][3]);
void positionScore(char playerMoves[][3]);
void deltaHumanMoves(char[][BOARDSIZE], char[][3], int, char);
void copyBoard(char[][BOARDSIZE], char[][BOARDSIZE]);
void greedy(char[][BOARDSIZE], char[][3], int, char);
void copyArray(char destArray[][3], char sourceArray[][3]);

/* Takes the input for dimensions */
int inputDimension(){
    printf("Enter the board dimension: ");
    scanf("%d", &dim);
    return dim;
}

/* Takes input of the computer's colour and subsequently defines the user's color*/
void assignPlayerColors(char* aiColor, char* humanColor, int turnState[]){
    printf("Computer plays (B/W) : ");
    scanf(" %c", aiColor);
    switch(*aiColor){
        case 'B':   *humanColor = 'W';
            turnState[0] = 0;
            turnState[1] = 1;
            break;
        case 'W':   *humanColor = 'B';
            turnState[0] = 1;
            turnState[1] = 0;
            break;
        default :   printf("Enter (B/W) only\n"); //error handling
            assignPlayerColors(aiColor, humanColor, turnState);
    }
}

/* Initializes the board to the default starting position */
void initializeBoard(char board[][BOARDSIZE], int dim){
    for(int i = 0; i < dim; i++)
        for (int j = 0; j < dim; j++)
            if((i == ((dim/2) - 1) && j == i) || (i == (dim/2) && j == i))
                board[i][j] = 'W';
            else if((i == ((dim/2) - 1) && j == (i + 1)) || (i == (dim/2) && j == (i - 1)))
                board[i][j] = 'B';
            else
                board[i][j] = 'U';
}

int countOfTiles(char board[][BOARDSIZE], char color){
    int count = 0;
    for(int i = 0; i < dim; i++)
        for(int j = 0; j < dim; j++)
            if(board[i][j] == color) count++;
    return count;
}

/* Initializes the legal-moves array to contain moves outside the board */
void initializeArray(char array[][3]){
    for(int i = 0; i < MAXMOVES; array[i][0] = '#', array[i][1] = '#', array[i][2] = '0', i++);
}

/* Prints the array until the moves lie on the board */
void printArray(char array[][3]){
    int i = 0;
    while(positionInBounds((array[i][0] - 'a'), (array[i][1] - 'a')))
        printf("%c%c%d\n", array[i++][0], array[i][1], (array[i][2] - '0'));
}

/* Searches the array for a move to check if it is in the set of legal moves */
bool searchArray(char playerMoves[][3], char moveCode1, char moveCode2){
    for(int k = 0; positionInBounds((playerMoves[k][0] - 'a'), (playerMoves[k][1] - 'a')); k++)
        if((moveCode1 == playerMoves[k][0]) && (moveCode2 == playerMoves[k][1]))
            return true;
    return false;
}

/* Sorts the array in descending order of score and then ascending order of rows and columns */
void sortArray(char playerMoves[][3]){
    for(int i = 0; positionInBounds((playerMoves[i][0] - 'a'), (playerMoves[i][1] - 'a')); i++)
        for(int j = i+1; positionInBounds((playerMoves[j][0] - 'a'), (playerMoves[j][1] - 'a')); j++){
            if(playerMoves[j][2] > playerMoves[i][2])
                swap(playerMoves, i, j);
            else if(playerMoves[j][2] == playerMoves[i][2]){
                if(playerMoves[i][0] > playerMoves[j][0])
                    swap(playerMoves, i, j);
                else if((playerMoves[i][0] == playerMoves[j][0]) && (playerMoves[i][1] > playerMoves[j][1]))
                    swap(playerMoves, i, j);
            }
        }
}

/* Swaps the content in the ith and jth rows of the array*/
void swap(char playerMoves[][3], int i, int j){
    char temp[3];
    strncpy(&temp[0], &playerMoves[i][0], 3);
    strncpy(&playerMoves[i][0], &playerMoves[j][0], 3);
    strncpy(&playerMoves[j][0], &temp[0], 3);
}

/* Flips along all legal directions for a legal move */
void flip(char localBoard[][BOARDSIZE], char playerColor, char moveCode[]){
    char copyMove[2], finalPos[2];
    for(int i = -1; i < 2; i++)
        for(int j = 1; j > -2; j--)
            if((i || j) && checkAlongDirection(localBoard, (moveCode[0] - 'a'), (moveCode[1] - 'a'), i, j,
                                               playerColor, finalPos, 1)){
                strcpy(&copyMove[0], &moveCode[0]);
                while(!((copyMove[0] == finalPos[0]) && (copyMove[1] == finalPos[1])))
                    localBoard[((copyMove[0] += i) - 'a')][((copyMove[1] += j) - 'a')] = playerColor;
            }
}

/* Prints the board */
void printBoard(char localBoard[][BOARDSIZE]){
    printf("  ");
    for(int i = 0; i < dim; printf("%c", ('a' + i)), i++);
    printf("\n");
    for(int i = 0; i < dim; i++){
        printf("%c ", ('a' + i));
        for(int j = 0; j < dim; printf("%c", localBoard[i][j]), j++);
        printf("\n");
    }
}

/* Tests if the position lies in the board */
bool positionInBounds(int row, int col){
    if(row < dim && col < dim && row >= 0 && col >= 0) return true;
    else return false;
}

/* Checks if a valid position exists along a particular direction */
bool checkAlongDirection(char localBoard[][BOARDSIZE], int row, int col, int deltaRow, int deltaCol,
                         char color, char position[], int utility){
    int i = row, j = col, moveCount = 0;
    char currentChar = localBoard[row][col];
    while((positionInBounds(i,j) && abs(color - currentChar) == BWDIFF) || !moveCount){
        currentChar = localBoard[i+=deltaRow][j+=deltaCol];
        moveCount++;
    }
    if(currentChar == color && moveCount > 1){
        position[0] = (char) (row + 'a' + (moveCount*utility*deltaRow)); //Utilility being one will return the final position
        position[1] = (char) (col + 'a' + (moveCount*utility*deltaCol)); //Utility being zero will return the current position
        position[2] = (char) ('0' + (moveCount - 1)); //Will return true as the direction (and position) is legal.
        return true;
    }
    else return false;
}

/* Updates the legal moves after checking in all directions about all 'U's on the board */
void updateLegalMoves(char localBoard[][BOARDSIZE], char whiteMoves[][3], char blackMoves[][3]){
    int alpha = 4;
    initializeArray(whiteMoves);
    initializeArray(blackMoves);
    char position[3];
    int k = -1, l = -1;
    for(int row = 0; row < dim; row++)
        for(int col = 0; col < dim; col++)
            if(localBoard[row][col] == 'U')
                for(int i = -1; i < 2; i++)
                    for(int j = 1; j > -2; j--)
                        if(i || j){
                            if(checkAlongDirection(localBoard, row, col, i, j, 'W', position, 0)){
                                if(!(whiteMoves[k][0] == position[0] && whiteMoves[k][1] == position[1])) k++;
                                whiteMoves[k][0] = position[0];
                                whiteMoves[k][1] = position[1];
                                whiteMoves[k][2] += alpha*(position[2] - '0');
                            }
                            if(checkAlongDirection(localBoard, row, col, i, j, 'B', position, 0)){
                                if(!(blackMoves[l][0] == position[0] && blackMoves[l][1] == position[1])) l++;
                                blackMoves[l][0] = position[0];
                                blackMoves[l][1] = position[1];
                                blackMoves[l][2] += alpha*(position[2] - '0');
                            }
                        }
}

/* Updates the board by flipping all the tiles after checking for the legality of the move */
void updateBoard(char localBoard[][BOARDSIZE], char playerColor, char moveCode[], char whiteMoves[][3], char blackMoves[][3]){
    if((playerColor == 'W') && searchArray(whiteMoves, moveCode[0], moveCode[1])){
        localBoard[(moveCode[0] - 'a')][(moveCode[1] - 'a')] = 'W';
        flip(localBoard, playerColor, moveCode);
    }
    else if((playerColor == 'B') && searchArray(blackMoves, moveCode[0], moveCode[1])){
        localBoard[(moveCode[0] - 'a')][(moveCode[1] - 'a')] = 'B';
        flip(localBoard, playerColor, moveCode);
    }
}

/* Inputs move from the user, checks validity, and progresses the game */
void takeHumanMove(char localBoard[][BOARDSIZE], char humanMove[], char humanColor, char humanMoves[][3], int turnState[]){
//    int row = 0, col = 0;
//    findSmartestMove(localBoard, dim, humanColor, &row, &col);
//    printf("Testing AI move (row, col): %c%c\n", row + 'a', col + 'a');
//    humanMove[0] = (char)(row + 'a'); humanMove[1] = (char)(col + 'a');
    printf("Enter move for colour %c (RowCol): ", humanColor);
    scanf("%s", humanMove);
    if(searchArray(humanMoves, humanMove[0], humanMove[1])){
        turnState[0] = false;
        turnState[1] = true;
    }
    else turnState[0] = -1; //illegal move entered
}

void positionScore(char playerMoves[][3]){
    int beta = 20;
    int i = 0;
    while(positionInBounds((playerMoves[i][0] - 'a'), (playerMoves[i][1] - 'a'))){
        int row = playerMoves[i][0] - 'a', col = playerMoves[i][1] - 'a';
        if ((row == 0 || row == (dim - 1)) && (col == 0 || col == (dim - 1)))
            playerMoves[i][2] += beta * 10; //corners
        else if (((row == 0 || row == (dim - 1)) && (col != 0 && col != (dim - 1))) ||
                 ((col == 0 || col == (dim - 1)) && (row != 0 && row != (dim - 1))))
            playerMoves[i][2] += beta * 5; //outer-edges
        else if ((row == 1 || row == (dim - 2)) && (col == 1 || col == (dim - 2)))
            playerMoves[i][2] += beta * (-2); //region parallel to outer edges
        else playerMoves[i][2] += beta * 1; //region in the middle of the board
        i++;
    }
}

void copyBoard(char tempBoard[][BOARDSIZE], char sourceBoard[][BOARDSIZE]){
    for(int i = 0; i < dim; i++)
        for(int j = 0; j < dim; j++)
            tempBoard[i][j] = sourceBoard[i][j];
}

void copyArray(char destArray[][3], char sourceArray[][3]){
    for(int i = 0; i < MAXMOVES; i++){
        destArray[i][0] = sourceArray[i][0];
        destArray[i][1] = sourceArray[i][1];
        destArray[i][2] = '0';
    }
}

void deltaHumanMoves(char board[][BOARDSIZE], char aiMoves[][3], int countHumanMoves, char humanColor){
    int gamma = -5, i = 0;
    while(positionInBounds((aiMoves[i][0] - 'a'), (aiMoves[i][1] - 'a'))){
        char tempBoard[BOARDSIZE][BOARDSIZE], copyHumanMoves[MAXMOVES][3], copyAiMoves[MAXMOVES][3], aiMove[2];
        strncpy(&aiMove[0], &aiMoves[i][0], 2);
        initializeArray(copyHumanMoves);
        copyBoard(tempBoard, board);
        copyArray(copyAiMoves, aiMoves);
            switch(humanColor){
                case 'W':   updateBoard(tempBoard, 'B', aiMove, copyHumanMoves, copyAiMoves);
                            updateLegalMoves(tempBoard, copyHumanMoves, copyAiMoves);
                            aiMoves[i][2] += gamma*(countLegalMoves(copyHumanMoves) - countHumanMoves);
                            break;
                case 'B':   updateBoard(tempBoard, 'W', aiMove, copyAiMoves, copyHumanMoves);
                            updateLegalMoves(tempBoard, copyAiMoves, copyHumanMoves);
                            aiMoves[i][2] += gamma*(countLegalMoves(copyHumanMoves) - countHumanMoves);
                            break;
                default:    printf("Error occurred. Wrong Human Color\n"); //error handling
            }
        i++;
    }
}

int countLegalMoves(char playerMoves[][3]){
    int i = 0, count = 0;
    while(positionInBounds((playerMoves[i][0] - 'a'), (playerMoves[i][1] - 'a'))){
        count++;
        i++;
    }
    return count;
}

void greedy(char board[][BOARDSIZE], char aiMoves[][3], int countHumanMoves, char humanColor){
    positionScore(aiMoves);
    deltaHumanMoves(board, aiMoves, countHumanMoves, humanColor);
    sortArray(aiMoves); //required?? how was the program working earlier?
}

/* Contains the heuristic to evaluate AI's move */
bool heuristic(char board[][BOARDSIZE], char aiMoves[][3], char aiMove[2], char humanColor, int countHumanMoves){
    if(positionInBounds((aiMoves[0][0] - 'a'), (aiMoves[0][1] - 'a'))){
        greedy(board, aiMoves, countHumanMoves, humanColor);
        strncpy(&aiMove[0], &aiMoves[0][0], 2);
        return true;
    }
    return false;
}

/* Calculates and returns the AI's move */
void findAiMove(char board[][BOARDSIZE], char aiMove[], char aiColor, char whiteMoves[][3], char blackMoves[][3], int turnState[]){
    switch(aiColor){
        case 'W':   if(heuristic(board, whiteMoves, aiMove, 'B', countLegalMoves(blackMoves))){
                        turnState[1] = false;
                        turnState[0] = true;
                    }
                    else turnState[1] = -1; //no legal moves
                    break;
        case 'B':   if(heuristic(board, blackMoves, aiMove, 'W', countLegalMoves(whiteMoves))){
                        turnState[1] = false;
                        turnState[0] = true;
                    }
                    else turnState[1] = -1; //no legal moves
                    break;
        default :   printf("Error in Computer Color.\n"); //error handling
            turnState[1] = -1;
    }
}

/* Controls and executes the game-play */
void play(){
    char board[BOARDSIZE][BOARDSIZE];
    bool gameOn = true;
    char whiteMoves[MAXMOVES][3], blackMoves[MAXMOVES][3], aiColor, humanColor, aiMove[2], humanMove[2];
    int turnState[2];
    initializeBoard(board, inputDimension());
    assignPlayerColors(&aiColor, &humanColor, turnState);
    printBoard(board);
    updateLegalMoves(board, whiteMoves, blackMoves);
    sortArray(blackMoves);
    sortArray(whiteMoves);
    while(gameOn){
        if(turnState[0] == 1){ //user's move
            if((humanColor == 'W') && positionInBounds((whiteMoves[0][0] - 'a'), (whiteMoves[0][1] - 'a')))
                takeHumanMove(board, humanMove, humanColor, whiteMoves, turnState);
            else if((humanColor == 'B') && positionInBounds((blackMoves[0][0] - 'a'), (blackMoves[0][1] - 'a')))
                takeHumanMove(board, humanMove, humanColor, blackMoves, turnState);
            else if((humanColor == 'W') && positionInBounds((blackMoves[0][0] - 'a'), (blackMoves[0][1] - 'a'))
                    && !positionInBounds((whiteMoves[0][0] - 'a'), (whiteMoves[0][1] - 'a'))){
                printf("%c player has no valid move.\n", humanColor);
                turnState[0] = false;
                turnState[1] = true;
                strcpy(humanMove, "!!");
            }
            else if((humanColor == 'B') && positionInBounds((whiteMoves[0][0] - 'a'), (whiteMoves[0][1] - 'a'))
                    && !positionInBounds((blackMoves[0][0] - 'a'), (blackMoves[0][1] - 'a'))){
                printf("%c player has no valid move.\n", humanColor);
                turnState[0] = false;
                turnState[1] = true;
                strcpy(humanMove, "!!");
            }
            if(turnState[0] == -1){
                gameOn = false;
                printf("Invalid move.\n");
            }
            else if(strcmp(humanMove, "!!") != 0 && (countOfTiles(board, 'U'))){
                updateBoard(board, humanColor, humanMove, whiteMoves, blackMoves);
                printBoard(board);
                updateLegalMoves(board, whiteMoves, blackMoves);
                sortArray(blackMoves);
                sortArray(whiteMoves);
            }
        }
        if(!positionInBounds((whiteMoves[0][0] - 'a'), (whiteMoves[0][1] - 'a'))
           && !positionInBounds((blackMoves[0][0] - 'a'), (blackMoves[0][1] - 'a'))) gameOn = false;
        if((turnState[1] == 1) && gameOn){ //computer's move
            findAiMove(board, aiMove, aiColor, whiteMoves, blackMoves, turnState);
            if(turnState[1] != -1 && (countOfTiles(board, 'U'))){
                printf("Computer places %c at %c%c.\n", aiColor, aiMove[0], aiMove[1]);
                updateBoard(board, aiColor, aiMove, whiteMoves, blackMoves);
                updateLegalMoves(board, whiteMoves, blackMoves);
                printBoard(board);
                sortArray(blackMoves);
                sortArray(whiteMoves);
            }
            else if((humanColor == 'W') && turnState[1] == -1
                    && positionInBounds((whiteMoves[0][0] - 'a'), (whiteMoves[0][1] - 'a'))){
                printf("%c player has no valid move.\n", aiColor);
                turnState[0] = true;
                turnState[1] = false;
            }
            else if((humanColor == 'B') && turnState[1] == -1
                    && positionInBounds((blackMoves[0][0] - 'a'), (blackMoves[0][1] - 'a'))){
                printf("%c player has no valid move.\n", aiColor);
                turnState[0] = true;
                turnState[1] = false;
            }
        }
    }
    if((countOfTiles(board, humanColor) > countOfTiles(board, aiColor) && turnState[0] != -1)) printf("%c player wins.\n", humanColor);
    else if((countOfTiles(board, humanColor) == countOfTiles(board, aiColor)) && turnState[0] != -1) printf("Draw!\n");
    else printf("%c player wins.\n", aiColor);
}

int main(void){
    play();
    return 0;
}
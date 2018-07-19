#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

char board[26][26];
int dim;
const int MAXMOVES = 676, BWDIFF = 21; //Difference between the ASCII codes for 'B' and 'W'

/* Function Prototypes */
int inputDimension();
void assignPlayerColors(char*, char*, int[]);
int countOfTiles(char);
void initializeBoard(int);
bool positionInBounds(int, int);
bool checkAlongDirection(int, int, int, int, char, char[], int);
void updateLegalMoves(char[][3], char[][3]);
void updateBoard(char, char[], char[][3], char[][3]);
void swap(char[][3], int, int);
void sortArray(char[][3]);
void takeHumanMove(char[], char, char[][3], int[]);
bool heuristic(char[][3], char[2]);
void findAiMove(char[], char, char[][3], char[][3], int[]);
void printBoard();
void initializeArray(char[][3]);
void printArray(char[][3]);
bool searchArray(char[][3], char, char);
void flip(char, char[]);
void play();

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
void initializeBoard(int dim){
    for(int i = 0; i < dim; i++)
        for (int j = 0; j < dim; j++)
            if((i == ((dim/2) - 1) && j == i) || (i == (dim/2) && j == i))
                board[i][j] = 'W';
            else if((i == ((dim/2) - 1) && j == (i + 1)) || (i == (dim/2) && j == (i - 1)))
                board[i][j] = 'B';
            else
                board[i][j] = 'U';
}

int countOfTiles(char color){
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
void flip(char playerColor, char moveCode[]){
    char copyMove[2], finalPos[2];
    for(int i = -1; i < 2; i++)
        for(int j = 1; j > -2; j--)
            if((i || j) && checkAlongDirection((moveCode[0] - 'a'), (moveCode[1] - 'a'), i, j, playerColor, finalPos, 1)){
                strcpy(copyMove, &moveCode[0]);
                while(!((copyMove[0] == finalPos[0]) && (copyMove[1] == finalPos[1])))
                    board[((copyMove[0] += i) - 'a')][((copyMove[1] += j) - 'a')] = playerColor;
            }
}

/* Prints the board */
void printBoard(){
    printf("  ");
    for(int i = 0; i < dim; printf("%c", ('a' + i)), i++);
    printf("\n");
    for(int i = 0; i < dim; i++){
        printf("%c ", ('a' + i));
        for(int j = 0; j < dim; printf("%c", board[i][j]), j++);
        printf("\n");
    }
}

/* Tests if the position lies in the board */
bool positionInBounds(int row, int col){
    if(row < dim && col < dim && row >= 0 && col >= 0) return true;
    else return false;
}

/* Checks if a valid position exists along a particular direction */
bool checkAlongDirection(int row, int col, int deltaRow, int deltaCol, char color, char position[], int utility){
    int i = row, j = col, moveCount = 0;
    char currentChar = board[row][col];
    while((positionInBounds(i,j) && abs(color - currentChar) == BWDIFF) || !moveCount){
        currentChar = board[i+=deltaRow][j+=deltaCol];
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
void updateLegalMoves(char whiteMoves[][3], char blackMoves[][3]){
    initializeArray(whiteMoves);
    initializeArray(blackMoves);
    char position[3];
    int k = -1, l = -1;
    for(int row = 0; row < dim; row++)
        for(int col = 0; col < dim; col++)
            if(board[row][col] == 'U')
                for(int i = -1; i < 2; i++)
                    for(int j = 1; j > -2; j--)
                        if(i || j){
                            if(checkAlongDirection(row, col, i, j, 'W', position, 0)){
                                if(!(whiteMoves[k][0] == position[0] && whiteMoves[k][1] == position[1])) k++;
                                whiteMoves[k][0] = position[0];
                                whiteMoves[k][1] = position[1];
                                whiteMoves[k][2] += (position[2] - '0');
                            }
                            if(checkAlongDirection(row, col, i, j, 'B', position, 0)){
                                if(!(blackMoves[l][0] == position[0] && blackMoves[l][1] == position[1])) l++;
                                blackMoves[l][0] = position[0];
                                blackMoves[l][1] = position[1];
                                blackMoves[l][2] += (position[2] - '0');
                            }
                        }
}

/* Updates the board by flipping all the tiles after checking for the legality of the move */
void updateBoard(char playerColor, char moveCode[], char whiteMoves[][3], char blackMoves[][3]){
    if((playerColor == 'W') && searchArray(whiteMoves, moveCode[0], moveCode[1])){
            board[(moveCode[0] - 'a')][(moveCode[1] - 'a')] = 'W';
            flip(playerColor, moveCode);
    }
    else if((playerColor == 'B') && searchArray(blackMoves, moveCode[0], moveCode[1])){
            board[(moveCode[0] - 'a')][(moveCode[1] - 'a')] = 'B';
            flip(playerColor, moveCode);
    }
}

/* Inputs move from the user, checks validity, and progresses the game */
void takeHumanMove(char humanMove[], char humanColor, char humanMoves[][3], int turnState[]){
    printf("Enter move for colour %c (RowCol): ", humanColor);
    scanf("%s", humanMove);
    if(searchArray(humanMoves, humanMove[0], humanMove[1])){
        turnState[0] = false;
        turnState[1] = true;
    }
    else turnState[0] = -1; //illegal move entered
}

/* Contains the heuristic to evaluate AI's move */
bool heuristic(char aiMoves[][3], char aiMove[2]){
    if(positionInBounds((aiMoves[0][0] - 'a'), (aiMoves[0][1] - 'a'))){
        strncpy(aiMove, &aiMoves[0][0], 2);
        return true;
    }
    return false;
}

/* Calculates and returns the AI's move */
void findAiMove(char aiMove[], char aiColor, char whiteMoves[][3], char blackMoves[][3], int turnState[]){
    switch(aiColor){
        case 'W':   if(heuristic(whiteMoves, aiMove)){
                        turnState[1] = false;
                        turnState[0] = true;
                    }
                    else turnState[1] = -1; //no legal moves
                    break;
        case 'B':   if(heuristic(blackMoves, aiMove)){
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
    bool gameOn = true;
    char whiteMoves[MAXMOVES][3], blackMoves[MAXMOVES][3], aiColor, humanColor, aiMove[2], humanMove[2];
    int turnState[2];
    initializeBoard(inputDimension());
    assignPlayerColors(&aiColor, &humanColor, turnState);
    printBoard();
    updateLegalMoves(whiteMoves, blackMoves);
    sortArray(blackMoves);
    sortArray(whiteMoves);
    while(gameOn){
        if(turnState[0] == 1){ //user's move
            if((humanColor == 'W') && positionInBounds((whiteMoves[0][0] - 'a'), (whiteMoves[0][1] - 'a')))
                takeHumanMove(humanMove, humanColor, whiteMoves, turnState);
            else if((humanColor == 'B') && positionInBounds((blackMoves[0][0] - 'a'), (blackMoves[0][1] - 'a')))
                takeHumanMove(humanMove, humanColor, blackMoves, turnState);
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
            else if(strcmp(humanMove, "!!") != 0 && (countOfTiles('U'))){
                updateBoard(humanColor, humanMove, whiteMoves, blackMoves);
                printBoard();
                updateLegalMoves(whiteMoves, blackMoves);
                sortArray(blackMoves);
                sortArray(whiteMoves);
            }
        }
        if(!positionInBounds((whiteMoves[0][0] - 'a'), (whiteMoves[0][1] - 'a'))
           && !positionInBounds((blackMoves[0][0] - 'a'), (blackMoves[0][1] - 'a'))) gameOn = false;
        if((turnState[1] == 1) && gameOn){ //computer's move
            findAiMove(aiMove, aiColor, whiteMoves, blackMoves, turnState);
            if(turnState[1] != -1 && (countOfTiles('U'))){
                printf("Computer places %c at %s.\n", aiColor, aiMove);
                updateBoard(aiColor, aiMove, whiteMoves, blackMoves);
                updateLegalMoves(whiteMoves, blackMoves);
                printBoard();
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
    if((countOfTiles(humanColor) > countOfTiles(aiColor) && turnState[0] != -1)) printf("%c player wins.\n", humanColor);
    else if((countOfTiles(humanColor) == countOfTiles(aiColor)) && turnState[0] != -1) printf("Draw!\n");
    else printf("%c player wins.\n", aiColor);
}

int main(void){
    play();
    return 0;
}
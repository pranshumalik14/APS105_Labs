#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

char board[26][26];
int dim;
const int MAXMOVES = 676;


/* Takes the input for dimensions */
int inputDimension(int *dim){
    printf("Enter the board dimension: ");
    scanf("%d", dim);
    return *dim;
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

/* Changes the board to the entered configuration */
void configureBoard(){
    char configCode[3];
    printf("Enter board configuration:\n");
    scanf("%s", configCode);
    while(strcmp(configCode, "!!!")){
        board[(configCode[1] - 'a')][(configCode[2] - 'a')] = configCode[0];
        scanf("%s", configCode);
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
    while((positionInBounds(i,j) && abs(color - currentChar) == 21) || !moveCount){
        currentChar = board[i+=deltaRow][j+=deltaCol];
        moveCount++;
    }
    if(currentChar == color && moveCount > 1){
        position[0] = (char) (row + 'a' + (moveCount*utility*deltaRow)); //Utilility being one will return the final position
        position[1] = (char) (col + 'a' + (moveCount*utility*deltaCol)); //Utility being zero will return the current position
        return true; //Will return true as the direction (and position) is legal.
    }
    else return false;
}

/* Updates the legal moves after checking in all directions about all 'U's on the board */
void updateLegalMoves(char whiteMoves[][2], char blackMoves[][2]){
    char position[2];
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
                            }
                            if(checkAlongDirection(row, col, i, j, 'B', position, 0)){
                                if(!(blackMoves[l][0] == position[0] && blackMoves[l][1] == position[1])) l++;
                                blackMoves[l][0] = position[0];
                                blackMoves[l][1] = position[1];
                            }
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

/* Initializes the legal-moves array to contain moves outside the board */
void initializeArray(char array[][2]){
    for(int i = 0; i < MAXMOVES; array[i][0] = '#', array[i][1] = '#', i++);
}

/* Prints the array until the moves lie on the board */
void printArray(char array[][2]){
    int i = 0;
    while(positionInBounds((array[i][0] - 'a'), (array[i][1] - 'a')))
        printf("%c%c\n", array[i++][0], array[i][1]);
}

/* Searches the array for a move to check if it is in the set of legal moves */
bool searchArray(char playerMoves[][2], char moveCode1, char moveCode2){
    for(int k = 0; positionInBounds((playerMoves[k][0] - 'a'), (playerMoves[k][1] - 'a')); k++)
        if((moveCode1 == playerMoves[k][0]) && (moveCode2 == playerMoves[k][1]))
            return true;
    return false;
}

/* Updates the board by flipping all the tiles after checking for the legality of the move */
void updateBoard(char whiteMoves[][2], char blackMoves[][2]){
    char moveCode[3], finalPos[2], copyMove[2];
    printf("Enter a move:\n");
    scanf("%s", moveCode);
    if(moveCode[0] == 'W')
        if(searchArray(whiteMoves, moveCode[1], moveCode[2])){
            printf("Valid Move.\n");
            board[(moveCode[1] - 'a')][(moveCode[2] - 'a')] = 'W';
            for(int i = -1; i < 2; i++)
                for(int j = 1; j > -2; j--)
                    if((i || j) && checkAlongDirection((moveCode[1] - 'a'), (moveCode[2] - 'a'), i, j, 'W', finalPos, 1)){
                        strncpy(copyMove, &moveCode[1], 2);
                        while(!((copyMove[0] == finalPos[0]) && (copyMove[1] == finalPos[1])))
                            board[((copyMove[0] += i) - 'a')][((copyMove[1] += j) - 'a')] = 'W';
                    }
        }
        else printf("Invalid Move.\n");
    else if(moveCode[0] == 'B')
        if(searchArray(blackMoves, moveCode[1], moveCode[2])){
            printf("Valid Move.\n");
            board[(moveCode[1] - 'a')][(moveCode[2] - 'a')] = 'B';
            for(int i = -1; i < 2; i++)
                for(int j = 1; j > -2; j--)
                    if((i || j) && checkAlongDirection((moveCode[1] - 'a'), (moveCode[2] - 'a'), i, j, 'B', finalPos, 1)){
                        strncpy(copyMove, &moveCode[1], 2);
                        while(!((copyMove[0] == finalPos[0]) && (copyMove[1] == finalPos[1])))
                            board[((copyMove[0] += i) - 'a')][((copyMove[1] += j) - 'a')] = 'B';
                    }
        }
        else printf("Invalid Move.\n");
    else printf("Invalid move.\n");
}

int main(void){
    char whiteMoves[MAXMOVES][2], blackMoves[MAXMOVES][2];
    initializeBoard(inputDimension(&dim));
    initializeArray(whiteMoves);
    initializeArray(blackMoves);
    printBoard();
    configureBoard();
    updateLegalMoves(whiteMoves, blackMoves);
    printBoard();
    printf("Available moves for W:\n");
    printArray(whiteMoves);
    printf("Available moves for B:\n");
    printArray(blackMoves);
    updateBoard(whiteMoves, blackMoves);
    printBoard();
    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#define BOARDSIZE 8 //Maximum size of the board for the current test
#define MAXMOVES (BOARDSIZE*BOARDSIZE)
#define BWDIFF 21 //Difference between the ASCII codes for 'B' and 'W'

char board[BOARDSIZE][BOARDSIZE];
int dim;
typedef struct score{
    int flipCount; //stores the number of flips
    int positionScore; //stores the position-based favorability
    int deltaOpponentMoves; //stores the difference in opponent's moves of the opponent for each move
    int deltaSelfTiles; //stores the difference in own number of tiles for each move for a variable depth
    double totalScore; //stores the total score using the composite scoring defined in the function
}score;

/* Function Prototypes */
int inputDimension();
void assignPlayerColors(char*, char*, int[]);
int countOfTiles(char);
void initializeBoard(int);
bool positionInBounds(int, int);
bool checkAlongDirection(int, int, int, int, char, char[], int);
void updateLegalMoves(char[][BOARDSIZE], char[][2], char[][2], score[], score[]);
void updateBoard(char[][BOARDSIZE], char, char[], char[][2], char[][2]);
void swap(char[][2], score[], int, int);
void sortArray(char[][2], score*);
void takeHumanMove(char[], char, char[][2], int[]);
bool heuristic(char[][2], char[2], score[], char, char[][2]);
void findAiMove(char[], char, char[][2], char[][2], score[], score[], int[]);
void copyBoard(char[][BOARDSIZE]);
void printBoard();
void initializeArray(char[][2]);
void printArray(char[][2]);
bool searchArray(char[][2], char, char);
void flip(char, char[]);
void play();
void deltaHumanMoves(char, char, char, char[][2], char[][2], score*);
void positionScore(int, int, score*);
void deltaAiTiles();
void greedy(char[][2], score[], char, char[][2]);
void midGame();
void initializeScore(score[]);
int countLegalMoves(char[][2]);


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

/* Counts the number of tiles for the given color */
int countOfTiles(char color){
    int count = 0;
    for(int i = 0; i < dim; i++)
        for(int j = 0; j < dim; j++)
            if(board[i][j] == color) count++;
    return count;
}

/* Initializes all the scores to zero  */
void initializeScore(score array[]){
    for(int i = 0; i < MAXMOVES; i++) {
        array[i].positionScore = 0;
        array[i].flipCount = 0;
        array[i].deltaOpponentMoves = 0;
        array[i].totalScore = 0.0;
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

/* Copies the source array into the destination array */
void copyArray(char destArray[][2], char sourceArray[][2]){
    for(int i = 0; i < MAXMOVES; i++){
        destArray[i][0] = sourceArray[i][0];
        destArray[i][1] = sourceArray[i][1];
    }
}

/* Searches the array for a move to check if it is in the set of legal moves */
bool searchArray(char playerMoves[][2], char moveCode1, char moveCode2){
    for(int k = 0; positionInBounds((playerMoves[k][0] - 'a'), (playerMoves[k][1] - 'a')); k++)
        if((moveCode1 == playerMoves[k][0]) && (moveCode2 == playerMoves[k][1]))
            return true;
    return false;
}

/* Sorts the array in descending order of score and then ascending order of rows and columns */
void sortArray(char playerMoves[][2], score playerScore[]){
    for(int i = 0; positionInBounds((playerMoves[i][0] - 'a'), (playerMoves[i][1] - 'a')); i++)
        for(int j = i+1; positionInBounds((playerMoves[j][0] - 'a'), (playerMoves[j][1] - 'a')); j++){
            if(playerScore[j].totalScore > playerScore[i].totalScore)
                swap(playerMoves, playerScore, i, j);
            else if(playerScore[j].totalScore == playerScore[i].totalScore){
                if(playerMoves[i][0] > playerMoves[j][0])
                    swap(playerMoves, playerScore, i, j);
                else if((playerMoves[i][0] == playerMoves[j][0]) && (playerMoves[i][1] > playerMoves[j][1]))
                    swap(playerMoves, playerScore, i, j);
            }
        }
}

/* Swaps the content in the ith and jth rows of the array*/
void swap(char playerMoves[][2], score playerScore[], int i, int j){
    score tempScore = playerScore[i];
    char tempMove[2];
    strncpy(&tempMove[0], &playerMoves[i][0], 2);
    strncpy(&playerMoves[i][0], &playerMoves[j][0], 2);
    strncpy(&playerMoves[j][0], &tempMove[0], 2);
    playerScore[i] = playerScore[j];
    playerScore[j] = tempScore;
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

/* Copies the current board into an array */
void copyBoard(char tempBoard[][BOARDSIZE]){
    for(int i = 0; i < dim; i++)
        for(int j = 0; j <dim; j++)
            tempBoard[i][j] = board[i][j];
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

/* Counts the number of legal moves in the array */
int countLegalMoves(char playerMoves[][2]){
    int i = 0, count = 0;
    while(positionInBounds((playerMoves[i][0] - 'a'), (playerMoves[i][0] - 'a'))){
        count++;
        i++;
    }
    return count;
}

/* Updates the legal moves after checking in all directions about all 'U's on the board */
void updateLegalMoves(char board[][BOARDSIZE], char whiteMoves[][2], char blackMoves[][2], score whiteScore[], score blackScore[]){
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
                                whiteScore[k].flipCount += (position[2] - '0');
                            }
                            if(checkAlongDirection(row, col, i, j, 'B', position, 0)){
                                if(!(blackMoves[l][0] == position[0] && blackMoves[l][1] == position[1])) l++;
                                blackMoves[l][0] = position[0];
                                blackMoves[l][1] = position[1];
                                blackScore[l].flipCount += (position[2] - '0');
                            }
                        }
}

/* Updates the board by flipping all the tiles after checking for the legality of the move */
void updateBoard(char board[][BOARDSIZE], char playerColor, char moveCode[], char whiteMoves[][2], char blackMoves[][2]){
    if((playerColor == 'W') && searchArray(whiteMoves, moveCode[0], moveCode[1])){
        board[(moveCode[0] - 'a')][(moveCode[1] - 'a')] = 'W';
        flip(playerColor, moveCode);
    }
    else if((playerColor == 'B') && searchArray(blackMoves, moveCode[0], moveCode[1])){
        board[(moveCode[0] - 'a')][(moveCode[1] - 'a')] = 'B';
        flip(playerColor, moveCode);
    }
}

/* Counts the change in the number of human's moves for each AI's moves */
void deltaHumanMoves(char humanColor, char aiMove1, char aiMove2, char humanMoves[][2], char aiMoves[][2], score *aiScore){
    char tempBoard[BOARDSIZE][BOARDSIZE], aiMove[2], copyHumanMoves[MAXMOVES][2], copyAiMoves[MAXMOVES][2];
    score localHumanScore[MAXMOVES], localAiScore[MAXMOVES]; //are not used in the algorithm currently, but can be later used.
    aiMove[0] = aiMove1; aiMove[1] = aiMove2;
    copyBoard(tempBoard);
    copyArray(copyAiMoves, aiMoves);
    copyArray(copyHumanMoves, humanMoves);
    switch(humanColor){
        case 'W':   updateBoard(tempBoard, 'B', aiMove, copyHumanMoves, copyAiMoves);
                    updateLegalMoves(tempBoard, copyHumanMoves, copyAiMoves, localHumanScore, localAiScore);
                    aiScore->deltaOpponentMoves = countLegalMoves(copyHumanMoves) - countLegalMoves(humanMoves);
                    break;
        case 'B':   updateBoard(tempBoard, 'W', aiMove, copyAiMoves, copyHumanMoves);
                    updateLegalMoves(tempBoard, copyHumanMoves, copyAiMoves, localHumanScore, localAiScore);
                    aiScore->deltaOpponentMoves = countLegalMoves(copyHumanMoves) - countLegalMoves(humanMoves);
                    break;
        default:    printf("Error occurred. Wrong Human Color\n"); //error handling
    }
}

/* Assigns the position-based score to the respective move on the board */
void positionScore(int row, int col, score *playerScore){
    if((row == 0 || row == (dim - 1)) && (col == 0 || col == (dim - 1)))
        playerScore->positionScore = 10; //corners
    else if(((row == 0 || row == (dim - 1)) && (col != 0 && col != (dim - 1))) ||
            ((col == 0 || col == (dim - 1)) && (row != 0 && row != (dim - 1))))
        playerScore->positionScore = 5; //outer-edges
    else if((row == 1 || row == (dim - 2)) && (col == 1 || col == (dim - 2)))
        playerScore->positionScore = -2; //region parallel to outer edges
    else playerScore->positionScore = 1; //region in the middle of the board
}

/* Finds change in the AI's tiles for all moves at a depth of two */
void deltaAiTiles(){

}

/* Implements the greedy algorithm to get the immediate best move */
void greedy(char aiMoves[][2], score aiScore[], char humanColor, char humanMoves[][2]){
    double alpha = 0.25, beta = 1, gamma = -0.25;
    int i = 0;
    while(positionInBounds((aiMoves[i][0] - 'a'), (aiMoves[i][1] - 'a'))){
        positionScore((aiMoves[i][0] - 'a'), (aiMoves[i][1] - 'a'), &aiScore[i]);
        deltaHumanMoves(humanColor, aiMoves[i][0], aiMoves[i][1], humanMoves, aiMoves, &aiScore[i]);
        aiScore[i].totalScore = alpha*aiScore[i].flipCount + beta*aiScore[i].positionScore + gamma*aiScore[i].deltaOpponentMoves;
        i++;
    }
    sortArray(aiMoves, aiScore);
}

/* Minimizes AI's tiles on the board during the mid-game */
void midGame(){
    //Have dynamic weights for the greedy and the deltaAiTiles for different stages of the game.
}

/* Inputs move from the user, checks validity, and progresses the game */
void takeHumanMove(char humanMove[], char humanColor, char humanMoves[][2], int turnState[]){
    printf("Enter move for colour %c (RowCol): ", humanColor);
    scanf("%s", humanMove);
    if(searchArray(humanMoves, humanMove[0], humanMove[1])){
        turnState[0] = false;
        turnState[1] = true;
    }
    else turnState[0] = -1; //illegal move entered
}

/* Contains the heuristic to evaluate AI's move */
bool heuristic(char aiMoves[][2], char aiMove[2], score aiScore[], char humanColor, char humanMoves[][2]){
    if(positionInBounds((aiMoves[0][0] - 'a'), (aiMoves[0][1] - 'a'))){
        greedy(aiMoves, aiScore, humanColor, humanMoves);
        strncpy(aiMove, &aiMoves[0][0], 2);
        return true;
    }
    return false;
}

/* Calculates and returns the AI's move */
void findAiMove(char aiMove[], char aiColor, char whiteMoves[][2], char blackMoves[][2], score whiteScore[],
                score blackScore[], int turnState[]){
    switch(aiColor){
        case 'W':   if(heuristic(whiteMoves, aiMove, whiteScore, 'B', blackMoves)){
                        turnState[1] = false;
                        turnState[0] = true;
                    }
                    else turnState[1] = -1; //no legal moves
                    break;
        case 'B':   if(heuristic(blackMoves, aiMove, blackScore, 'W', whiteMoves)){
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
    char whiteMoves[MAXMOVES][2], blackMoves[MAXMOVES][2], aiColor, humanColor, aiMove[2], humanMove[2];
    score blackScore[MAXMOVES], whiteScore[MAXMOVES];
    int turnState[2];
    initializeBoard(inputDimension());
    assignPlayerColors(&aiColor, &humanColor, turnState);
    initializeScore(blackScore);
    initializeScore(whiteScore);
    printBoard();
    updateLegalMoves(board, whiteMoves, blackMoves, whiteScore, blackScore);
    sortArray(blackMoves, blackScore);
    sortArray(whiteMoves, whiteScore);
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
            else if(strcmp(humanMove, "!!") != 0){
                updateBoard(board, humanColor, humanMove, whiteMoves, blackMoves);
                printBoard();
                updateLegalMoves(board, whiteMoves, blackMoves, whiteScore, blackScore);
                sortArray(blackMoves, blackScore);
                sortArray(whiteMoves, whiteScore);
            }
        }
        if(!positionInBounds((whiteMoves[0][0] - 'a'), (whiteMoves[0][1] - 'a'))
           && !positionInBounds((blackMoves[0][0] - 'a'), (blackMoves[0][1] - 'a'))) gameOn = false;
        if((turnState[1] == 1) && gameOn){ //computer's move
            findAiMove(aiMove, aiColor, whiteMoves, blackMoves, whiteScore, blackScore, turnState);
            if(turnState[1] != -1){
                printf("Computer places %c at %s.\n", aiColor, aiMove);
                updateBoard(board, aiColor, aiMove, whiteMoves, blackMoves);
                updateLegalMoves(board, whiteMoves, blackMoves, whiteScore, blackScore);
                printBoard();
                sortArray(blackMoves, blackScore);
                sortArray(whiteMoves, whiteScore);
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
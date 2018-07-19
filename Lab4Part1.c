#include <stdio.h>

int takeInput(){
    int rows;
    printf("Enter the number of rows in the triangle: ");
    scanf("%d", &rows);
    return rows;
}

void makeTriangle(int rows){
    for (int i = 0; i < (rows - 1); i++){
        for (int j = 1; j <= (2*rows - 1); j++)
            if(j == (rows - i) || j == (rows + i)) printf("^");
            else printf(" ");
        printf("\n");
    }
    for (int k = 1; k <= (2*rows - 1); k++) printf("^");
    printf("\n");
}

int main(void){
    makeTriangle(takeInput());
    return 0;
}
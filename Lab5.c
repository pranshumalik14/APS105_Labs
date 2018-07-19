#include <stdio.h>

void takeInput(int *r){
    printf("Enter the number of rows: ");
    scanf("%d", r);
}

int factorial(int n){
    int factorial = 1;
    for (int i = 1; (i <= n) && (factorial *= i); i++); //condensed factorial calculator
    return factorial;
}

int choose(int n, int r){
    return (factorial(n) / (factorial(r) * factorial(n-r))); //formula for n choose r (nCr)
}

void spaces(int n){
    for(int i = 0; i<n && printf(" "); i++); //condensed spaces printer loop
}

void triangle(int r){
    for (int i = 0; i < r; i++){
        spaces(3 * (r - (i+1))); //prints the initial (r-i-1) space blocks of 3 spaces
        for (int j = 0; j <= i; j++) {
            printf("%-3d",choose(i, j)); //prints nCr
            spaces(3); //every number has 3 spaces between itself and the next entry
        }
        printf("\n");
    }
}

int main(void){
    int r;
    takeInput(&r);
    while(r >= 0){
        triangle(r);
        takeInput(&r);
    }
    return 0;
}
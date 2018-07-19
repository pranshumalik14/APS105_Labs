#include <stdio.h>

/* Function to update the amount of cents */
void input(int *inputCents){
    printf("Please give an amount in cents less than 100: ");
    scanf("%d", inputCents);
}

/* Function to calculate the numbers of denominations */
void calculateDenomination(int inputCents){
    int zero = 0, quarters = 0, dimes = 0, nickels = 0, cents = 0;
    printf("%d cents:", inputCents);

    /* Finding the number of Quarters */
    quarters += inputCents/25;
    if(quarters > 1)
        printf(" %d quarters", quarters);
    else if(quarters == 1)
        printf(" 1 quarter");
    if(inputCents/25 != 0 && (inputCents%25 != 0)) printf(",");
    if((inputCents %= 25) == 0 && !(zero++)) printf(".\n"); //Checking if the remainder equals zero.

    /* Finding the number of Dimes */
    dimes += inputCents/10;
    if(dimes > 1)
        printf(" %d dimes", dimes);
    else if(dimes == 1)
        printf(" 1 dime");
    if(inputCents/10 != 0 && (inputCents%10 != 0)) printf(",");
    if((inputCents %= 10) == 0 && !(zero++)) printf(".\n"); //print the full stop only once.

    /* Finding the number of Nickels */
    nickels += inputCents/5;
    if(nickels > 1)
        printf(" %d nickels", nickels);
    else if(nickels == 1)
        printf(" 1 nickel");
    if(inputCents/5 != 0 && (inputCents%5 != 0)) printf(",");
    if((inputCents %= 5) == 0 && !(zero++)) printf(".\n"); //post increment for later effect.

    /* Finding the number of Cents */
    cents += inputCents;
    if(cents > 1)
        printf(" %d cents.\n", cents);
    else if(cents == 1)
        printf(" 1 cent.\n");
}

int main(void){
    int amountCents = 0; // number of cents entered by the user

    do{
        input(&amountCents);
        if ((amountCents > 0) && (amountCents < 100)) calculateDenomination(amountCents);
    }while((amountCents > 0) && (amountCents < 100)); //only works for 0 < amountCents < 100.

    printf("Goodbye.\n");
    return 0;
}
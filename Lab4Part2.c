#include <stdio.h>
#include <math.h>

int numIn, numOut = 0, reserve, i = 0; //Global variable for user input and program output

char takeInput(){
    char selection;
    printf("Enter B for conversion of Binary to Decimal, OR\nEnter D for conversion of Decimal to Binary: ");
    scanf("%c", &selection);
    printf("Enter your number: ");
    return selection;
}

void convertDecimalToBinary(){
    scanf("%d", &numIn);
    reserve = numIn;
    do  numOut += (numIn%2) * pow(10, i++); //Raises 10 to the place value and multiplies it by the remainder at that place
    while(numIn /= 2); //Condensed do-while loop
    printf("%d in decimal = %d in binary", reserve, numOut);
}

void convertBinaryToDecimal(){
    scanf("%d", &numIn);
    reserve = numIn;
    do  numOut += (numIn%10) * pow(2, i++); //Raises 2 to the place value and multiplies it by the digit at that place
    while(numIn /= 10); //Condensed do-while loop
    printf("%d in binary = %d in decimal", reserve, numOut);
}

int main(void){
    switch (takeInput()){
        case 'B':   convertBinaryToDecimal();
                    break;
        case 'D':   convertDecimalToBinary();
                    break;
        default: printf("Invalid input; Goodbye\n");
    }
    return 0;
}
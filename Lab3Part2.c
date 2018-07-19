#include <stdio.h>

void add(double num1, double num2){
    printf("Sum of %.2lf and %.2lf is %.2lf", num1, num2, num1+num2);
}

void subtract(double num1, double num2){
    printf("Difference of %.2lf from %.2lf is %.2lf", num1, num2, num1-num2);
}

void multiply(double num1, double num2){
    printf("Product of %.2lf and %.2lf is %.2lf", num1, num2, num1*num2);
}

void divide(double num1, double num2){
    if(num2)
        printf("Division of %.2lf by %.2lf is %.2lf\n", num1, num2, num1/num2);
    else
        printf("Error, trying to divide by zero\n");
}

int main(void){
    double num1, num2;
    char c;

    /* Taking all inputs */
    printf("Enter first number: ");
    scanf("%lf", &num1);
    printf("Enter second number: ");
    scanf("%lf", &num2);
    printf("Enter calculation command (one of a, s, m, or d): ");
    scanf(" %c", &c);

    /* Interpretation and Calculation */
    switch(c){
        case 'a':   add(num1,num2);
                    break;
        case 's':   subtract(num1,num2);
                    break;
        case 'm':   multiply(num1, num2);
                    break;
        case 'd':   divide(num1, num2);
                    break;
        default:    printf("Error, unknown calculation command given\n");
    }
    return 0;
}
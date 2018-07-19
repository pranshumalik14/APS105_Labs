#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* Multiple ways come to mind when coding this program: */
/* 1. File management. Entering data into a file, and then searching for the record. */
/* 2. Making an array of structures and then searching for the record */
/* 3. Making a 2D array (12 by 2) of pointers to store strings and values (as indexes) */

struct colorCode{
    char character;
    char color[10];
    float value;
    float tolerance;
};

char bands[4]; //Global variable to store the inputs and use them across functions

struct colorCode colorCodes[12]; //declaring an array of structs

void declareColorCodes(){
    /* Declaring the Colors as type colorCode  */

    /* colorCode Black  details */
    colorCodes[0].character = 'K';
    strcpy(colorCodes[0].color, "Black");
    colorCodes[0].value = 0.0;

    /* colorCode Brown details */
    colorCodes[1].character = 'B';
    strcpy(colorCodes[1].color, "Brown");
    colorCodes[1].value = 1.0;
    colorCodes[1].tolerance = 1.0;

    /* colorCode Red details */
    colorCodes[2].character = 'R';
    strcpy(colorCodes[2].color, "Red");
    colorCodes[2].value = 2.0;
    colorCodes[2].tolerance = 2.0;

    /* colorCode Orange details */
    colorCodes[3].character = 'O';
    strcpy(colorCodes[3].color, "Orange");
    colorCodes[3].value = 3.0;

    /* colorCode Yellow details */
    colorCodes[4].character = 'E';
    strcpy(colorCodes[4].color, "Yellow");
    colorCodes[4].value = 4.0;

    /* colorCode Green details */
    colorCodes[5].character = 'G';
    strcpy(colorCodes[5].color, "Green");
    colorCodes[5].value = 5.0;
    colorCodes[5].tolerance = 0.5;

    /* colorCode Blue details */
    colorCodes[6].character = 'U';
    strcpy(colorCodes[6].color, "Blue");
    colorCodes[6].value = 6.0;
    colorCodes[6].tolerance = 0.25;

    /* colorCode Violet details */
    colorCodes[7].character = 'V';
    strcpy(colorCodes[7].color, "Violet");
    colorCodes[7].value = 7.0;
    colorCodes[7].tolerance = 0.1;

    /* colorCode Grey details */
    colorCodes[8].character = 'Y';
    strcpy(colorCodes[8].color, "Grey");
    colorCodes[8].value = 8.0;
    colorCodes[8].tolerance = 0.05;

    /* colorCode White details */
    colorCodes[9].character = 'W';
    strcpy(colorCodes[9].color, "White");
    colorCodes[9].value = 9.0;

    /* colorCode Gold  details */
    colorCodes[10].character = 'L';
    strcpy(colorCodes[10].color, "Gold");
    colorCodes[10].value = -1.0;
    colorCodes[10].tolerance = 5.0;

    /* colorCode Silver details */
    colorCodes[11].character = 'S';
    strcpy(colorCodes[11].color, "Silver");
    colorCodes[11].value = -2.0;
    colorCodes[11].tolerance = 10.0;
}

void takeInput(){
    /* Taking inputs */

    printf("Please enter the 1st band:\n");
    scanf(" %c", &bands[0]);
    printf("Please enter the 2nd band:\n");
    scanf(" %c", &bands[1]);
    printf("Please enter the multiplier band:\n");
    scanf(" %c", &bands[2]);
    printf("Please enter the tolerance band:\n");
    scanf(" %c", &bands[3]);
}

void calculateResistance(char bands[4]){
    /* Declaring all variables */

    int bandsIndex[4] = {0, 0, 0, 0}, resistanceExponent;
    double resistanceMantissa = 0.0;

    /* Searching for the band and saving its index (in the array of structs) into bandsIndex[] */

    for (int i = 0; i<=3; i++) {
        for (int j = 0; j < 12; j++)
            if ((toupper(bands[i]) == colorCodes[j].character) && (i < 2)) {
                resistanceMantissa += colorCodes[j].value * pow(10, (-i)); //Converting the mantissa to the standard form
                bandsIndex[i] = j;
            }
            else if ((toupper(bands[i]) == colorCodes[j].character) && (i >= 2))
                bandsIndex[i] = j;
    }
    /* Calculating the residue of the power and accounting for it in the mantissa */

    resistanceExponent = (int)(colorCodes[(bandsIndex[2])].value + 1)%3;
    resistanceMantissa *= pow(10, resistanceExponent);
    printf("Resistor bands: %s %s %s %s\n", colorCodes[(bandsIndex[0])].color, colorCodes[(bandsIndex[1])].color, colorCodes[(bandsIndex[2])].color, colorCodes[(bandsIndex[3])].color);

    /* Determining the order character before Ohms */

    if ((int)(colorCodes[(bandsIndex[2])].value + 1)/3 == 1)
        printf("Resistance: %.2lf %cOhms +/- %.2lf%%\n", resistanceMantissa, 'K', colorCodes[(bandsIndex[3])].tolerance);
    else if ((int)(colorCodes[(bandsIndex[2])].value + 1)/3 >= 2)
        printf("Resistance: %.2lf %cOhms +/- %.2lf%%\n", resistanceMantissa, 'M', colorCodes[(bandsIndex[3])].tolerance);
    else
        printf("Resistance: %.2lf Ohms +/- %.2lf%%\n", resistanceMantissa, colorCodes[(bandsIndex[3])].tolerance);
}

int main(void){
    declareColorCodes();
    takeInput();
    calculateResistance(bands);
    return 0;
}
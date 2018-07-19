#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#define MAX_LIB_SIZE 100
#define MAX_LENGTH 1024

typedef struct song{
    char songName[MAX_LENGTH];
    char artist[MAX_LENGTH];
    char genre[MAX_LENGTH];
} Song;

/* Function Prototypes */
void inputStringFromUser(char prompt[], char s[], int arraySize);
void printMusicLibraryEmpty();
void printMusicLibraryTitle();
void printLibrary(Song library[], int size);
void cocktailSort(Song library[], int size);
int customStrcmp(char *, char*);

/* Prompt the user for a string safely, without buffer overflow */
void inputStringFromUser(char prompt[], char s[], int maxStrLength) {
    int i = 0;
    char c;
    printf("%s --> ", prompt);
    while(i < maxStrLength && (c = (char) getchar()) != '\n')
        s[i++] = c;
    s[i] = '\0';
}

/* Function to call when printing an empty music library. */
void printMusicLibraryEmpty(){
    printf("\nThe music library is empty.\n");
}

/* Function to call to print a title when the entire music library is printed. */
void printMusicLibraryTitle(){
    printf("\nMy Personal Music Library: \n");
}

/*
 * A custom string compare function that compares the lowercase strings of the songs.
 * This is done to eliminate the possibility of error in ordering songs without capitalization.
*/
int customStrcmp(char *stringToCompare, char *stringToCompareWith){
    //creates temporary space to store the source and destination strings/arrays
    char *tempStringToCompare = (char*) malloc(sizeof(char)*(MAX_LENGTH + 1));
    char *tempStringToCompareWith = (char*) malloc(sizeof(char)*(MAX_LENGTH + 1));
    int i = 0, j = 0;
    while(stringToCompare[i] != '\0'){ //custom string copy function
        tempStringToCompare[i] = (char) tolower(stringToCompare[i]); //store the lowercase alphabet
        i++;
    }
    tempStringToCompare[i] = '\0';
    while(stringToCompareWith[j] != '\0'){
        tempStringToCompareWith[j] = (char) tolower(stringToCompareWith[j]); //store the lowercase alphabet
        j++;
    }
    tempStringToCompareWith[i] = '\0';
    int compare = strcmp(tempStringToCompare, tempStringToCompareWith);
    free(tempStringToCompare); //free the temporary memory
    free(tempStringToCompareWith);
    return compare; //returns the integer resulting from comparing the lower case version of two the strings.
}

/* Sorting the library array in ascending alphabetical order, by artist */
void cocktailSort(Song library[], int size){ //Bubble sort both ways
    bool goingUp = true;
    bool sorted = false;
    bool shouldSwap;
    int bottom = 0, top = size - 1, i;
    while(bottom < top && !sorted){
        sorted = true;
        if(goingUp){
            for(i = bottom; i < top; i++){
                if(customStrcmp(library[i].artist, library[i+1].artist) > 0){
                    shouldSwap = true;
                } else if(customStrcmp(library[i].artist, library[i+1].artist) == 0){
                    if(customStrcmp(library[i].songName, library[i+1].songName) > 0){
                        shouldSwap = true;
                    } else shouldSwap = false;
                } else shouldSwap = false;
                if(shouldSwap){
                    Song temp = library[i];
                    library[i] = library[i+1];
                    library[i+1] = temp;
                    sorted = false;
                }
            }
            top--;
            goingUp = false;
        } else{
            for(i = top; i > bottom; i--){
                if(customStrcmp(library[i].artist, library[i-1].artist) < 0){
                    shouldSwap = true;
                } else if(customStrcmp(library[i].artist, library[i-1].artist) == 0){
                    if(customStrcmp(library[i].songName, library[i-1].songName) < 0){
                        shouldSwap = true;
                    } else shouldSwap = false;
                } else shouldSwap = false;
                if(shouldSwap){
                    Song temp = library[i-1];
                    library[i-1] = library[i];
                    library[i] = temp;
                    sorted = false;
                }
            }
            bottom++;
            goingUp = true;
        }
    }
}

/* Printing the entire music library */
void printLibrary(Song library[], int librarySize){
    if(librarySize){
        printMusicLibraryTitle();
        for(int i = 0; i < librarySize; i++)
            printf("\n%s\n%s\n%s\n", library[i].songName, library[i].artist, library[i].genre);
    }
    else printMusicLibraryEmpty();
}

int main(void){
    Song library[MAX_LIB_SIZE]; //static declaration
    int librarySize = 0;  //number of elements stored in the library
    printf("Personal Music Library.\n\n"); //Announce the start of the program
    printf("Commands are I (insert), S (sort by artist), P (print), Q (quit).\n");
    char response;
    char input[MAX_LENGTH + 1];
    do{
        inputStringFromUser("\nCommand", input, MAX_LENGTH);
        response = (char) toupper(input[0]); // Response is the first character entered by user.
        if(response == 'I'){ //Insert a song into the array.
            char *promptName = "Song name";
            char *promptArtist =  "Artist";
            char *promptGenre = "Genre";
            inputStringFromUser(promptName, library[librarySize].songName, MAX_LENGTH);
            inputStringFromUser(promptArtist, library[librarySize].artist, MAX_LENGTH);
            inputStringFromUser(promptGenre, library[librarySize].genre, MAX_LENGTH);
            librarySize++; //increment the library size by one
        } else if(response == 'P'){ //Print the music library.
            printLibrary(library, librarySize);
        } else if(response == 'S'){
            cocktailSort(library, librarySize); //Sort the library in ascending order
            printLibrary(library, librarySize); //Print the sorted music library
        } else if(response == 'Q'){
            ; //do nothing
        } else{ //do this if no command matched ...
            printf("\nInvalid command.\n");
        }
    } while(response != 'Q');
    return 0;
}
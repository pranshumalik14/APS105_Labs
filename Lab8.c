#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#define OOM 42 //Defining the Out-Of-Memory (OOM) exit error handling constant
#define MAX_LENGTH 1024 //Defining the maximum character length of a string

typedef struct node{
    char *artist;
    char *songName;
    char *genre;
    struct node *nextNode;
} Node;

/* Function Prototypes */
void inputStringFromUser(char[], char[], int);
void songNameDuplicate(char[]);
void songNameFound(char[]);
void songNameNotFound(char[]);
void songNameDeleted(char[]);
void printMusicLibraryEmpty();
void printMusicLibraryTitle();
Node *insertIntoOrderedList(Node*, char[], char[], char[]);
Node *newNode (char[], char[], char[], Node*);
void printMusicLibrary(Node*);
bool search(Node*, char[], int);
Node *delete(Node*, char[]);
Node *deleteFirstNode(Node*);
Node *deleteMusicLibrary(Node*);
int customStrcmp(char*, char*);

/* Prompt the user for a string safely, without buffer overflow */
void inputStringFromUser(char prompt[], char s[], int maxStrLength){
    int i = 0;
    char c;
    printf("%s --> ", prompt);
    while(i < maxStrLength && (c = (char) getchar()) != '\n')
        s[i++] = c ;
    s[i] = '\0' ;
}

/*
 * Function to call when the user is trying to insert a song name
 * that is already in the personal music library.
*/
void songNameDuplicate(char songName[]){
    printf("\nA song with the name '%s' is already in the music library.\n"
           "No new song entered.\n", songName);
}

/* Function to call when a song name was found in the personal music library. */
void songNameFound(char songName[]){
    printf("\nThe song name '%s' was found in the music library.\n", songName) ;
}

/* Function to call when a song name was not found in the personal music library. */
void songNameNotFound(char songName[]){
    printf( "\nThe song name '%s' was not found in the music library.\n", songName);
}

/*
 * Function to call when a song name that is to be deleted
 * was found in the personal music library.
*/
void songNameDeleted(char songName[]){
    printf( "\nDeleting a song with name '%s' from the music library.\n", songName);
}

/* Function to call when printing an empty music library. */
void printMusicLibraryEmpty(void){
    printf( "\nThe music library is empty.\n" ) ;
}

/* Function to call to print a title when the entire music library is printed. */
void printMusicLibraryTitle(void){
    printf( "\nMy Personal Music Library: \n" ) ;
}

/* Function to allocate a new node and initialize it. */
Node *newNode(char artist[], char songName[], char genre[], Node *nextNode){
    Node *np = (Node*)malloc(sizeof(Node));
    if(np == NULL){ //ran out of memory
        printf("Error. Out of memory.\n);
        exit(OOM); //exits with the custom-defined OOM constant
    }
    np->artist = artist;
    np->songName = songName;
    np->genre = genre;
    np->nextNode = nextNode;
    return np;
}

/* Inserts the new song into the linked list in an alphabetically-ordered fashion */
Node *insertIntoOrderedList(Node *head, char artist[], char songName[], char genre[]){
    if(head == NULL) return newNode(artist, songName, genre, NULL);
    if(customStrcmp(songName, head->songName) < 0) return newNode(artist, songName, genre, head); //newNode goes up head;
    else if(customStrcmp(songName, head->songName) == 0){
        songNameDuplicate(songName);
        return head;
    }
    Node *np = head;
    while((np->nextNode != NULL) && (customStrcmp(songName, np->nextNode->songName) > 0)) np = np->nextNode;
    if((np->nextNode != NULL) && customStrcmp(songName, np->nextNode->songName) == 0) songNameDuplicate(songName);
    else np->nextNode = newNode(artist, songName, genre, np->nextNode); //data should be inserted right after *np

    return head;
}

/* Prints all the nodes in the Library */
void printMusicLibrary(Node *head){
    Node *current = head;
    if(head == NULL){
        printMusicLibraryEmpty();
    } else{
        printMusicLibraryTitle();
        while (current != NULL) {
            printf("\n%s\n%s\n%s\n", current->songName, current->artist, current->genre);
            current = current->nextNode;
        }
    }
}

/* Searches for the song among all the nodes in the linked list */
bool search(Node *head, char searchKey[], int utility){
    Node *current = head;
    while(current != NULL){
        if(!strcmp(current->songName, searchKey)){
            if(utility == 1){ //Utility is passed as '1' in if messages are to be printed.
                songNameFound(searchKey);
                printf("\n%s\n%s\n%s\n", current->songName, current->artist, current->genre);
            }
            return true;
        }
        current = current->nextNode;
    }
    if(utility == 1) songNameNotFound(searchKey);
    return false;
}

/* Deletes the first node */
Node *deleteFirstNode(Node *head){
    if (head != NULL){
        Node *newHead = head->nextNode;
        songNameDeleted(head->songName);
        free(head->songName);
        free(head->artist);
        free(head->genre);
        free(head);
        return newHead;
    }
    return head;
}

/* Deletes the node after searching for the song */
Node *delete(Node *head, char songToDelete[]){ //returns ptr to head of list
    if(head == NULL) return NULL;
    if(!strcmp(head->songName, songToDelete)) //delete 1st node
        return deleteFirstNode(head); //Search through the list
    Node *np = head;
    while((np->nextNode != NULL) && strcmp(np->nextNode->songName, songToDelete))
        np = np->nextNode;
    if(np->nextNode == NULL) //valToDelete not in list; nothing to delete
        return head;
    Node *nodeToRemove = np->nextNode; //want to delete np->link
    np->nextNode = np->nextNode->nextNode;
    songNameDeleted(songToDelete);
    free(nodeToRemove->songName);
    free(nodeToRemove->artist);
    free(nodeToRemove->genre);
    free(nodeToRemove);
    return head;
}

/* Deletes the entire music library */
Node* deleteMusicLibrary(Node *head){
    while(head != NULL){
        Node *firstNode = head;
        head = head->nextNode;
        songNameDeleted(firstNode->songName);
        free(firstNode->songName);
        free(firstNode->artist);
        free(firstNode->genre);
        free(firstNode);
    }
    return NULL;
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

int main(void){
    Node *head = NULL; // Declare the head of the linked list.
    printf("Personal Music Library.\n\n" ); // Announce the start of the program
    printf("%s", "Commands are I (insert), D (delete), S (search by song name),\n"
                 "P (print), Q (quit).\n");
    char response;
    char input[MAX_LENGTH + 1];
    do {
        inputStringFromUser("\nCommand", input, MAX_LENGTH); // Response is the first character entered by user.
        response = (char) toupper(input[0]); // Convert to uppercase to simplify later comparisons.
        if(response == 'I'){
            char *songName = (char*) malloc(sizeof(char)*(MAX_LENGTH + 1));
            char *artist = (char*) malloc(sizeof(char)*(MAX_LENGTH + 1));
            char *genre = (char*) malloc(sizeof(char)*(MAX_LENGTH + 1));
            inputStringFromUser("Song name", songName, MAX_LENGTH);
            inputStringFromUser("Artist", artist, MAX_LENGTH);
            inputStringFromUser("Genre", genre, MAX_LENGTH);
            head = insertIntoOrderedList(head, artist, songName, genre);
        } else if(response == 'D'){ // Delete a song from the list.
            char *prompt = "\nEnter the name of the song to be deleted";
            char *searchKey = (char*) malloc(sizeof(char)*(MAX_LENGTH + 1)); //Allocate temporary memory for searchKey
            inputStringFromUser(prompt, searchKey, MAX_LENGTH);
            if(search(head, searchKey, 0)) head = delete(head, searchKey); //Update the head after deleting the song
            else songNameNotFound(searchKey);
            free(searchKey); //Free the temporary memory allocated for searchKey
        } else if(response == 'S'){ // Search for a song by its name.
            char *prompt = "\nEnter the name of the song to search for";
            char *searchKey = (char*) malloc(sizeof(char)*(MAX_LENGTH + 1)); //Allocate temporary memory for searchKey
            inputStringFromUser(prompt, searchKey, MAX_LENGTH);
            search(head, searchKey, 1); //Initiate search for the song
            free(searchKey); //Free the temporary memory allocated for searchKey
        } else if(response == 'P'){
            printMusicLibrary(head); //Prints the entire linked list
        } else if(response == 'Q'){
            ; //do nothing
        } else{ // do this if no command matched...
            printf("\nInvalid command.\n");
        }
    } while(response != 'Q');
    head = deleteMusicLibrary(head); // Delete the entire linked list.
    printMusicLibrary(head); // Print the linked list to confirm deletion.
    return 0;
}
/**************************************************************************
 * Assessment Title: Simple Shell
 *
 * Number of Submitted C Files: 8
 *
 * Date: 18th March 2019
 *
 * 
 * Author: Maria Elizabeth Hughes 201716768
*		 Paul Hutchinson
*		David  McFadyen
*		Mohammed Adil Ali Akbar
*
 *
 *
 * Personal Statement: I confirm that this submission is all my own work.
 *
 *          (Signed)_Maria E Hughes
 *
 * 
 *
 **************************************************************************/



#define _GNU_SOURCE

#include "main.h"

/* Include Statements */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "history.h"
#include "alias.h"
#include "execute.h"

/* Prototypes */
char* buildPrefix(char* directory);

/* Main Function */
int main() {

    char currentDir[PATHSIZE]; // Current Directory the user is in
    char instruction[MAX_INSTR]; // Pre-parsed instruction
    char **phrase; // Array of components of the instruction
    char originalPath[500]; // User's Input Path
    char *history[MAX_HISTORY_SIZE]; // Instructions the user has entered
    char *alias[MAX_ALIAS_SIZE][2] = {{""}};
    int rear = 0; // A constant used in the history function.
    int counter = 0;

    /* Stage 1: Prompt user, and Read and parse user input, Exit shell and Initialise the
       working directory. */

    // Initialises and Allocates Memory
    for (int i = 0; i < MAX_HISTORY_SIZE; i++) {
        history[i] = (char * ) malloc(512);
        strcpy(history[i], "\0");
    }

    // Uses the Default Home Directory as the Default Path.
    strcpy(currentDir, getenv("HOME"));
    strcpy(originalPath, getenv("PATH"));

    // Sets Current Directory to Current Directory.
    chdir(currentDir);

    // Reads History from File.
    readHistory(history, &rear);
    readAliases(alias);
  
    for(;;) {
        getcwd(currentDir, sizeof(currentDir));
        printf("%s",buildPrefix(currentDir));

        // Get user and input and get rid of trailing control character inserted by fgets.
        if (fgets(instruction, sizeof instruction, stdin)==NULL){
            printf("\n");
            exitProgram(0, originalPath, history, rear, alias);
        }

        // This changes the '\n' to '\0'.
        size_t len = strlen(instruction);
        if (len && (instruction[len-1] == '\n')) {
            instruction[len-1] = '\0';
        }
     

        invokeAlias(instruction, alias);
        // Run the given command
        phrase = parseInput(instruction);

        // Put instruction into history and get new rear
        if (phrase[0]!=NULL && strcmp(phrase[0], "!")!=0) {
            rear = addToHistory(history, rear, phrase);
        }

        // Calls the executeInstruction function for each instruction to be executed.
        executeInstruction(phrase, history, rear, originalPath, alias,counter);

        memset(instruction,0,strlen(instruction));
    }
}


/* This function builds the string propts the user for input also
 * allowing the user to see what directory they are currently in
 */
char* buildPrefix(char* directory) {
    static char prefix[PATHSIZE];
    char *name, *temp = malloc(PATHSIZE);
    static char newDir[PATHSIZE];

    // Changes "/home/<username>" to the tilde key (~) if the Current Directory is the Home Directory
    if (strstr(directory, getenv("HOME"))) {

        newDir[0] = '~'; newDir[1]='\0';
        strcpy(temp, directory+strlen(getenv("HOME")));
        strcat(newDir, temp);

    }
	
	// Otherwise, a new User Directory will be created.
    else {
        strcpy(newDir, directory);
    }
  
    strcpy(prefix, "\x1b[34m"); // Blue   
    strcat(prefix, getenv("USER"));
    strcat(prefix, ":");
    strcat(prefix, "\x1b[32m"); // Green
    strcat(prefix, newDir);
    strcat(prefix, "\x1b[0m"); // Reset Colour
    strcat(prefix, "$ ");

    free(temp);
    return prefix;
}


/* Stage 4: Changing Directories. */
/* Called to chang the directory the user is in to a specified 
 * location
 */
void changeDirectory(char **arguments) {

    char *firstArgument = arguments[1];
    
    // This will check that there is not an invalid number of arguments, printing an error message if there is.
    if (arguments[1]!=NULL &&  arguments[2] != NULL) {
        printf("Error: CD: Too many arguments\n");
        return;
    }

    // If there is no argument entered, it will act as the home directory link (if "cd" is entered on it's own).
    if (firstArgument == NULL) {
        chdir(getenv("HOME"));
    }

    else {
        // The . represents the current directory.
        if (strcmp(".", firstArgument) == 0) {
            chdir(".");
        }
        // The .. represents the parent directory.
        else if(strcmp("..", firstArgument) == 0) {
            chdir("..");
        }
        else {
            // This will use the perror() function if there is no such directory.
            if(chdir(firstArgument) == -1) {
                perror(firstArgument);
            }
            
        }
    }
}

/* Separate the users instruction into an array of actionable components.
 */
char** parseInput(char* instruction) {
    char specialChar[] = "!|><&;\"";
    int counter=0, wordIdx=0, letterIdx=0;
    static char *phrase[MAX_INSTR/2];
	
	// Places item into an array of components passed to the main function.
    while (counter<strlen(instruction)&&counter<MAX_INSTR){

        // If the character is not a special character...
        if (strchr(specialChar, instruction[counter])==NULL) {

            // ...then add it to a member of the phrase.
            if (instruction[counter]!=' '&&instruction[counter]!='\t') {

                if (letterIdx == 0) {
                    phrase[wordIdx]= (char *) malloc(100);
                }

                phrase[wordIdx][letterIdx++]=instruction[counter];


                if (instruction[counter+1]==' '||instruction[counter+1]=='\0'||
                    instruction[counter+1]=='\t') {
                    phrase[wordIdx][letterIdx]='\0';
                    wordIdx++;
                    letterIdx=0;
                }
            }
        }

		// Also allows for characters to be inserted in the middle of tokens and for them still to be parsed correctly.
        else {

            // Add the special character to the phrase
            if (letterIdx!=0) {
                phrase[wordIdx][letterIdx]='\0';
                wordIdx++;
                letterIdx=0;
            }
            phrase[wordIdx]=malloc(2);
            phrase[wordIdx][0]=instruction[counter];
            phrase[wordIdx][1]='\0';
            wordIdx++;
        }
        counter++;
    }
    // Set the value after the last phrase to NULL
    phrase[wordIdx]=NULL;
    return phrase;
}


/* Saves history and alias and returns the PATH variable before
 * eziting the program
 */
void exitProgram(int exitCode, char *originalPath, char **history, int rear, char *(*alias)[2]) {
    // Reset the path to what it was before the session was opened
    setenv("PATH", originalPath, 1);
    printf("Reset path to:%s\n", originalPath);
    writeHistory(history, rear);
    writeAliases(alias);
    exit(exitCode);
}

#define _GNU_SOURCE

/* Include Statements */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "phrase.h"
#include "history.h"
#include "alias.h"
#include "execute.h"
#include "main.h"
#include "path.h"

/* Definitions */
#define PATHSIZE 200        // Maximum number of chars in currentDirectory

/* Prototypes */
char* buildPrefix(char* directory);

/* Main Function */
int main() {

    char currentDir[PATHSIZE]; // Current Directory the user is in
    char instruction[MAX_INSTR]; // Pre-parsed instruction
    char **phrase; // Array of components of the instruction
    char originalPath[500]; // User's Input Path
    char *history[MAX_HISTORY_SIZE]; // Instructions the user has entered
    char *alias[MAX_ALIAS_SIZE][2] = {{NULL}};
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


/* Stage 3: Setting the current directory to Home. */

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
	
void changeDirectory(char **arguments) {

    char *firstArgument = arguments[1];

    // This will check that there is not an invalid number of arguments, printing an error message if there is.
    if (arguments[2] != NULL) {
        printf("Error: Too many arguments\n");
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

/* Ran on the way out */
void exitProgram(int exitCode, char *originalPath, char **history, int rear, char *(*alias)[2]) {
    // Reset the path to what it was before the session was opened
    setenv("PATH", originalPath, 1);
    writeHistory(history, rear);
    writeAliases(alias);
    exit(exitCode);
}

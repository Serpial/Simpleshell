/* Include Statements */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/* Definitions */
#define PATHSIZE 200        // Maximum number of chars in currentDirectory
#define MAX_INSTR 512       // Maximum number of chars per phrase
#define MAX_HISTORY_SIZE 20 // Maximum number of instructions stored

/* Prototypes */
char* buildPrefix(char* directory);
char** parseInput(char* instruction);
char** joinSubPhrase (char **phrase);
void executeExternal(char **phrase);
void getPath(char **phrase);
void setPath(char **phrase);
void changeDirectory(char **arguments);
void exitProgram(int exitCode, char originalPath[500], char **history, int rear);
void writeHistory(char **history, int rear);
void readHistory(char **history, int *rear);
void printHistory(char **history, int rear);
void executeInstruction (char **phrase, char **history, int rear, char originalPath[500]);
void recallHistory (char **phrase, char **history, int rear, char originalPath[500]);


/* Main Function */
int main() {

char currentDir[PATHSIZE]; // Current Directory the user is in
char instruction[MAX_INSTR]; // Pre-parsed instruction
char **phrase; // Array of components of the instruction
char originalPath[500]; // User's Input Path
char *history[MAX_HISTORY_SIZE]; // Instructions the user has entered
int rear = 0; // A constant used in the history function.


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

    for(;;) {
        getcwd(currentDir, sizeof(currentDir));
        printf("%s",buildPrefix(currentDir));

        // Get user and input and get rid of trailing control character inserted by fgets.
        if (fgets(instruction, sizeof instruction, stdin)==NULL){
            printf("\n");
            exitProgram(0, originalPath, history, rear);
        }

        // This changes the '\n' to '\0'.
        size_t len = strlen(instruction);
        if (len && (instruction[len-1] == '\n')) {
            instruction[len-1] = '\0';
        }

        // Run the given command.
        phrase = parseInput(instruction);

        // Put instruction into history if it isn't a 'fetch from history' command.
        if (phrase[0]!=NULL && strcmp(phrase[0], "!")!=0) {
            strcpy(history[rear], instruction);
            rear = (1+rear) % MAX_HISTORY_SIZE;
        }

        // Calls the executeInstruction function for each instruction to be executed.
        executeInstruction(phrase, history, rear, originalPath);
        memset(instruction,0,strlen(instruction));
    }
}

/* Stage 2: Execute external commands. */

void executeInstruction (char **phrase, char **history, int rear, char originalPath[500]) {

    // Each instruction will be executed if the input is not null.
    if (phrase[0]!=NULL) {

        // getPath.
        if (strcmp(phrase[0], "getpath") == 0) {
            getPath(phrase);
        }
        // setPath.
        else if (strcmp(phrase[0], "setpath") == 0) {
            setPath(phrase);
        }
        // ! (used in history).
        else if (strcmp(phrase[0], "!") == 0) {
            recallHistory(phrase, history, rear, originalPath);
        }
        // history.
        else if (strcmp(phrase[0], "history") == 0) {
            printHistory(history, rear);
        }
        // cd (change directory).
        else if (strcmp(phrase[0], "cd") == 0) {
            changeDirectory(phrase);
        }
        // exit.
        else if (strcmp(phrase[0], "exit") == 0) {
            exitProgram(0, originalPath, history, rear);
        }
        // if the command is not pre-defined.
        else {
            executeExternal(phrase);
        }
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

    strcpy(prefix, getenv("USER"));
    strcat(prefix, ":");
    strcat(prefix, newDir);
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

	
 // Separate the users instruction into an array of actionable components.
char** parseInput(char* instruction) {
    char specialChar[] = "!|><&;\"";
    int counter=0, wordIdx=0, letterIdx=0;
    static char *phrase[MAX_INSTR/2];
	
	// Places item into an array of components passed to the main function.
    while (counter<strlen(instruction)&&counter<MAX_INSTR){

        // If the character is not a special character...
        if (strchr(specialChar, instruction[counter])==NULL) {

            // ...then add it to a member of the phrase.
            if (instruction[counter]!=' ') {

                if (letterIdx == 0) {
                    phrase[wordIdx]= (char *) malloc(100);
                }

                phrase[wordIdx][letterIdx++]=instruction[counter];


                if (instruction[counter+1]==' '||instruction[counter+1]=='\0') {
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

// Duplicates process
void executeExternal(char **phrase){
    pid_t pid;
    pid = fork();

    if (pid == -1){ // fork is less than zero and so an error has occured
        perror("Error has occurred, fork has failed");
    }

    else if (pid == 0){ // in the child and so can execute the command
        // use execvp to execute the command and detect errors
        if (execvp(phrase[0], phrase)== -1){
            printf(" %s: we dont recognise this command\n", phrase[0]);
            _exit(1);
            // perror("We did not recognise this command");
        }
    }
    else { // in the parent process
        int status;
        waitpid(pid, &status,0);
    }
}

/* Stage 3: Setting the Path */

// Allows the user to SET the path environment variable.
void setPath(char **phrase) {
    char tempPath[500];

    if (phrase[2]!=NULL){
        printf("Too many arguments\n");
    } else if (phrase[1]==NULL){
        printf("Too few arguments\n");
    } else {
        strcpy(tempPath, phrase[1]);
        strcat(tempPath, ":");
        strcat(tempPath, getenv("PATH"));
        setenv("PATH", tempPath, 1);
    }
}

/* Stage 3: Getting the Path */

// Allow the user to GET the path environment variable.
void getPath(char **phrase){
    if (phrase[1]!=NULL){
        printf("Too many arguments\n");
    } else {
        printf("%s\n", getenv("PATH"));
    }
}


int howMany(char **history) {
    int counter=0;
    int index=0;
    while(strcmp(history[index], "\0") && index!=MAX_HISTORY_SIZE+1) {
        counter++;
        index++;
    }
    return counter;
}

/* Stage 6: Persistent history. */

void recallHistory (char **phrase, char **history, int rear, char originalPath[500]) {
    int lineNum=0;

    if (phrase[1]!=NULL) {
        if (strcmp(phrase[1],"!")==0 && phrase[2]==NULL) {

            lineNum = (rear==0?MAX_HISTORY_SIZE-1:rear-1);
            printf("%s\n", history[lineNum]);
            phrase = parseInput(history[lineNum]);
            executeInstruction(phrase, history, rear, originalPath);
            return;


        } else {
            lineNum = strtol(phrase[1],NULL,0);
            if (lineNum!=0 && lineNum<=MAX_HISTORY_SIZE && lineNum>=-MAX_HISTORY_SIZE) {
                if (lineNum<0) {  // go down from rear


                    lineNum = (rear+lineNum)<0?
                        (MAX_HISTORY_SIZE-1)+(rear+lineNum): lineNum+rear;

                    printf("%s\n", history[lineNum]);
                    phrase = parseInput(history[lineNum]);
                    executeInstruction(phrase, history, rear, originalPath);
                    return;
                } else {  // pick a spot
                    if (howMany(history)==20) {
                        lineNum = (rear+lineNum-1)%MAX_HISTORY_SIZE;
                    } else {
                        lineNum--;
                    }

                    printf("%s\n", history[lineNum]);
                    phrase = parseInput(history[lineNum]);
                    executeInstruction(phrase, history, rear, originalPath);
                    return;
                }
            } else {
                printf("Incorrect Entry\n");
            }
        }
    }
    if (phrase[2]!=NULL) {
        printf("You have entered too many arguments\n");
        return;
    }
}

/*Stage 5: Adding commands to history, invoking commands from history and
printing the history. */

void writeHistory(char **history, int rear) {
    FILE *fp;
    int i;
    char fileLocation[MAX_INSTR]="";

    strcpy(fileLocation, getenv("HOME"));
    strcat(fileLocation, "/.hist_list");
    fp= fopen(fileLocation,"w");

    if (fp == NULL){
        printf("Could not open history file\n");
        return;
    }

    i=rear;
    do {
        if (strcmp(history[i], "\0")!=0){
            fprintf(fp,"%s\n", history[i]);
        }
        i=(i+1)%MAX_HISTORY_SIZE;
    } while (i!=rear);
}

void readHistory(char **history, int *rear){
    FILE *fp;
    int numInstr=0;
    char newInstruction[MAX_INSTR]="";
    char fileLocation[MAX_INSTR]="";
    size_t len;

    strcpy(fileLocation, getenv("HOME"));
    strcat(fileLocation, "/.hist_list");

    fp = fopen(fileLocation,"a+");

    while(fgets(newInstruction, sizeof(newInstruction), fp)!=NULL && numInstr<20) {
        len = strlen(newInstruction);
        if (len && (newInstruction[len-1] == '\n')) {
            newInstruction[len-1] = '\0';
        }
        strcpy(history[numInstr++], newInstruction);
    }
    *rear = numInstr==MAX_HISTORY_SIZE?0:numInstr;
    fclose(fp);
}

/*Prints 20 elements of history to the user.*/
void printHistory(char **history, int rear){
    //set i to be a copy of rear to read in from most recent
    int i = rear;
    int historyIndex=1;
    do {
        //only show elements in array that aren't null
        if (strcmp(history[i], "\0") != 0)
            printf("%i. %s \n", historyIndex++, history[i]); //print history
        i = (1+i)%MAX_HISTORY_SIZE; //update i to ensure its circular
    }
    while ( i != rear);
}

/* Ran on the way out */
void exitProgram(int exitCode, char originalPath[500], char **history, int rear) {
    // Reset the path to what it was before the session was opened
    setenv("PATH", originalPath, 1);
    writeHistory(history, rear);
    exit(exitCode);
}


/* takes instances of quotation marks and joins them in to phrases
 */
char** joinSubPhrase (char **phrase) {
    static char *newPhrase[MAX_INSTR/2];
    int itemIndex=0;

    for (int i=0; phrase[i]!= NULL; i++) {
        if (strcmp(phrase[i], "\"")==0) {
            if (phrase[i+1]!=NULL){
                newPhrase[itemIndex]=malloc(200);
                strcpy(newPhrase[itemIndex], phrase[++i]);
                for (int j=i+1; phrase[j]!=NULL; j++) {
                    if (strcmp(phrase[j],"\"")==0) {
                        i=j;
                        itemIndex++;
                        break;
                    } else {
                        strcat(newPhrase[itemIndex], " ");
                        strcat(newPhrase[itemIndex], phrase[j]);
                    }
                    if (phrase[j+1]==NULL) { return NULL; }
                }
            } else { return NULL; }
        } else {
            newPhrase[itemIndex]=malloc(200);
            strcpy(newPhrase[itemIndex], phrase[i]);
            itemIndex++;
        }
    }

    newPhrase[itemIndex]=NULL;
    return newPhrase;
}

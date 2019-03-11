#define _GNU_SOURCE

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
#define MAX_ALIAS_SIZE 20

/* Prototypes */
char* buildPrefix(char* directory);
char** parseInput(char* instruction);
char** joinSubPhrase (char **phrase);
void executeExternal(char **phrase);
void getPath(char **phrase);
void setPath(char **phrase);
void changeDirectory(char **arguments);
void exitProgram(int exitCode, char originalPath[500], char **history, int rear, char *alias[MAX_ALIAS_SIZE][2]);
void writeHistory(char **history, int rear);
void readHistory(char **history, int *rear);
void printHistory(char **history, int rear);
void executeInstruction (char **phrase, char **history, int rear, char originalPath[500], char *alias[MAX_ALIAS_SIZE][2]);
void recallHistory (char **phrase, char **history, int rear, char originalPath[500], char *alias[MAX_ALIAS_SIZE][2]);
void printAlias(char *alias[MAX_ALIAS_SIZE][2]);
void addAlias(char**phrase, char *alias[MAX_ALIAS_SIZE][2]);
void removeAlias(char **phrase, char *alias[MAX_ALIAS_SIZE][2]);
void invokeAlias(char instruction[MAX_INSTR], char *alias[MAX_ALIAS_SIZE][2]);
void readAliases (char *alias[MAX_ALIAS_SIZE][2]);
void writeAliases(char *alias[MAX_ALIAS_SIZE][2]);
void substituteAlias(char*alias[MAX_ALIAS_SIZE][2]);

/* Main Function */
int main() {

    char currentDir[PATHSIZE]; // Current Directory the user is in
    char instruction[MAX_INSTR]; // Pre-parsed instruction
    char **phrase; // Array of components of the instruction
    char originalPath[500]; // User's Input Path
    char *history[MAX_HISTORY_SIZE]; // Instructions the user has entered
    char *alias[MAX_ALIAS_SIZE][2] = {{NULL}};
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

        // Put instruction into history if it isn't a 'fetch from history' command.
        if (phrase[0]!=NULL && strcmp(phrase[0], "!")!=0) {
            strcpy(history[rear], instruction);
            rear = (1+rear) % MAX_HISTORY_SIZE;
        }

        // Calls the executeInstruction function for each instruction to be executed.
        executeInstruction(phrase, history, rear, originalPath, alias);
        memset(instruction,0,strlen(instruction));
    }
}

/* Stage 2: Execute external commands. */

void executeInstruction (char **phrase, char **history, int rear, char originalPath[500], char *alias[MAX_ALIAS_SIZE][2]) {
    int i=1;
    // Each instruction will be executed if the input is not null.
    if (phrase[0]!=NULL){
        if (strcmp(phrase[0], "getpath")==0) { // getPath.
            getPath(phrase);
        } else if (strcmp(phrase[0], "setpath") == 0) { // setPath.
            setPath(phrase);
        } else if (strcmp(phrase[0], "!")==0) {  // ! (used in history).
            recallHistory(phrase, history, rear, originalPath, alias);      
        } else if (strcmp(phrase[0], "history")==0) { // history.
            printHistory(history, rear);
        } else if (strcmp(phrase[0], "cd")==0) { // cd (change directory).
            changeDirectory(joinSubPhrase(phrase));
        } else if (strcmp(phrase[0], "exit")==0) {
            exitProgram(0, originalPath, history, rear, alias);
        } else if((strcmp(phrase[0], "alias")==0) && phrase[1]==NULL){
            printAlias(alias);
        } else if((strcmp(phrase[0], "alias")==0) && phrase[1]!=NULL){
            addAlias(phrase, alias);
        } else if(strcmp(phrase[0], "unalias")==0){
            removeAlias(phrase, alias);
        } else { // if the command is not pre-defined.
                //substituteAlias(alias);
                executeExternal(joinSubPhrase(phrase));


        
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

void recallHistory (char **phrase, char **history, int rear, char originalPath[500], char *alias[MAX_ALIAS_SIZE][2]) {
    int lineNum=0;

    if (phrase[1]!=NULL) {
        if (strcmp(phrase[1],"!")==0 && phrase[2]==NULL) {

            lineNum = (rear==0?MAX_HISTORY_SIZE-1:rear-1);
            printf("%s\n", history[lineNum]);
            phrase = parseInput(history[lineNum]);
            executeInstruction(joinSubPhrase(phrase), history, rear, originalPath, alias);
            return;


        } else {
            lineNum = strtol(phrase[1],NULL,0);
            if (lineNum!=0 && lineNum<=MAX_HISTORY_SIZE && lineNum>=-MAX_HISTORY_SIZE) {
                if (lineNum<0) {  // go down from rear


                    lineNum = (rear+lineNum)<0?
                        (MAX_HISTORY_SIZE-1)+(rear+lineNum): lineNum+rear;

                    printf("%s\n", history[lineNum]);
                    phrase = parseInput(history[lineNum]);
                    executeInstruction(joinSubPhrase(phrase), history, rear, originalPath, alias);
                    return;
                } else {  // pick a spot
                    if (howMany(history)==20) {
                        lineNum = (rear+lineNum-1)%MAX_HISTORY_SIZE;
                    } else {
                        lineNum--;
                    }

                    printf("%s\n", history[lineNum]);
                    phrase = parseInput(history[lineNum]);
                    executeInstruction(joinSubPhrase(phrase), history, rear, originalPath, alias);
                    return;
                }
            } else {
                if (lineNum!=0) {
                    printf("Error: You have entered too large a value\n");
                } else {
                    printf("Error: Cannot Enter 0\n");
                }
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
void exitProgram(int exitCode, char originalPath[500], char **history, int rear, char *alias[MAX_ALIAS_SIZE][2]) {
    // Reset the path to what it was before the session was opened
    setenv("PATH", originalPath, 1);
    writeHistory(history, rear);
    writeAliases(alias);
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

void printAlias(char *alias[MAX_ALIAS_SIZE][2]){
    int index;
    int nullEntries=0;
    int counter =1;

    for (index =0; index <MAX_ALIAS_SIZE; index++){
        if (alias[index][0] == NULL){
            nullEntries++; 
        }
    }

    if (nullEntries == MAX_ALIAS_SIZE){
        printf("You dont have any aliases\n");
    }
    else{
        for(index =0; index< (MAX_ALIAS_SIZE - nullEntries); index++){
            printf("%i. %s %s \n", counter++, alias[index][0], alias[index][1]);
        }
    }
}

void addAlias(char**phrase, char *alias[MAX_ALIAS_SIZE][2]){
if (phrase[1] == NULL){
    printf("Not enough arguments\n");
    return;
}
else if(phrase[2] == NULL){
printf("Not enough arguments\n");
    return;
}


//char name[] = " ";
char command [512] = " ";
int index = 2;
int j;
int found = 0;
//find how mnany arguments there are 
while(phrase[index] != NULL){
 index++;
}

//makes command for alias
for (j =2; j<index; j++){
    strcat(command, phrase[j]);
    strcat(command, " ");
}

//counts null charatcers
int nullEntries=0;
 for (index =0; index <MAX_ALIAS_SIZE; index++){
    if (alias[index][0] == NULL){
        nullEntries++; 
    }
 }

 //checks list of previous alias 
 for (j =0; j<(MAX_ALIAS_SIZE-nullEntries); j++){
    if (strcmp(phrase[1], alias[j][0]) == 0){
        printf("overwriting previous alias\n");
        alias[j][1] = strdup(command);   
        found = 1;
    }
 }

 //checks if alias is full if not adds
 if(nullEntries == 0){
    printf("no more space for alias");
    //return;
    }
 else if (nullEntries != 0 && found == 0){
    alias[MAX_ALIAS_SIZE-nullEntries][0] =strdup(phrase[1]);
    alias[MAX_ALIAS_SIZE-nullEntries][1] =strdup(command);
 }

substituteAlias(alias);  
}


void removeAlias(char **phrase, char *alias[MAX_ALIAS_SIZE][2]){
    int found = 0;
    int index;
    int j;
    char name[512] = " ";

    if (phrase[1] == NULL){
        printf("Error: too few arguments\n");
        return;
    } else if (phrase[2] != NULL){
        printf("Error: too many arguments\n");
        return;
    }

    strcpy(name, phrase[1]);

    for (index=0; index< MAX_ALIAS_SIZE; index++){
        if (alias[index][0] != NULL){
            if (strcmp(name, alias[index][0]) == 0){
                alias[index][0] = NULL;
                alias[index][1] = NULL;
                found = 1;
                j = index;
            }
        }
    }

    if(found == 0){
        printf("This alias does not exits\n");
        return;
    }
    else{ //moves all elemnets in array up one
        while(j<(MAX_ALIAS_SIZE-1)){
            alias[j][0] = alias[j+1][0];
            alias[j][1] = alias[j+1][1];
            j++;
        } // make the last entry null
        alias[MAX_ALIAS_SIZE-1][0] = NULL;
        alias[MAX_ALIAS_SIZE-1][1] = NULL;
    }
    return;
}


void invokeAlias(char instruction[MAX_INSTR], char *alias[MAX_ALIAS_SIZE][2]){
    int index;
    for (index = 0; index<MAX_ALIAS_SIZE; index++){
        if (alias[index][0] != NULL){
            if (strcmp(instruction, alias[index][0]) == 0){
                strcpy(instruction, alias[index][1]);
            }
        }
    }
}

void readAliases (char *alias[MAX_ALIAS_SIZE][2]) {
    FILE *fp;
    char fileLocation[MAX_INSTR]="";
    char joinedAlias[MAX_INSTR]="", temp[MAX_INSTR];
    int numAlias=0, len, letterIndex, pastBar = 0;
    
    strcpy(fileLocation, getenv("HOME"));
    strcat(fileLocation, "/.aliases");
    fp = fopen(fileLocation, "a+");
    
    while(fgets(joinedAlias, sizeof(joinedAlias), fp)!=NULL && numAlias<MAX_ALIAS_SIZE) {
        len = strlen(joinedAlias);
        letterIndex=0;
        if (len && (joinedAlias[len-1]=='\n')) {
            joinedAlias[len-1] = '\0';   
        }

        // Prepend alias so that addAlias can be used
        if (strchr(joinedAlias, '|')!=NULL) {
            strcpy(temp, joinedAlias);
            strcpy(joinedAlias, "alias ");
            strcat(joinedAlias, temp);

            for (int i=0; i<strlen(joinedAlias); i++) {
                if (joinedAlias[i]=='|') {
                    joinedAlias[i]= ' ';
                    break;
                }
            }
            addAlias(parseInput(joinedAlias), alias);
        } else {
            printf("Error: Unable to add alias:%s\n", joinedAlias);
        }
    }
    fclose(fp);
}

void writeAliases(char *alias[MAX_ALIAS_SIZE][2]) {
    FILE *fp;
    char fileLocation[MAX_INSTR]="";
    int aliasCounter = 0;
    
    strcpy(fileLocation, getenv("HOME"));
    strcat(fileLocation, "/.aliases");
    fp = fopen(fileLocation, "w");

    if (fp == NULL) {
        printf("Could not open history file\n");
        return;
    }
    
    while (aliasCounter<MAX_ALIAS_SIZE && alias[aliasCounter][1]!=NULL) {
        fprintf(fp, "%s|%s\n", alias[aliasCounter][0], alias[aliasCounter][1]);
        aliasCounter++;
    }
}


void substituteAlias(char *alias[MAX_ALIAS_SIZE][2]){
int j = 0;
int index =0;
int k=0;
char value[512] = "";
char match[512] = "";
int nullEntries=0;
 for (k=0; k<MAX_ALIAS_SIZE; k++){
    if (alias[k][0] == NULL){
        nullEntries++; 
    }
 }
for (j=0; j<MAX_ALIAS_SIZE-nullEntries; j++){
        strcpy(value, alias[j][1]);
         for (index=0; index< MAX_ALIAS_SIZE-nullEntries; index++){
            if (strcmp(value, alias[index][0]) == 0){
                strcpy(match, alias[index][1]);
                //match = strdup(alias[index][1]);
                //strcpy(alias[j][1], match);
                alias[j][1] = strdup(match);
            
            }
        }
    }
}














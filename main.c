#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/* Definitions */
#define PATHSIZE 200        // Maximum number of chars in currentDir
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
void readAliases () {
    FILE *fp;
    char fileLocation[MAX_INSTR]="";
    char newAlias[MAX_INSTR]="";
    int numAlias=0, len;
    static char newAliasList[MAX_ALIAS_SIZE][2];
    
    strcpy(fileLocation, getenv("HOME"));
    strcat(fileLocation, "/.aliases");
    fp = fopen(fileLocation, "a+");

    while(fgets(newAlias, sizeof(newAlias), fp)!=NULL && numAlias<MAX_ALIAS_SIZE) {
        len = strlen(newAlias);
        if (len && (newAlias[len-1]=='\n')) {
            newAlias[len-1] = '\0';

            
            
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

/* Main Function */
int main() {
    char currentDir[PATHSIZE]; // Current Directory the user is in
    char instruction[MAX_INSTR]; // Pre-parsed instruction
    char **phrase; // Array of components of the instruction
    char originalPath[500];
    char *history[MAX_HISTORY_SIZE];
    char *alias[MAX_ALIAS_SIZE][2] = {{NULL}};
    int rear=0;

    //iniitialises and allocates memory
    for (int i = 0; i < MAX_HISTORY_SIZE; i++) {
        history[i] = (char * ) malloc(512);
        strcpy(history[i], "\0");
    }
  
    // Use the default home directory as the default path
    strcpy(currentDir, getenv("HOME"));
    strcpy(originalPath, getenv("PATH"));
    // Sets current directory to current diretory
    chdir(currentDir);
    // Reads history from file
    readHistory(history, &rear);
  
    for(;;) {
        getcwd(currentDir, sizeof(currentDir));
        printf("%s",buildPrefix(currentDir));
    
        // get user and input and get rid of trailing control characters
        //    inserted by fgets also exits on Ctrl-D
        if (fgets(instruction, sizeof instruction, stdin)==NULL){
            printf("\n");
            exitProgram(0, originalPath, history, rear, alias);
        }
        // fgets causes a \n to be placed on the back of input.
        size_t len = strlen(instruction);
        if (len && (instruction[len-1] == '\n')) {
            // This changes the '\n' to '\0
            instruction[len-1] = '\0';
        }

        invokeAlias(instruction, alias);
        // Run the given command
        phrase = parseInput(instruction);
    
        // Put instruction in to history if it isn't a fetch from history command
        if (phrase[0]!=NULL && strcmp(phrase[0], "!")!=0) {
            strcpy(history[rear], instruction);
            rear = (1+rear) % MAX_HISTORY_SIZE;
        }
        executeInstruction(phrase, history, rear, originalPath, alias);
        memset(instruction,0,strlen(instruction));
    }
}


void executeInstruction (char **phrase, char **history, int rear, char originalPath[500], char *alias[MAX_ALIAS_SIZE][2]) {

  
    if (phrase[0]!=NULL){
        if (strcmp(phrase[0], "getpath")==0) {
            getPath(phrase);
        } else if (strcmp(phrase[0], "setpath")==0) {
            setPath(phrase);
        } else if (strcmp(phrase[0], "!")==0) {
            recallHistory(phrase, history, rear, originalPath, alias);      
        } else if (strcmp(phrase[0], "history")==0) {
            printHistory(history, rear);
        } else if (strcmp(phrase[0], "cd")==0) {
            changeDirectory(phrase);
        } else if (strcmp(phrase[0], "exit")==0) {
            exitProgram(0, originalPath, history, rear, alias);
        } else if((strcmp(phrase[0], "alias")==0) && phrase[1]==NULL){
            printAlias(alias);
        }
        else if((strcmp(phrase[0], "alias")==0) && phrase[1]!=NULL){
            addAlias(phrase, alias);
        }
        else if(strcmp(phrase[0], "unalias")==0){
            removeAlias(phrase, alias);
        }

        else {
            executeExternal(phrase);
        }
    }
}

/* Try to get the user's username and add it to the
 * current directory the user is in
 */
char* buildPrefix(char* directory) {
    static char prefix[PATHSIZE];
    char *name, *temp = malloc(PATHSIZE);
    static char newDir[PATHSIZE];

    // switches "/home/<username>" to "~"
    if (strstr(directory, getenv("HOME"))) {
        newDir[0] = '~'; newDir[1]='\0';
        strcpy(temp, directory+strlen(getenv("HOME")));
        strcat(newDir, temp);
    } else {
        strcpy(newDir, directory);
    }
  
    strcpy(prefix, getenv("USER"));
    strcat(prefix, ":");
    strcat(prefix, newDir);
    strcat(prefix, "$ ");

    free(temp);
    return prefix;
}

/*
 * This is the function for cd (Change Directory) that I have created.
 */ 
void changeDirectory(char **arguments) {
    char *firstArgument = arguments[1];
  
    if (arguments[2] != NULL) { // This will check that there is not an invalid number of arguments, printing an error message if there is.
        printf("Invalid number of arguments\n");
        return;
    }
    if (firstArgument == NULL) { // If there is no argument entered, it will act as the home directory link (if "cd" is entered on it's own). This is the first form as specified in the ACE4 PDF.
        chdir(getenv("HOME"));
    } 
    else {  
        if (strcmp(".", firstArgument) == 0) { // The . represents the current directory.
            chdir(".");
        } else if(strcmp("..", firstArgument) == 0) { // The .. represents the parent directory.
            chdir("..");
        } else { 
            if(chdir(firstArgument) == -1) { // This will use the perror() function if there is no such directory.
                perror(firstArgument);
            }
        }
    }
}



/* Separate the users instruction into an array of actionable
 * components.
 * Places item into an array of components passed to main 
 * Also allows for characters to be inserted in the middle of
 * tokens and for them still to be parsed correctly
 */
char** parseInput(char* instruction) {
    char specialChar[] = "!|><&;\"";
    int counter=0, wordIdx=0, letterIdx=0;
    static char *phrase[MAX_INSTR/2];
  
    while (counter<strlen(instruction)&&counter<MAX_INSTR){
        // If the character is not a special character
        if (strchr(specialChar, instruction[counter])==NULL) {
            // then added it to a member of the phrase
            if (instruction[counter]!=' ') {
                if (letterIdx==0) {
                    phrase[wordIdx]= (char *) malloc(100);
                }
                phrase[wordIdx][letterIdx++]=instruction[counter];
                if (instruction[counter+1]==' '||instruction[counter+1]=='\0') {
                    phrase[wordIdx][letterIdx]='\0';
                    wordIdx++;
                    letterIdx=0;
                }
            }
        } else {
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

void executeExternal(char **phrase){
    pid_t pid;
    pid = fork(); //duplicates process 

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

/* Allow the user to set the path environment variable
 */
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

/* Allow the user to get the path environment variable
 */
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


void recallHistory (char **phrase, char **history, int rear, char originalPath[500], char *alias[MAX_ALIAS_SIZE][2]) {
    int lineNum=0;

    if (phrase[1]!=NULL) {
        if (strcmp(phrase[1],"!")==0 && phrase[2]==NULL) {
      
            lineNum = (rear==0?MAX_HISTORY_SIZE-1:rear-1);
            printf("%s\n", history[lineNum]);
            phrase = parseInput(history[lineNum]);
            executeInstruction(phrase, history, rear, originalPath, alias);
            return;

      
        } else {
            lineNum = strtol(phrase[1],NULL,0);
            if (lineNum!=0 && lineNum<=MAX_HISTORY_SIZE && lineNum>=-MAX_HISTORY_SIZE) {
                if (lineNum<0) {  // go down from rear
          
          
                    lineNum = (rear+lineNum)<0?
                        (MAX_HISTORY_SIZE-1)+(rear+lineNum): lineNum+rear;
         
                    printf("%s\n", history[lineNum]);
                    phrase = parseInput(history[lineNum]);
                    executeInstruction(phrase, history, rear, originalPath, alias);
                    return;
                } else {  // pick a spot          
                    if (howMany(history)==20) {
                        lineNum = (rear+lineNum-1)%MAX_HISTORY_SIZE;
                    } else {
                        lineNum--;
                    }
          
                    printf("%s\n", history[lineNum]);
                    phrase = parseInput(history[lineNum]);
                    executeInstruction(phrase, history, rear, originalPath, alias);
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

/*prints 20 elements of history to the user */
void printHistory(char **history, int rear){
    //set i to be a copy of rear to read in from most recent 
    int i = rear;
    int historyIndex=1;
    do {
        //only show elements in array that arent null
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
    char command [512] = " ";
    int index = 2;
    int j;
    int found = 0;
    
    if (phrase[1] == NULL){
        printf("Error: Too few arguments");
        return;
    }
    
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




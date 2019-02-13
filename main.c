#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/* Definitions */
#define PATHSIZE 200      // Maximum number of chars in currentDir
#define MAX_INSTR 512     // Maximum number of chars per phrase
#define MAX_HISTORY_SIZE 20//Maximum number of instructions stored 
/* Prototypes */

char* buildPrefix(char* directory);
void parseInput(char* instruction, char* phrase[MAX_INSTR/2]);
void emptyPhrase(char* phrase[MAX_INSTR/2]);
void executeExternal(char* phrase[MAX_INSTR/2]);
void getPath(char* phrase[MAX_INSTR/2]);
void setPath(char* phrase[MAX_INSTR/2]);
void changeDirectory(char **arguments);
void executeInstruction (char *phrase[MAX_INSTR/2], char* instruction,
                         char *history[21], int *rear);
void singleExclamation (char *phrase[MAX_INSTR/2], char* instruction,
                        char *history[21]);
void doubleExclamation (char *phrase[MAX_INSTR/2], char* instruction,
                        char *history[21]);
void writeHistory(char *instruction, char  *history[21], int *rear);
void readHistory(char  *history[21], int *rear);
void printHistory(char *history[21]);

/* Main Function */
int main() {
  char currentDir[PATHSIZE]; // Current Directory the user is in
  char instruction[MAX_INSTR]; // Pre-parsed instruction
  char *phrase[MAX_INSTR/2]; // Array of components of the instruction
  char originalPath[500];
  char *history[21];
  int *rear = 0;
  //char *histArray[MAX_HISTORY_SIZE];

  //iniitialises and allocates memory
  for (int i = 0; i <= MAX_HISTORY_SIZE; i++) {
    history[i] = (char * ) malloc(512);
    strcpy(history[i], "\0");
  }



  // Use the default home directory as the default path
  strcpy(currentDir, getenv("HOME"));
  strcpy(originalPath, getenv("PATH"));

  // Sets current directory to current diretory
  chdir(currentDir);

  for(;;) {
    memset(instruction,0,strlen(instruction));
    emptyPhrase(phrase);

    getcwd(currentDir, sizeof(currentDir));
    printf("%s",buildPrefix(currentDir));

    // get user and input and get rid of trailing control characters
    //    inserted by fgets also exits on Ctrl-D
    if (fgets(instruction, sizeof instruction, stdin)==NULL){
      printf("\n");
      exit(0);
    }
    // fgets causes a \n to be placed on the back of input.
    size_t len = strlen(instruction);
    if (len && (instruction[len-1] == '\n')) {
      // This changes the '\n' to '\0
      instruction[len-1] = '\0';
    }

    //add command to histroy 
    writeHistory(instruction, history, rear);


    // Run the given command
    executeInstruction(phrase, instruction, history, rear);
  }

  // Reset the path to what it was before the session was opened
  setenv("PATH", originalPath, 1);
}

void executeInstruction (char *phrase[MAX_INSTR/2], char* instruction,
                         char *history[21], int *rear) {

  parseInput(instruction, phrase);

  if (phrase[0]!=NULL){
    if (strcmp(phrase[0], "getpath")==0) {
      getPath(phrase);
    } else if (strcmp(phrase[0], "setpath")==0) {
      setPath(phrase);
    } else if (strcmp(instruction, "!!")==0) {
      doubleExclamation(phrase, instruction, history);
    } else if (strcmp(phrase[0], "!")==0) {
      singleExclamation(phrase, instruction, history);      
    } else if (strcmp(phrase[0], "history")==0) {
      readHistory(history, rear);
    } else if (strcmp(phrase[0], "cd")==0) {
      changeDirectory(phrase);
    } else if (strcmp(instruction, "exit")==0) {
      exit(0);
    } else {
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
void parseInput(char* instruction, char* phrase[MAX_INSTR/2]) {
  char specialChar[] = "!|><&;";
  int counter=0, wordIdx=0, letterIdx=0;
  
  while (counter<strlen(instruction)&&counter<MAX_INSTR){
    // If the character is not a special character
    if (strchr(specialChar, instruction[counter])==NULL) {
      // then added it to a member of the phrase
      if (instruction[counter]!=' ') {
        if (letterIdx==0) {
          phrase[wordIdx]=malloc(100);
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
}

void executeExternal(char* phrase[MAX_INSTR/2]){
  pid_t pid;
  pid = fork(); //duplicates process 

  if (pid == -1){ // fork is less than zero and so an error has occured
      perror("Error has occurred, fork has failed");
  }

  else if (pid == 0){ // in the child and so can execute the command 
   // use execvp to execute the command and detect errors    
    if (execvp(phrase[0], phrase)== -1){
      printf(" %s: we dont recognise this command\n", phrase[0]);
    }    
  }
  else { // in the parent process 
    int status;
    waitpid(pid, &status,0);
  }
}

/* Fuction that empties the phrase variable for the 
 * next phrase
 */
void emptyPhrase(char* phrase[MAX_INSTR/2]) {
  int counter=0;
  while (phrase[counter]!=NULL) {
    *(&phrase[counter])=NULL;
    counter++;
  }
}

/* Allow the user to set the path environment variable
 */
void setPath(char* phrase[MAX_INSTR/2]) {
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
void getPath(char* phrase[MAX_INSTR/2]){
  if (phrase[1]!=NULL){
    printf("Too many arguments\n");
  } else {
    printf("%s\n", getenv("PATH"));
  }
}

/* Need further implementation */
void singleExclamation (char *phrase[MAX_INSTR/2], char* instruction,
                        char *history[21]) {
  int lineNum;
  int len;
  
  if (phrase[2]!=NULL){
    printf("Too many arguments\n");
  } else if (phrase[1]==NULL){
    printf("Too few arguments\n");
  } else {
    printf("%s\n", instruction);
    len = strlen(phrase[1]);
    for (int i=0; i<len; i++) {
      lineNum = lineNum * 10 + (phrase[1][i] - '0');
    }
    if (lineNum > 20 || lineNum <1) {
      executeInstruction(phrase, history[lineNum], history, NULL);
    } else {
      printf("invalid item : %i\n", lineNum);
    }
  }
}

/* Need further implementation */
void doubleExclamation (char *phrase[MAX_INSTR/2], char* instruction,
                        char *history[21]) {
  int counter;
  if (phrase[1]!=NULL){
    printf("%s\n", instruction);
    executeInstruction(phrase, history[counter-1], history, NULL);
  } else {
    printf("Too few arguments\n");
  }
}


void writeHistory(char *instruction, char *history[21], int *rear){
  strcpy(history[* rear], instruction);
  * rear = (* rear+1)%MAX_HISTORY_SIZE;
  FILE *fp;
  fp= fopen("./hist_list","a");
  if (fp == NULL){
    printf("Could not open file");
    exit(1);
  }
  fprintf(fp,"%s", instruction);
  }


void readHistory(char *history[21], int *rear){
  FILE *fp;
  char x;
  fp = fopen("./hist_list","a+");
  
  //Reading
  x = getc(fp);
  while (x!= EOF){
    putchar(x);
    x = getc(fp);
    strcpy(history[*rear], &x);
    * rear = (* rear+1)%MAX_HISTORY_SIZE;
  }
  fclose(fp);
  for(int i = 1; i<= MAX_HISTORY_SIZE; i++){
        printf("%i. %s \n", i, history[i]);
    }
}


/**ß
void printHistory(char *history[21]){
    for(int i = 1; i<= 21; i++){
        printf("%s", history[i]);
    }
}
*/







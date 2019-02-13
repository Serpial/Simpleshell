#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/* Definitions */
#define PATHSIZE 200      // Maximum number of chars in currentDir
#define MAX_INSTR 512     // Maximum number of chars per phrase

/* Prototypes */
char* switchHome(char* currentDir);
char* buildPrefix(char* currentDir);
void parseInput(char* instruction, char* phrase[MAX_INSTR/2]);
void emptyPhrase(char* phrase[MAX_INSTR/2]);
void executeExternal(char* phrase[MAX_INSTR/2] );
void getPath(char* phrase[MAX_INSTR/2]);
void setPath(char* phrase[MAX_INSTR/2]);
void changeDirectory(char **arguments);


int main() {
  char currentDir[PATHSIZE]; // Current Directory the user in in
  char instruction[MAX_INSTR]; // Pre-parsed instruction
  char *phrase[MAX_INSTR/2]; // Array of components of the instruction
  char originalPath[500];

  // Use the default home directory as the default path
  strcpy(currentDir, getenv("HOME"));
  strcpy(originalPath, getenv("PATH"));

  // Sets current directory to current diretory
  chdir(currentDir);
  
  for(;;) {
    memset(instruction,0,strlen(instruction));
    emptyPhrase(phrase);
    
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

    parseInput(instruction, phrase);

    if (phrase[0]!=NULL){
      if(strcmp(phrase[0],"exit")==0) {
        exit(0);
      } else if (strcmp(phrase[0], "getpath")==0) {
        getPath(phrase);
      } else if (strcmp(phrase[0], "setpath")==0) {
        setPath(phrase);
      } else if (strcmp(phrase[0], "cd")==0) {
        changeDirectory(phrase);
      } else {
        executeExternal(phrase);
      }
    }
  }



  setenv("PATH", originalPath, 1);
  
}


/* switches "/home/<username>" to "~"
 */
char* switchHome(char* currentDir) {
  char* temp = malloc(PATHSIZE);
  static char newDir[PATHSIZE];
  if (strstr(currentDir, getenv("HOME"))) {
    newDir[0] = '~'; newDir[1]='\0';
    strncpy(temp, currentDir+strlen(getenv("HOME")),PATHSIZE);
    strcat(newDir, temp);
    return newDir;
  }
  return currentDir;
}

/* Try to get the user's username and add it to the
 * current directory the user is in
 */
char* buildPrefix(char* currentDir) {
  static char prefix[PATHSIZE];
  char *name;

  strcpy(prefix, getenv("USER"));
  strcat(prefix, ":");
  strcat(prefix, switchHome(currentDir));
  strcat(prefix, "$ ");
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
  char specialChar[] = "|><&;";
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
    memset(phrase[counter],0,strlen(phrase[counter]));
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

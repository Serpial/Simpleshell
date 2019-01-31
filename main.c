#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* Definitions */
#define PATHSIZE 200      // Maximum number of chars in currentDir
#define MAX_INSTR 512     // Maximum number of chars per phrase

/* Prototypes */
char* switchHome(char* currentDir);
char* buildPrefix(char* currentDir);
void parseInput(char* instruction, char* phrase[MAX_INSTR/2]);
void executeExternal(char* phrase[MAX_INSTR/2] );


int main() {
  char currentDir[PATHSIZE]; // Current Directory the user in in
  char instruction[MAX_INSTR]; // Pre-parsed instruction
  char *phrase[MAX_INSTR/2]; // Array of components of the instruction

  // Use the default home directory as the default path
  strcpy(currentDir, getenv("HOME"));

  for(;;) {
    printf("%s",buildPrefix(currentDir));

    fgets(instruction,MAX_INSTR,stdin);
    parseInput(instruction, phrase);
    executeExternal(phrase);

    if (strcmp(phrase[0],"exit")) exit(0);
  
  }
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

/* Separate the users instruction into an array of actionable
 * components.
 * Places item into an array of components passed to main 
 */
void parseInput(char* instruction, char* phrase[MAX_INSTR/2]) {
  int counter=0;
  char delim[] = " ";
  char *ptr = strtok(instruction, delim);
  while (ptr != NULL) {
    phrase[counter]=malloc(100);
    strcpy (phrase[counter], ptr);
    ptr = strtok(NULL, delim);
    counter++;

  }
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

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
char* buildPrefix(char* directory);
char** parseInput(char* instruction);
char** joinSubPhrase (char **phrase);
void executeExternal(char **phrase);
void getPath(char **phrase);
void setPath(char **phrase);
void changeDirectory(char **arguments);
void executeInstruction (char **phrase, char* instruction,
                         char *history[21]);
void singleExclamation (char **phrase, char* instruction,
                        char *history[21]);
void doubleExclamation (char **phrase, char* instruction,
                        char *history[21]);



/* Main Function */
int main() {
  char currentDir[PATHSIZE]; // Current Directory the user is in
  char instruction[MAX_INSTR]; // Pre-parsed instruction
  char **phrase; // Array of components of the instruction
  char originalPath[500];
  char *history[21];
  
  // Use the default home directory as the default path
  strcpy(currentDir, getenv("HOME"));
  strcpy(originalPath, getenv("PATH"));
    
  // Sets current directory to current diretory
  chdir(currentDir);
  
  for(;;) {
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
    
    // Run the given command
        
    executeInstruction(phrase, instruction, history);
    memset(instruction,0,strlen(instruction));

  }

  // Reset the path to what it was before the session was opened
  setenv("PATH", originalPath, 1);
}

void executeInstruction (char **phrase, char* instruction,
                         char *history[21]) {

  phrase = joinSubPhrase(parseInput(instruction));
    
  if (phrase!=NULL && phrase[0]!=NULL){
    if (strcmp(phrase[0], "getpath")==0) {
      getPath(phrase);
    } else if (strcmp(phrase[0], "setpath")==0) {
      setPath(phrase);
    } else if (strcmp(instruction, "!!")==0) {
      doubleExclamation(phrase, instruction, history);
    } else if (strcmp(phrase[0], "!")==0) {
      singleExclamation(phrase, instruction, history);      
    } else if (strcmp(phrase[0], "history")==0) {
      // getHistory();
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

/* Need further implementation */
void singleExclamation (char **phrase, char* instruction,
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
      executeInstruction(phrase, history[lineNum], history);
    } else {
      printf("invalid item : %i\n", lineNum);
    }
  }
}

/* Need further implementation */
void doubleExclamation (char **phrase, char* instruction,
                        char *history[21]) {
  int counter;
  if (phrase[1]!=NULL){
    printf("%s\n", instruction);
    executeInstruction(phrase, history[counter-1], history);
  } else {
    printf("Too few arguments\n");
  }
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
          if (phrase[j+1]==NULL) {
            printf("You have forgotten one of these \"\n");
            return NULL;
          }
        }
      } else return NULL;
    } else {
      newPhrase[itemIndex]=malloc(200);
      strcpy(newPhrase[itemIndex], phrase[i]);
      itemIndex++;
    }
  }

  newPhrase[itemIndex]=NULL;
  return newPhrase;
}

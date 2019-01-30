#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Definitions */
#define PATHSIZE 200      // Maximum number of chars in currentDir
#define MAX_INSTR 512     // Maximum number of chars per phrase

/* Prototypes */
char* switchHome(char* currentDir);
char* buildPrefix(char* currentDir);
void parseInput(char* instruction, char* phrase[MAX_INSTR/2]);
void emptyPhrase(char* phrase[MAX_INSTR/2]);



int main() {
  char currentDir[PATHSIZE]; // Current Directory the user in in
  char instruction[MAX_INSTR]; // Pre-parsed instruction
  char *phrase[MAX_INSTR/2]; // Array of components of the instruction

  // Use the default home directory as the default path
  strcpy(currentDir, getenv("HOME"));

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
    size_t len = strlen(instruction);
    if (len && (instruction[len-1] == '\n')) {
      instruction[len-1] = '\0';
    }

    parseInput(instruction, phrase);

    // exit on "exit"
    if(strcmp(instruction,"exit")==0) {
      exit(0);
    }

    // A  method of acessing the items
    int counter=0;
    while(phrase[counter][0]!='\0') {
      printf("%d:%s\n", counter, phrase[counter]);
      counter++;
    }
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
 * Also allows for characters to be inserted in the middle of
 * tokens and for them still to be parsed correctly
 */
void parseInput(char* instruction, char* phrase[MAX_INSTR/2]) {
  int itemNo=-1, connectedLettNo=0;
  char delim[] = " ", specialChar[] = "|><&;";
  char* occurance,* splitPoint = strtok(instruction, delim);
  char connectedWord[MAX_INSTR/2], strChr[2];

  // strChr allow each conversion of char to string
  strChr[1]='\0';
  
  while (splitPoint != NULL) {
    for (int i=0; i<strlen(specialChar); ) {
      occurance = strchr(splitPoint, specialChar[i]);
      if (occurance!=NULL) {
        for (int j=0; j<(occurance-splitPoint); j++) {
          connectedWord[connectedLettNo++]=splitPoint[j];
        }
        
        // Get rid of everything upto (and incl.) the special character
        splitPoint = splitPoint + (occurance-splitPoint+1);
        
        // Add the phrase before the spec. char. to the phrase if there is one
        if (connectedLettNo!=0) { 
          connectedWord[connectedLettNo]='\0';
          phrase[++itemNo]=malloc(100);
          strcpy (phrase[itemNo], connectedWord);
        }
        
        // Add the special character to the phrase
        phrase[++itemNo]=malloc(100);
        strChr[0]=specialChar[i];
        strcpy (phrase[itemNo], strChr);
          
        // Make this zero so we can have another word
        connectedLettNo=0;
      } else {
        i++;
      }
      if (i >= strlen(specialChar)) {
        // If the item doesn't contain a special character then add to phrase
        if (splitPoint[0]!='\0'){
          phrase[++itemNo]=malloc(100);
          strcpy (phrase[itemNo], splitPoint);
        }
      }
    }
    splitPoint = strtok(NULL, delim);
  }
  phrase[itemNo+1]=malloc(1);
}

/* Fuction that empties the phrase variable for the 
 * next phrase
 */
void emptyPhrase(char* phrase[MAX_INSTR/2]) {
  int counter=0;
  while (phrase[counter]!=NULL) {
    phrase[counter]=NULL;
    counter++;
  }
}

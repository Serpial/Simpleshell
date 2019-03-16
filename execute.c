#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "alias.h"
#include "history.h"
#include "main.h"
#include "path.h"

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

/* Stage 2: Execute external commands. */

void executeInstruction (char **phrase, char **history, int rear, char originalPath[500], char *alias[MAX_ALIAS_SIZE][2], int counter) {
    
    // Each instruction will be executed if the input is not null.
    if (phrase[0]!=NULL){
        if (strcmp(phrase[0], "getpath")==0) { // getPath.
            getPath(phrase);
        } else if (strcmp(phrase[0], "setpath") == 0) { // setPath.
            setPath(phrase);
        } else if (strcmp(phrase[0], "!")==0) {  // ! (used in history).
            recallHistory(phrase, history, rear, originalPath, alias, counter);      
        } else if (strcmp(phrase[0], "history")==0) { // history.
            printHistory(history, rear);
        } else if ((strcmp(phrase[0], "cd")==0)){ // cd (change directory).
            changeDirectory(phrase);
        } else if (strcmp(phrase[0], "exit")==0) {
            exitProgram(0, originalPath, history, rear, alias);
        } else if((strcmp(phrase[0], "alias")==0) && phrase[1]==NULL){
            printAlias(alias);
        } else if((strcmp(phrase[0], "alias")==0) && phrase[1]!=NULL){
            addAlias(phrase, alias);
        } else if(strcmp(phrase[0], "unalias")==0){
            removeAlias(phrase, alias);
        } else { 
            while(counter<10){
                int nullEntries=0;
                nullEntries = howManyNullSpaces(alias);
                char temp[512];
                char *p = temp;
            //checks if command is an alias then runs execute instruction again 
            for (int j =0; j<(MAX_ALIAS_SIZE-nullEntries); j++){
                counter++;
                if (strcmp(phrase[0], alias[j][0]) == 0){
                    strcpy(temp, alias[j][1]);   
                     p[strlen(p)-1]=0;
                     strcpy(phrase[0], temp);
                     executeInstruction(phrase, history, rear, originalPath, alias,counter);
                    return;
               }
                }

            }
            executeExternal(phrase);
        }
 
    }   
  
    
}


            
 
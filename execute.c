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

void executeInstruction (char **phrase, char **history, int rear, char originalPath[500], char *alias[MAX_ALIAS_SIZE][2]) {
    
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
            changeDirectory(phrase);
        } else if (strcmp(phrase[0], "exit")==0) {
            exitProgram(0, originalPath, history, rear, alias);
        } else if((strcmp(phrase[0], "alias")==0) && phrase[1]==NULL){
            printAlias(alias);
        } else if((strcmp(phrase[0], "alias")==0) && phrase[1]!=NULL){
            addAlias(phrase, alias);
        } else if(strcmp(phrase[0], "unalias")==0){
            removeAlias(phrase, alias);
        } else { // if the command is not pre-defined.
             // if the command is not pre-defined.
                
               //if we havent found command yet see if it is in alias
                 int j =0;
                 int i=0;
                 int l=0;
                 int k=0;
                 int p=0;
                 char value[512] ="";
                 char match[512]= "";
                 char test[512]= "";
               int nullEntries=0;
                 nullEntries = howManyNullSpaces(alias);

                 //checks if command is an alias then runs execute instruction again 
                for (int j =0; j<(MAX_ALIAS_SIZE-nullEntries); j++){
                    printf("here1\n");
                    if (strcmp(phrase[0], alias[j][0]) == 0){
                        printf("here2\n");
                        strcpy(value, alias[j][1]);
                        printf("here3\n");
                        for (i=0; i<MAX_ALIAS_SIZE-nullEntries; i++){
                            printf("here4\n");
                            strcpy(test, alias[i][1]);
                                printf("here5\n");
                                printf("test:%s\n", test);
                                printf("alias:%s\n", alias[j][1]);
                            if(strcmp(value, test)==0){
                                printf("here6\n");
                                strcpy(match, alias[i][1]);
                                printf("here7\n");
                                memset(phrase[0], 0, sizeof(phrase));
                                for (k=0; k<strlen(match); k++){

                                    printf("here8\n");
                                    if(match[k]!= ' '){
                                        char temp[512]= "";   
                                        //memset(phrase[0], 0, sizeof(phrase));
                                        printf("here9\n");
                                        printf("match:%c\n", match[k]);
                                        //strcat(phrase[l], &match[k]);
                                        strcat(temp,&match[k]);
                                        printf("here10\n");
                                        printf("temp:%s\n", temp);
                                        phrase[l] = temp;
                                        printf("phrase:%s\n", phrase[l]);
                                        l++;
                                    }
                                    
                                }
                                executeInstruction(phrase, history, rear, originalPath, alias);
                            }


                       
                    }
                }

}
     
                
                //if the command is not an 
                executeExternal(phrase);

}
        
    }

}

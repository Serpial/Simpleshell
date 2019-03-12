#include "history.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "phrase.h"
#include "alias.h"
#include "execute.h"

// Function prototype
int howMany(char **history);


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

/* Stage 6: Persistent history. */

void recallHistory (char **phrase, char **history, int rear, char *originalPath, char *(*alias)[2]) {
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
                        (MAX_HISTORY_SIZE-1)+(rear+lineNum)+1: lineNum+rear;

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

int howMany(char **history) {
    int counter=0;
    int index=0;
    while(strcmp(history[index], "\0") && index!=MAX_HISTORY_SIZE+1) {
        counter++;
        index++;
    }
    return counter;
}

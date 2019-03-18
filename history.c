/**************************************************************************
 * Assessment Title: Simple Shell
 *
 * Number of Submitted C Files: 8
 *
 * Date: 18th March 2019
 *
 * 
 * Authors: 
 *      Maria Elizabeth Hughes 201716768
 *		Paul Hutchison 201741535
 *		David  McFadyen 201707791
 *		Mohammed Adil Ali Akbar 201708796
 *
 *
 *
 * Personal Statement: I confirm that this submission is all my own work.
 *
 *          (Signed)_Maria E Hughes
 *          (Signed)_Paul Hutchison
 *          (Signed)_David McFadyen
 *
 * 
 *
 **************************************************************************/




#include "history.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "alias.h"
#include "execute.h"

// Function prototype
int howMany(char **history);
void executeRecall(char **phrase, char **history, int rear, char *originalPath, char *(*alias)[2], int counter, int lineNum);

/*Stage 5: Adding commands to history, invoking commands from history and
  printing the history. */

/*Adds instruction to history under certain conditions
 */
int addToHistory(char **history, int rear, char** phrase) {
    int counter=0, i;
    char newInstruction[MAX_INSTR];
    strcpy(newInstruction, "");
    char charToAdd[2];
    charToAdd[1]='\0';

    // Adds each token in phrase to instruction with a space in between
    while (phrase[counter]!=NULL && counter<MAX_INSTR) {
        for(i=0; phrase[counter][i]!='\0';i++) {
            charToAdd[0]=phrase[counter][i];
            strcat(newInstruction, charToAdd);
        }
        if (phrase[counter][i]=='\0'&&phrase[counter+1]!=NULL){
            strcat(newInstruction, " ");
        }
        counter++;
    }
    
    newInstruction[MAX_INSTR-1]='\0';
    if (strcmp(newInstruction, "")!=0) {
        strcpy(history[rear], newInstruction);
        rear = (1+rear) % MAX_HISTORY_SIZE;
    }
    return rear;
}

/* Prints 20 elements of history to the user.
 */
void printHistory(char **history, int rear, char **phrase){
    if (phrase[1]!=NULL) {
        printf("Error: history: too many arguments\n");
        return;
    }
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

/* Is what is used to “recall” a command from memory.
 * With it a user can specify where in memory the requested memory is
 * and then have it be used as normal through the executer file.
 */
void recallHistory (char **phrase, char **history, int rear, char *originalPath, char *(*alias)[2], int counter) {
    int lineNum=0;

    if (phrase[1]!=NULL) {
        if (strcmp(phrase[1],"!")==0 && phrase[2]==NULL) {
            // This is for getting the last executable instruction when !! is used
            lineNum = (rear==0?MAX_HISTORY_SIZE-1:rear-1);
            executeRecall(phrase, history, rear, originalPath, alias, counter, lineNum);
            return;


        } else {
            lineNum = strtol(phrase[1],NULL,0);
            if (lineNum!=0 && lineNum<=MAX_HISTORY_SIZE && lineNum>=-MAX_HISTORY_SIZE) {
                if (lineNum<0) {  // This is for getting the last x instruction
                    lineNum = (rear+lineNum)<0?
                        (MAX_HISTORY_SIZE-1)+(rear+lineNum)+1: lineNum+rear;
                    
                    executeRecall(phrase, history, rear, originalPath, alias, counter, lineNum);
                    return;
                } else {  // This is for getting that item from history
                    if (howMany(history)==20) {
                        lineNum = (rear+lineNum-1)%MAX_HISTORY_SIZE;
                    } else {
                        lineNum--;
                    }
                    
                    executeRecall(phrase, history, rear, originalPath, alias, counter, lineNum);
                    return;
                }
            } else {
                if (lineNum!=0) {
                    printf("Error: history recall: You have entered too large a value\n");
                } else {
                    printf("Error: history recall: Cannot Enter 0\n");
                }
            }
        }
    }
    if (phrase[2]!=NULL) {
        printf("Error: history recall: You have entered too many arguments\n");
        return;
    }
}

/* Small function that follows the steps necessary whenever an item
 * is recalled. It only recals if the item on the list is not nothing
 * otherwise it gives an error
 */
void executeRecall(char **phrase, char **history, int rear, char *originalPath, char *(*alias)[2], int counter, int lineNum) {
    if (strcmp(history[lineNum], "")!=0) {
        printf("%s\n", history[lineNum]);
        phrase = parseInput(history[lineNum]);
        executeInstruction(phrase, history, rear, originalPath, alias, counter);
    } else {
        printf("Error: history recall: Not an item in history\n");
    }
}

 /*howMany: returns how many entries are in History*/
int howMany(char **history) {
    int counter=0;
    int index=0;
    while(index<MAX_HISTORY_SIZE && strcmp(history[index], "\0") ) {
        counter++;
        index++;
    }
    return counter;
}


/* Stage 6: Persistent history. */

/* Writes the current elements in History to a specified file
 * location returning an error message if it does not open
 */
void writeHistory(char **history, int rear) {
    FILE *fp;
    int i;
    char fileLocation[MAX_INSTR]="";

    strcpy(fileLocation, getenv("HOME"));
    strcat(fileLocation, "/.hist_list");
    fp= fopen(fileLocation,"w");

    if (fp == NULL){
        printf("Error: Could not open history file\n");
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

/* Opens a text file in the same specified location as WriteHistory
 * and fills  the running programs history with the text in the file.
 */
void readHistory(char **history, int *rear){
    FILE *fp;
    int numInstr=0;
    char newInstruction[MAX_INSTR]="";
    char fileLocation[MAX_INSTR]="";
    size_t len;

    // Set the file location
    strcpy(fileLocation, getenv("HOME"));
    strcat(fileLocation, "/.hist_list");
    fp = fopen(fileLocation,"a+");

    // Get lines from files
    while(fgets(newInstruction, sizeof(newInstruction), fp)!=NULL && numInstr<20) {
        if (strcmp(newInstruction, "\n")!=0){ // dont add empty line
            len = strlen(newInstruction);
            if (len && (newInstruction[len-1] == '\n')) {
                newInstruction[len-1] = '\0';
            }
            strcpy(history[numInstr++], newInstruction);
        }
    }
    // Set rear
    *rear = numInstr==MAX_HISTORY_SIZE?0:numInstr;
    fclose(fp);
}

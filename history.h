/**************************************************************************
 * Assessment Title: Simple Shell
 *
 * Number of Submitted C Files: 8
 *
 * Date: 18th March 2019
 *
 * 
 * Author: Maria Elizabeth Hughes 201716768
*		 Paul Hutchinson
*		David  McFadyen
*		Mohammed Adil Ali Akbar
*
 *
 *
 * Personal Statement: I confirm that this submission is all my own work.
 *
 *          (Signed)_Maria E Hughes
 *
 * 
 *
 **************************************************************************/


#ifndef HISTORY_H
#define HISTORY_H

#define MAX_HISTORY_SIZE 20 // Maximum number of instructions stored 

int addToHistory(char **history, int rear, char** phrase);
void writeHistory(char **history, int rear);
void readHistory(char **history, int *rear);
void printHistory(char **history, int rear, char **phrase);
void recallHistory (char **phrase, char **history, int rear, char *originalPath, char *(*alias)[2], int counter);
#endif

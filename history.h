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
 *          (Signed)_Adil A Akbar
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

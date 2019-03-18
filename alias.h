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




#ifndef ALIAS_H
#define ALIAS_H

#define MAX_ALIAS_SIZE 10

void printAlias(char *alias[MAX_ALIAS_SIZE][2]);
void addAlias(char**phrase, char *alias[MAX_ALIAS_SIZE][2]);
void removeAlias(char **phrase, char *alias[MAX_ALIAS_SIZE][2]);
void invokeAlias(char* instruction, char *alias[MAX_ALIAS_SIZE][2]);
void readAliases (char *alias[MAX_ALIAS_SIZE][2]);
void writeAliases(char *alias[MAX_ALIAS_SIZE][2]);
void substituteAlias(char *alias[MAX_ALIAS_SIZE][2]);
int howManyNullSpaces(char *alias[MAX_ALIAS_SIZE][2]);
#endif

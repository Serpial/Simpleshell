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

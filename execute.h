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


#ifndef EXECUTE_H
#define EXECUTE_H
// Definitions
#define PATHSIZE 200  // Maximum number of chars in currentDirectory

void executeExternal(char **phrase);
void executeInstruction (char **phrase, char **history, int rear, char originalPath[500], char *(*alias)[2], int counter);
#endif

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



#ifndef EXECUTE_H
#define EXECUTE_H
// Definitions
#define PATHSIZE 200  // Maximum number of chars in currentDirectory

void executeExternal(char **phrase);
void executeInstruction (char **phrase, char **history, int rear, char originalPath[500], char *(*alias)[2], int counter);
#endif

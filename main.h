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
*		Paul Hutchinson
*		David  McFadyen 201707791
*		Mohammed Adil Ali Akbar 201708796
*
 *
 *
 * Personal Statement: I confirm that this submission is all my own work.
 *
 *          (Signed)_Maria E Hughes
 *          (Signed)_David McFadyen
 *
 * 
 *
 **************************************************************************/



#ifndef MAIN_H
#define MAIN_H

// Definitions
#define MAX_INSTR 512 // Maximum number of chars per phrase

char** parseInput(char* instruction);
void changeDirectory(char **arguments);
void exitProgram(int exitCode, char *originalPath, char **history, int rear, char *(*alias)[2]);
#endif

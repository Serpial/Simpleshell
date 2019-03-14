#ifndef MAIN_H
#define MAIN_H

// Definitions
#define MAX_INSTR 512 // Maximum number of chars per phrase

char** parseInput(char* instruction);
void changeDirectory(char **arguments);
void exitProgram(int exitCode, char *originalPath, char **history, int rear, char *(*alias)[2]);
#endif

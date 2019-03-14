#ifndef EXECUTE_H
#define EXECUTE_H
// Definitions
#define PATHSIZE 200  // Maximum number of chars in currentDirectory

void executeExternal(char **phrase);
void executeInstruction (char **phrase, char **history, int rear, char originalPath[500], char *(*alias)[2]);
#endif

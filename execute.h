#ifndef EXECUTE_H
#define EXECUTE_H

void executeExternal(char **phrase);
void executeInstruction (char **phrase, char **history, int rear, char originalPath[500], char *alias[MAX_ALIAS_SIZE][2], int ßßcounter);
#endif

#ifndef HISTORY_H
#define HISTORY_H

#define MAX_HISTORY_SIZE 20 // Maximum number of instructions stored 

int addToHistory(char **history, int rear, char** phrase);
void writeHistory(char **history, int rear);
void readHistory(char **history, int *rear);
void printHistory(char **history, int rear);
void recallHistory (char **phrase, char **history, int rear, char *originalPath, char *(*alias)[2]);
#endif

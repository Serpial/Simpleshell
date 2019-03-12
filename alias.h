#ifndef ALIAS_H
#define ALIAS_H

#define MAX_ALIAS_SIZE 20

void printAlias(char *alias[MAX_ALIAS_SIZE][2]);
void addAlias(char**phrase, char *alias[MAX_ALIAS_SIZE][2]);
void removeAlias(char **phrase, char *alias[MAX_ALIAS_SIZE][2]);
void invokeAlias(char* instruction, char *alias[MAX_ALIAS_SIZE][2]);
void readAliases (char *alias[MAX_ALIAS_SIZE][2]);
void writeAliases(char *alias[MAX_ALIAS_SIZE][2]);
void substituteAlias(char *alias[MAX_ALIAS_SIZE][2]);
int howManyNullSpaces(char *alias[MAX_ALIAS_SIZE][2]);
#endif

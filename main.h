#ifndef MAIN_H
#define MAIN_H

void changeDirectory(char **arguments);
void exitProgram(int exitCode, char *originalPath, char **history, int rear, char *(*alias)[2]);
#endif

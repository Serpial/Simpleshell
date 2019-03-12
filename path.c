#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Stage 3: Setting the Path */

// Allows the user to SET the path environment variable.
void setPath(char **phrase) {
    char tempPath[500];

    if (phrase[2]!=NULL){
        printf("Too many arguments\n");
    } else if (phrase[1]==NULL){
        printf("Too few arguments\n");
    } else {
        strcpy(tempPath, phrase[1]);
        strcat(tempPath, ":");
        strcat(tempPath, getenv("PATH"));
        setenv("PATH", tempPath, 1);
    }
}

/* Stage 3: Getting the Path */

// Allow the user to GET the path environment variable.
void getPath(char **phrase){
    if (phrase[1]!=NULL){
        printf("Too many arguments\n");
    } else {
        printf("%s\n", getenv("PATH"));
    }
}

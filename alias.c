#define _GNU_SOURCE
#include "alias.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "phrase.h"


void printAlias(char *alias[MAX_ALIAS_SIZE][2]){
    int index;
    int nullEntries=0;
    int counter =1;

    for (index =0; index <MAX_ALIAS_SIZE; index++){
        if (alias[index][0] == NULL){
            nullEntries++; 
        }
    }

    if (nullEntries == MAX_ALIAS_SIZE){
        printf("You dont have any aliases\n");
    }
    else{
        for(index =0; index< (MAX_ALIAS_SIZE - nullEntries); index++){
            printf("%i. %s %s \n", counter++, alias[index][0], alias[index][1]);
        }
    }
}

void addAlias(char**phrase, char *alias[MAX_ALIAS_SIZE][2]){
    if (phrase[1] == NULL){
        printf("Not enough arguments");
        return;
    }

    //char name[] = " ";
    char command [512] = " ";
    int index = 2;
    int j;
    int found = 0;
    //find how mnany arguments there are 
    while(phrase[index] != NULL){
        index++;
    }

    //makes command for alias
    for (j =2; j<index; j++){
        strcat(command, phrase[j]);
        strcat(command, " ");
    }

    //counts null charatcers
    int nullEntries=0;
    for (index =0; index <MAX_ALIAS_SIZE; index++){
        if (alias[index][0] == NULL){
            nullEntries++; 
        }
    }

    //checks list of previous alias 
    for (j =0; j<(MAX_ALIAS_SIZE-nullEntries); j++){
        if (strcmp(phrase[1], alias[j][0]) == 0){
            printf("overwriting previous alias\n");
            alias[j][1] = strdup(command);   
            found = 1;
        }
    }

    //checks if alias is full if not adds
    if(nullEntries == 0){
        printf("no more space for alias");
        //return;
    }
    else if (nullEntries != 0 && found == 0){
        alias[MAX_ALIAS_SIZE-nullEntries][0] =strdup(phrase[1]);
        alias[MAX_ALIAS_SIZE-nullEntries][1] =strdup(command);
    }
}


void removeAlias(char **phrase, char *alias[MAX_ALIAS_SIZE][2]){
    int found = 0;
    int index;
    int j;
    char name[512] = " ";

    if (phrase[1] == NULL){
        printf("Error: too few arguments\n");
        return;
    } else if (phrase[2] != NULL){
        printf("Error: too many arguments\n");
        return;
    }

    strcpy(name, phrase[1]);

    for (index=0; index< MAX_ALIAS_SIZE; index++){
        if (alias[index][0] != NULL){
            if (strcmp(name, alias[index][0]) == 0){
                alias[index][0] = NULL;
                alias[index][1] = NULL;
                found = 1;
                j = index;
            }
        }
    }

    if(found == 0){
        printf("This alias does not exits\n");
        return;
    }
    else{ //moves all elemnets in array up one
        while(j<(MAX_ALIAS_SIZE-1)){
            alias[j][0] = alias[j+1][0];
            alias[j][1] = alias[j+1][1];
            j++;
        } // make the last entry null
        alias[MAX_ALIAS_SIZE-1][0] = NULL;
        alias[MAX_ALIAS_SIZE-1][1] = NULL;
    }
    return;
}


void invokeAlias(char* instruction, char *alias[MAX_ALIAS_SIZE][2]){
    int index;
    for (index = 0; index<MAX_ALIAS_SIZE; index++){
        if (alias[index][0] != NULL){
            if (strcmp(instruction, alias[index][0]) == 0){
                strcpy(instruction, alias[index][1]);
            }
        }
    }
}

void readAliases (char *alias[MAX_ALIAS_SIZE][2]) {
    FILE *fp;
    char fileLocation[MAX_INSTR]="";
    char joinedAlias[MAX_INSTR]="", temp[MAX_INSTR];
    int numAlias=0, len, letterIndex, pastBar = 0;
    
    strcpy(fileLocation, getenv("HOME"));
    strcat(fileLocation, "/.aliases");
    fp = fopen(fileLocation, "a+");
    
    while(fgets(joinedAlias, sizeof(joinedAlias), fp)!=NULL && numAlias<MAX_ALIAS_SIZE) {
        len = strlen(joinedAlias);
        letterIndex=0;
        if (len && (joinedAlias[len-1]=='\n')) {
            joinedAlias[len-1] = '\0';   
        }

        // Prepend alias so that addAlias can be used
        if (strchr(joinedAlias, '|')!=NULL) {
            strcpy(temp, joinedAlias);
            strcpy(joinedAlias, "alias ");
            strcat(joinedAlias, temp);

            for (int i=0; i<strlen(joinedAlias); i++) {
                if (joinedAlias[i]=='|') {
                    joinedAlias[i]= ' ';
                    break;
                }
            }
            addAlias(parseInput(joinedAlias), alias);
        } else {
            printf("Error: Unable to add alias:%s\n", joinedAlias);
        }
    }
    fclose(fp);
}

void writeAliases(char *alias[MAX_ALIAS_SIZE][2]) {
    FILE *fp;
    char fileLocation[MAX_INSTR]="";
    int aliasCounter = 0;
    
    strcpy(fileLocation, getenv("HOME"));
    strcat(fileLocation, "/.aliases");
    fp = fopen(fileLocation, "w");

    if (fp == NULL) {
        printf("Could not open history file\n");
        return;
    }
    
    while (aliasCounter<MAX_ALIAS_SIZE && alias[aliasCounter][1]!=NULL) {
        fprintf(fp, "%s|%s\n", alias[aliasCounter][0], alias[aliasCounter][1]);
        aliasCounter++;
    }
}

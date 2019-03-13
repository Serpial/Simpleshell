#include "phrase.h"

#include <string.h>
#include <stdlib.h>

/* takes instances of quotation marks and joins them in to phrases
 */
char** joinSubPhrase (char **phrase) {
    static char *newPhrase[MAX_INSTR/2];
    int itemIndex=0;

    for (int i=0; phrase[i]!= NULL; i++) {
        if (strcmp(phrase[i], "\"")==0) {
            if (phrase[i+1]!=NULL){
                newPhrase[itemIndex]=malloc(200);
                strcpy(newPhrase[itemIndex], phrase[++i]);
                for (int j=i+1; phrase[j]!=NULL; j++) {
                    if (strcmp(phrase[j],"\"")==0) {
                        i=j;
                        itemIndex++;
                        break;
                    } else {
                        strcat(newPhrase[itemIndex], " ");
                        strcat(newPhrase[itemIndex], phrase[j]);
                    }
                    if (phrase[j+1]==NULL) { return NULL; }
                }
            } else { return NULL; }
        } else {
            newPhrase[itemIndex]=malloc(200);
            strcpy(newPhrase[itemIndex], phrase[i]);
            itemIndex++;
        }
    }

    newPhrase[itemIndex]=NULL;
    return newPhrase;
}

// Separate the users instruction into an array of actionable components.
char** parseInput(char* instruction) {
    char specialChar[] = "!|><&;\"";
    int counter=0, wordIdx=0, letterIdx=0;
    static char *phrase[MAX_INSTR/2];
	
	// Places item into an array of components passed to the main function.
    while (counter<strlen(instruction)&&counter<MAX_INSTR){

        // If the character is not a special character...
        if (strchr(specialChar, instruction[counter])==NULL) {

            // ...then add it to a member of the phrase.
            if (instruction[counter]!=' '&&instruction[counter]!='\t') {

                if (letterIdx == 0) {
                    phrase[wordIdx]= (char *) malloc(100);
                }

                phrase[wordIdx][letterIdx++]=instruction[counter];


                if (instruction[counter+1]==' '||instruction[counter+1]=='\0'||
                    instruction[counter+1]=='\t') {
                    phrase[wordIdx][letterIdx]='\0';
                    wordIdx++;
                    letterIdx=0;
                }
            }
        }

		// Also allows for characters to be inserted in the middle of tokens and for them still to be parsed correctly.
        else {

            // Add the special character to the phrase
            if (letterIdx!=0) {
                phrase[wordIdx][letterIdx]='\0';
                wordIdx++;
                letterIdx=0;
            }
            phrase[wordIdx]=malloc(2);
            phrase[wordIdx][0]=instruction[counter];
            phrase[wordIdx][1]='\0';
            wordIdx++;
        }
        counter++;
    }
    // Set the value after the last phrase to NULL
    phrase[wordIdx]=NULL;
    return phrase;
}

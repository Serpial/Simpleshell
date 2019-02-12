/*
 * This is the function for cd (Change Directory) that I have created.
 */
 
void changeDirectory(char **arguments) 
{
    char *firstArgument = arguments[1];
	
    if (arguments[2] != NULL) { // This will check that there is not an invalid number of arguments, printing an error message if there is.
        printf("Invalid number of arguments\n");
        return;
    }
    if (firstArgument == NULL) { // If there is no argument entered, it will act as the home directory link (if "cd" is entered on it's own). This is the first form as specified in the ACE4 PDF.
        chDir(getenv("HOME"));
    } 
	else 
	{	
        if (strcmp(".", firstArgument) == 0) { // The . represents the current directory.
            chDir(".");
			
        } else if(strcmp("..", firstArgument) == 0) { // The .. represents the parent directory.
            chDir("..");
			
        } else { 
            if(chDir(firstArgument) == -1) { // This will use the perror() function if there is no such directory.
		perror(firstArgument);
            }
          }
      }
	  
    char *getDir = getDir(NULL, 0); // This will return the cwd (current working directory).
    printf("The current directory is: %s\n", getDir);
    free(getDir);
}
const int MAX_HISTORY_SIZE = 20;

char * histArray[MAX_HISTORY_SIZE];
for (int i = 0; i <= MAX_HISTORY_SIZE; i++) {
    histArray[i] = (char * ) malloc(512);
    strcpy(histArray[i], "\0");
}
int rear = 0;

	
void writeHistory(char *c, char * *histArray, int *rear){
	strcpy(histArray[* rear], c);
	* rear = (* rear+1)%MAX_HISTORY_SIZE
	
	FILE *fp;
	file = fopen(".hist_list","a");
	fprintf(fp,"%s",c);
	fclose(fp);
}

void readHistory(char * *histArray, int *rear){
	FILE *fp;
	int x;
	file = fopen(".hist_list","a+");
	
	//Reading
	x = getc(fp);
	while (x!= EOF){
		putchar(x);
		x = getc(fp);
		strcpy(histArray[* rear], x);
		* rear = (* rear+1)%MAX_HISTORY_SIZE
	}
	
	
	fclose(fp);
}

void printHistory(int histPrint){
    for(int i = 1; i<= histPrint; i++){
        printf(%s,histArray[i])
    }
}
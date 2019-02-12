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
	file = fopen("history.txt","a");
	fprintf(fp,"%s",c);
	fclose(fp);
}

void readHistory(char * *histArray, int *rear){
	FILE *fp;
	int x;
	file = fopen("history.txt","a+");
	
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

void printHistory(){
	FILE *fp;
	int x;
	file = fopen("history.txt","a+");
	
	x = getc(fp);
	while (x!= EOF){
		putchar(x);
		x = getc(fp);
		printf(%s,x);
	}
	fclose(fp);
}
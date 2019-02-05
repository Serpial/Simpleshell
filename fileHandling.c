void fileRead(){
   FILE *fopen(), *fp;
   int x;
   fp = fopen("history.txt","r");
   x = getc(fp);
   while (x!= EOF){
   		putchar(x);
		x = getc(fp);
		printf(%s,x);
   }
   fclose(fp);
}

void fileWrite(char *c){
	FILE *fp;
	file = fopen("history.txt","w");
	fprintf(fp,"%s",c);
	fclose(fp);
}

void fileAppend(char *c){
	FILE *fp;
	file = fopen("history.txt","a");
	fprintf(fp,"%s",c);
	fclose(fp);
}

void fileAppendOrRead(char *c){
	FILE *fp;
	int x;
	file = fopen("history.txt","a+");
	
	//Reading
	x = getc(fp);
	while (x!= EOF){
		putchar(x);
		x = getc(fp);
		printf(%s,x);
	}
	
	//Write
	fprintf(fp,"%s",c);
	fclose(fp);
}
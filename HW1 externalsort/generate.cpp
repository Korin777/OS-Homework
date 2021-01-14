#include<iostream>
#include<fstream>
#include<cstdlib>
#include<stdio.h>
#include<stdlib.h>
int main(int argc,char *argv[]){
    FILE * fout;
    fout = fopen( "10G.txt" , "w" );
	srand(time(NULL));
	for(int i = 0; i < atoi(argv[1]); i++) {
		int positive = rand();
        int negative = rand() - 2147483647 - 1;
        if(rand()%2==0) {
            fprintf(fout, "%d\n", positive);
        }
        else
            fprintf(fout, "%d\n", negative);
	}
} 
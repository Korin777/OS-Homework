#include<iostream>
#include<fstream>
#include<cstdlib>
#include<stdio.h>
#include<stdlib.h>
#include<map>
#include<string>
using namespace std;
// #define SIZE 7011042
#define SIZE 1199670


long long int random_longlong(int up_16bits, int low_16bits) {
    long long int randomnumber = (long long int)up_16bits << 16 + (long long int)low_16bits;
    if(randomnumber<0ll) {
        randomnumber = -randomnumber;
    } 
    if(randomnumber == (long long int)-9223372036854775808ll) {
        randomnumber = 0;
    }
    return randomnumber;
}


int main(int argc,char *argv[]){

    // int *test = (int*)malloc(sizeof(int)*5000000000);
    // cout << test << endl;
    FILE *fout,*fin;
    fin = fopen("number.txt","r");
    // long long int *a = (long long int*)malloc(sizeof(long long int)*SIZE);
    int *a = (int*)malloc(sizeof(int)*SIZE);
    for(int i = 0; i < SIZE; i++) {
        // fscanf(fin,"%lld\n",&a[i]);
        fscanf(fin,"%d\n",&a[i]);
    }
    fout = fopen( "get.txt" , "w" );
	srand(time(NULL));
	for(int i = 0; i < 100000; i++) {
        // long long int num = a[rand() % SIZE];
        // int num = a[rand() % SIZE];
        int num = rand();
        // fprintf(fout, "GET %lld\n",num);
        fprintf(fout, "GET %d\n",num);
	}

    // map<int,*char> test;
    // test[1] = "123";

} 
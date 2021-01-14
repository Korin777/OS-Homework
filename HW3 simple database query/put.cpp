#include<iostream>
#include<fstream>
#include<cstdlib>
#include<stdio.h>
#include<stdlib.h>
#include<map>
#include<string>
using namespace std;


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
    FILE * fout;
    fout = fopen( "put.txt" , "w" );
    int *record = (int*)malloc(sizeof(int)*1200000) ;
	srand(time(NULL));
    int i = 0;
	for(i = 0; i < 1200000; i++) {
        if(i%1000000 == 0)
        cout << i << endl;
        // long long int num = random_longlong(rand(),rand());
        int num = rand();
        record[i] = num;
        string s;
        for(int i = 0; i < 128; i++) {
            s[i] = rand()%75+48;
        }
        const char *c = s.data();
        // printf("%s\n",c);
        // cout << num << endl;
        fprintf(fout, "PUT %d =cbT[yU^rKh99GVHmiW<[z8y:TreQ`a^11kEczsC3JMpa\\=<4MH_LP]V^=@4nZKXD<nxoO@riFP9sFEwcG[4grDJ4=N[hS91_`y<4sgW><`jR^fnu0rKroe_2m@SEIT]\n", num);
        // fprintf(fout, "PUT %d\n", num);
	}
    for(i = i; i < 10000000; i++) {
        if(i%1000000 == 0)
        cout << i << endl;
        // long long int num = random_longlong(rand(),rand());
        int num = record[rand()%1200000];
        string s;
        for(int i = 0; i < 128; i++) {
            s[i] = rand()%75+48;
        }
        const char *c = s.data();
        // printf("%s\n",c);
        // cout << num << endl;
        // fprintf(fout, "PUT %lld %s\n", num,c);
        fprintf(fout, "PUT %d =cbT[yU^rKh99GVHmiW<[z8y:TreQ`a^11kEczsC3JMpa\\=<4MH_LP]V^=@4nZKXD<nxoO@riFP9sFEwcG[4grDJ4=N[hS91_`y<4sgW><`jR^fnu0rKroe_2m@SEIT]\n", num);
	}

    fclose(fout);
} 
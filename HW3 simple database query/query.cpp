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
    fout = fopen( "query.txt" , "w" );
	srand(time(NULL));
	for(int i = 0; i < 4000000; i++) {
        if(i%100000 == 0)
        cout << i <<endl;
        long long int num = random_longlong(rand(),rand());
        string s;
        for(int j = 0; j < 128; j++) {
            s[j] = rand()%75+48;
        }
        // const char *c = s.data();
        // printf("%s\n",c);
        // cout << num << endl;
        int tmp = rand();
        if(tmp % 5 != 0)
            fprintf(fout, "PUT %lld %s\n", num,s.c_str());
        else
            fprintf(fout, "GET %lld\n",num);
	}

    // map<int,*char> test;
    // test[1] = "123";

} 
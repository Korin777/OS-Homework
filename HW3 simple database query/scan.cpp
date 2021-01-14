#include<iostream>
#include<fstream>
#include<cstdlib>
#include<stdio.h>
#include<stdlib.h>
#include<map>
#include<algorithm>
using namespace std;
#define SIZE 10000000
int main(int argc,char *argv[]){
    FILE * fout;
    fout = fopen( "put.txt" , "r" );
    // fout = fopen( "output" , "r" );
    FILE *ftest = fopen("number.txt","w");
    char a[129],b[4];
    // long long int *c = (long long int*)malloc(sizeof(long long int)*SIZE);
    int *c = (int*)malloc(sizeof(int)*SIZE);
    int i = 0;
    for(int i = 0; i < SIZE;i++){
        // fscanf(fout,"%s %lld %s\n",b,&c[i],a);
        fscanf(fout,"%s %d %s\n",b,&c[i],a);
        // cout << c[i] << endl;
    }
    sort(c,c+SIZE);
    int count = 0;
    for(int i = 1; i < SIZE; i++) {
        if(c[i] == c[i-1]){
            count++;
            // cout << c[i] << endl;
        }
        else {
            // fprintf(ftest,"%lld\n",c[i]);
             fprintf(ftest,"%d\n",c[i]);
        }
    }
    cout << count << endl;


    // int last = 0;
    // int recent = 0;
    // int count = 0;
    // while (fscanf(fout,"%d",&recent)!=EOF)
    // {
    //     // cout << recent << endl;
    //     if(recent == last) {
    //         count++;
    //         cout << last << endl;
    //     }
    //     last = recent;
    // }
    // cout << count << endl;



} 
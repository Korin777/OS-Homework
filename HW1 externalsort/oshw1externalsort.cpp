#include<iostream>
#include<fstream>
#include<algorithm> 
#include<sstream>
#include<time.h>
#include<stdio.h>
#include<string>
#include<cstring>
#define SIZE 100000000
using namespace std;
typedef struct {
	int num;
	int file_id;
}compare_num;
bool compare(compare_num a,compare_num b) {
	return (a.num > b.num);
}
int main(int argc, char *argv[]){
	clock_t start,end;
	start = clock();
	string inputname = "input.txt";
	char tmp_filec1[inputname.size()+1];
	strcpy(tmp_filec1,inputname.c_str());
	string fname = "file";
	string tmpfname = fname + "1.txt";
	char tmp_filec2[tmpfname.size()+1];
	strcpy(tmp_filec2,tmpfname.c_str());

	FILE *fin;
	fin = fopen(tmp_filec1,"r");
 	FILE *fout;
 	fout = fopen( tmp_filec2 , "w" );
	int file_count = 1;
	stringstream ss;
	int count = 0;//1000000 10MB
	int cur_data;
	int *sort_num = (int*)malloc(sizeof(int) * SIZE);
	cout << "分檔中" << endl; 
	while(fscanf(fin,"%d",&sort_num[count]) != EOF) {
		count++;
		if(count == SIZE) {
			cout << "time consume: " << (double)(clock() - start) / CLOCKS_PER_SEC << endl;
			cout << "sort開始" << endl;
			sort(sort_num,sort_num + SIZE);
			cout << "sort結束" << endl;
			cout << "time consume: " << (double)(clock() - start) / CLOCKS_PER_SEC << endl;
			count = 0;
			cout << "寫入檔案" << endl;
			for(int i = 0; i < SIZE; i++) {
				fprintf(fout, "%d\n", sort_num[i]);	
			}
			cout << "寫入完畢" << endl;
			file_count++;
			fclose(fout);
			ss << file_count; 
			string tmp_file = fname + ss.str() + ".txt";
			char tmp_filec[tmp_file.size()+1];
			strcpy(tmp_filec,tmp_file.c_str());
			fout = fopen( tmp_filec , "w" );
			ss.str("");
			ss.clear();
		}
	}
	if(count == 0) {
		ss << file_count; 
		string tmp_file = "file" + ss.str() + ".txt";
		char tmp_filec[tmp_file.size()+1];
		strcpy(tmp_filec,tmp_file.c_str());
		if(remove(tmp_filec) == 0) {
			cout << "sucess" << endl;
		}
		ss.str("");
		ss.clear();
		file_count--;
	}
	else
	{
		sort(sort_num,sort_num + count);
		for(int i = 0; i < count; i++) {
		// fout << sort_num[i] << endl;
			fprintf(fout, "%d\n", sort_num[i]);
		}
		fclose(fout);
	}
	cout << "分檔結束" << endl;
	cout << "time consume: " << (double)(clock() - start) / CLOCKS_PER_SEC << endl;
	int delete_count = file_count;
	fclose(fin);
	compare_num cn[file_count];
	FILE *fin_arr[file_count];
	cout << "合併檔案中" << endl;
	for(int i = 0; i < file_count; i++){
		cn[i].file_id = i;
		ss << (i + 1);
		string tmp_file = fname + ss.str() + ".txt";
		char tmp_filec[tmp_file.size()+1];
		strcpy(tmp_filec,tmp_file.c_str());
		fin_arr[i] = fopen(tmp_filec,"r");
		ss.str("");
		ss.clear();
		fscanf(fin_arr[i],"%d",&cn[i].num);
	}
	string fname2 = "output";
	tmpfname = fname2 + ".txt";
	char tmp_filec3[tmpfname.size()+1];
	strcpy(tmp_filec3,tmpfname.c_str());
	fout = fopen( tmp_filec3 , "w" );
	sort(cn,cn + file_count,compare);
	fprintf(fout, "%d\n", cn[file_count-1].num);
	while(file_count!=0){
		if(fscanf(fin_arr[cn[file_count-1].file_id],"%d",&cn[file_count-1].num)!=EOF) {
			sort(cn,cn + file_count,compare);
		}
		else {
			file_count--;
			cout << file_count << endl;
			if(file_count == 0)
				break;
		}
		fprintf(fout, "%d\n", cn[file_count-1].num);
	}
	cout << "合併結束" << endl;
	cout << "time consume: " << (double)(clock() - start) / CLOCKS_PER_SEC << endl;
	cout << "刪除檔案中" << endl;
	for(int i = 0; i < delete_count; i++){
		ss << (i + 1);
		string tmp_file = fname + ss.str() + ".txt";
		char tmp_filec[tmp_file.size()+1];
		strcpy(tmp_filec,tmp_file.c_str());
		if(remove(tmp_filec) == 0)
			cout << "success"<<endl;
		else 
			cout << "fail"<<endl;
		ss.str("");
		ss.clear();
	}
	cout << "刪除結束" << endl;
	cout << "time consume: " << (double)(clock() - start) / CLOCKS_PER_SEC << endl;
}

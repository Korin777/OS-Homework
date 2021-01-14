#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include<sstream>
#include<algorithm> 
#include<unistd.h> 
#include <sys/stat.h>
#include <sys/types.h>
#include<map>
#include<time.h>
using namespace std;
#define SIZE 5000000
#define SIZEE 500000
#define MMSIZE 1000000

typedef struct {
    long long int key;
    char value[129];
} keyvalue;
bool compare(keyvalue a,keyvalue b) {
	return (a.key < b.key);
}


int main(int argc,char *argv[]) {

	clock_t start,end;
	start = clock();

    if(access("storage",F_OK ) != 0) {  //沒有storage資料夾的話創一個
        if(mkdir("storage",0777) == -1) {
            cout << "mkdir error" << endl;
        }
    }

    map<long long int,int>my_map[10];   //  用來記錄key儲存在 small memory 的哪個位置，加快找某個key值的速度
    map<long long int,string>mermory_map;
    map<long long int,int>::iterator iter;
    map<long long int,string>::iterator memory_iter;

    long long int find_front = -1,find_end = -1;    // find memory 的range
    bool dirty = false; // find memory 可能是錯的
    long long int key_number[10];

    FILE *fin,*fout = NULL;
    stringstream ss;

    int slash = -1,dot = 0;

    for(int i = 0; i < strlen(argv[1]); i++) {
        if(argv[1][i] == '/')
            slash = i;
        if(argv[1][i] == '.')
            dot = i;
    }
    char output_name[dot-slash-1+8];
    int p = slash + 1;
    int s;
    for( s = 0; s < dot-slash-1;s++) {
        output_name[s] = argv[1][p];
        p++;
    }
    output_name[s] = '.';
    s++;
    output_name[s] = 'o';
    s++;
    output_name[s] = 'u';
    s++;
    output_name[s] = 't';
    s++;
    output_name[s] = 'p';
    s++;
    output_name[s] = 'u';
    s++;
    output_name[s] = 't';
    s++;
    output_name[s] = '\0';

    fin = fopen(argv[1],"r");   //  input
    // fout = fopen(output_name,"w");     //  output
    char operation[4];  //PUT,GET,SCAN
    long long int key1,key2; 
    char value[129];
    keyvalue *kv_seek = (keyvalue*)malloc(sizeof(keyvalue)*1);
    keyvalue *kv[10];   //  small memory
    keyvalue *kvfind = (keyvalue*)malloc(sizeof(keyvalue)*SIZE);    //  large memory
    keyvalue *kvwr = (keyvalue*)malloc(sizeof(keyvalue)*SIZE);  //  當small memory要寫到disk，並排續disk資料時
    //用來讀disk資料的memory    //  read memory

    int kvfind_offset = -1; //  紀錄 large_memory 裡有效的資料有幾筆
    int kvfind_src = -1;    //  紀錄 large_memory 裡的資料室從disk哪個區域裡來的
    FILE *fin_arr[10];  
    int counter[10];    //  small memory 裡的有效資料數

    for(int i = 0; i < 10; i++) {
        key_number[i] = 0;
        kv[i] = (keyvalue*)malloc(sizeof(keyvalue)*SIZEE);
        for(int j = 0; j < SIZEE; j++)
            kv[i][j].key = -1;
        ss << i; 
        counter[i] = 0;
        string tmp_file = "storage/" + ss.str() + ".txt";
        char tmp_filec[tmp_file.size()+1];
        strcpy(tmp_filec,tmp_file.c_str());
        fin_arr[i] = fopen( tmp_filec , "r" );
        if(fin_arr[i] == 0) {   //檔案不存在的話，建立檔案
            fin_arr[i] = fopen(tmp_filec,"w+");
        }
        else {
            fseek (fin_arr[i], 0, SEEK_END);
            key_number[i] = ftell (fin_arr[i])/sizeof(keyvalue);
        }
        ss.str("");
        ss.clear();
    }

    while(fscanf(fin,"%s ",operation) != EOF) { // 讀取query  
        if(strcmp(operation,"PUT") == 0) {  //PUT KEY VALUE
            fscanf(fin,"%lld %s\n",&key1,value);
            int tmp = key1/1000000000000000000;
            if(find_front<=key1 && key1<=find_end)  // find memory 裡的 key-balue 可能被 update了
                dirty = true;

            memory_iter = mermory_map.find(key1);  //  找 memory map
            if(memory_iter != mermory_map.end()) { // key存在  更新 memory map
                string tmpstr(value);
                mermory_map[memory_iter->first] = tmpstr;
            }

            iter = my_map[tmp].find(key1);
            if(iter == my_map[tmp].end()) { // key不存在  
                my_map[tmp].insert(pair<long long int,int>(key1,counter[tmp]));
                strcpy(kv[tmp][counter[tmp]].value,value);
                kv[tmp][counter[tmp]].key = key1;
                counter[tmp]++;
            }
            else {                         // key存在，更新value
                strcpy(kv[tmp][iter->second].value,value);
                kv[tmp][iter->second].key = key1;
            }
            if(counter[tmp] == SIZEE) { //memory滿了，要寫到disk
                key_number[tmp] = 0;
                my_map[tmp].clear();
                sort(kv[tmp],kv[tmp]+SIZEE,compare);
                FILE *tmpfile = fopen("storage/tmp.txt","w");
                int offset = 0; //  尚未寫入 disk 的 small memory 位置
                bool smallmemory_eof = false;
                while(true) {   //排序 disk
                    int offsetwr = 0;   //  尚未寫入 disk 的 read memory 位置
                    size_t remaindata_num = fread(kvwr,sizeof(keyvalue),SIZE,fin_arr[tmp]);
                    if(remaindata_num == 0)   //  disk 資料全部讀完
                        break;
                    if(smallmemory_eof || kv[tmp][offset].key > kvwr[remaindata_num - 1].key) {
                        fwrite(kvwr,sizeof(keyvalue),remaindata_num,tmpfile);
                        key_number[tmp] += remaindata_num;
                    }
                    else {
                        int count = 0,countwr = 0;  //  要寫入幾筆資料
                        for(int i = 0; i < remaindata_num; ) {
                            if(kv[tmp][offset].key > kvwr[remaindata_num - 1].key) {    //read memory 的值都比 small memory 小，直接寫入 disk
                                fwrite(&kvwr[offsetwr],sizeof(keyvalue),remaindata_num-offsetwr,tmpfile);
                                key_number[tmp] += (remaindata_num-offsetwr);
                                offsetwr += (remaindata_num-offsetwr);
                                break;
                            }
                            if(kvwr[i].key <= kv[tmp][offset].key) {
                                if(kvwr[i].key == kv[tmp][offset].key) {    //  key相同，要更新disk裡的值
                                    strcpy(kvwr[i].value,kv[tmp][offset].value);
                                    offset++;
                                    if(offset == SIZEE) {
                                        smallmemory_eof = true;
                                        fwrite(&kvwr[offsetwr],sizeof(keyvalue),remaindata_num-offsetwr,tmpfile);
                                        key_number[tmp] += (remaindata_num-offsetwr);
                                        offsetwr += (remaindata_num-offsetwr);
                                        break;
                                    }
                                }
                                countwr++;
                                i++;
                            }
                            else {
                                fwrite(&kvwr[offsetwr],sizeof(keyvalue),countwr,tmpfile);
                                key_number[tmp] += countwr;
                                offsetwr += countwr;
                                countwr = 0;
                                for(int j = offset; j < SIZEE; j++) {
                                    if(kv[tmp][SIZEE-1].key<kvwr[i].key) {   // small memory 的值都比 read memory 小
                                        fwrite(&kv[tmp][offset],sizeof(keyvalue),SIZEE - offset,tmpfile);
                                        key_number[tmp] += (SIZEE - offset);
                                        fwrite(&kvwr[offsetwr],sizeof(keyvalue),remaindata_num-offsetwr,tmpfile);
                                        key_number[tmp] += (remaindata_num-offsetwr);
                                        offsetwr += (remaindata_num-offsetwr);
                                        offset = SIZEE;
                                        smallmemory_eof = true;
                                        break;
                                    }
                                    if(kv[tmp][j].key < kvwr[i].key) {
                                        count++;
                                    }
                                    else {
                                        fwrite(&kv[tmp][offset],sizeof(keyvalue),count,tmpfile);
                                        key_number[tmp] += count;
                                        offset += count;
                                        count = 0;
                                        break;
                                    }
                                }
                                if(smallmemory_eof)
                                    break;
                            }
                        }
                        if(offsetwr != remaindata_num) {
                            fwrite(&kvwr[offsetwr],sizeof(keyvalue),remaindata_num-offsetwr,tmpfile);
                            key_number[tmp] += (remaindata_num-offsetwr);
                            offsetwr += (remaindata_num-offsetwr);
                        }
                    }
                }
                if(offset != SIZEE)  {  // small memory 還沒全部寫入 disk
                    fwrite(&kv[tmp][offset],sizeof(keyvalue),SIZEE - offset,tmpfile);
                    key_number[tmp] += (SIZEE - offset);
                }
                ss << tmp; 
                string tmp_file = "storage/" + ss.str() + ".txt";
                char tmp_filec[tmp_file.size()+1];
                strcpy(tmp_filec,tmp_file.c_str());
                fclose(fin_arr[tmp]);
                fclose(tmpfile);   
                remove(tmp_filec);
                rename("storage/tmp.txt",tmp_filec);
                fin_arr[tmp] = fopen(tmp_filec,"r");
                ss.str("");
                ss.clear();
                counter[tmp] = 0;
            }
        }
        else if(strcmp(operation,"GET") == 0) {
            if(fout == NULL)
                fout = fopen(output_name,"w"); 
            bool find = false;
            fscanf(fin,"%lld\n",&key1);
            int tmp = key1/1000000000000000000;

            iter = my_map[tmp].find(key1);  //  找small memory
            if(iter != my_map[tmp].end()) { // key存在  
                fprintf(fout,"%s\n",kv[tmp][iter->second].value);
                find = true;
            }
            if(find)
                continue;

            memory_iter = mermory_map.find(key1);  //  找 memory map
            if(memory_iter != mermory_map.end()) { // key存在  
                fprintf(fout,"%s\n",(memory_iter->second).c_str());
                find = true;
            }


            // if(kvfind_src == tmp && !dirty) { //  要找的 key 可能在 find memory 裡
            //     if(kvfind_offset>0 && kvfind[0].key<key1 && key1<kvfind[kvfind_offset-1].key) {
            //         int middle,left = 0,right = kvfind_offset-1;
            //         // binary search
            //         while(left <= right) {
            //             middle = (right+left) / 2;
            //             if(kvfind[middle].key == key1) {
            //                 fprintf(fout,"%lld %s\n",key1,kvfind[middle].value);
            //                 find = true;
            //                 break;
            //             }
            //             else {
            //                 if(kvfind[middle].key < key1) {
            //                     left = middle + 1;
            //                 }
            //                 else {
            //                     right = middle - 1;
            //                 }
            //             }
            //         }
            //     }
            // }
            if(find)
                continue;

            // binary search disk
            long long int middle,left = 0,right = key_number[tmp]-1;
            while(left <= right) {
                middle = (right+left) / 2;
                fseek(fin_arr[tmp],sizeof(keyvalue) * middle,SEEK_SET);
                fread(kv_seek,sizeof(keyvalue),1,fin_arr[tmp]);
                if(kv_seek[0].key == key1) {
                    // kvfind_offset = fread(&kvfind[1],sizeof(keyvalue),SIZE-1,fin_arr[tmp]) + 1;
                    // kvfind[0].key = kv_seek[0].key;
                    // strcpy(kvfind[0].value,kv_seek[0].value);
                    // find_front = kvfind[0].key;
                    // find_end = kvfind[kvfind_offset].key;
                    // dirty = false;
                    // kvfind_src = tmp;
                    string tmpstr(kv_seek[0].value);
                    if(mermory_map.size() == MMSIZE) {
                        memory_iter = mermory_map.begin();
                        mermory_map.erase(memory_iter);
                    }
                    mermory_map.insert(pair<long long int,string>(key1,tmpstr));
                    fprintf(fout,"%s\n",kv_seek[0].value);
                    find = true;
                    fseek (fin_arr[tmp] , 0 , SEEK_SET);
                    break;
                }
                else {
                    if(kv_seek[0].key < key1) {
                        left = middle + 1;
                    }
                    else {
                        right = middle - 1;
                    }
                }
            }

            if(!find) { // key 不在 disk 裡
                fseek (fin_arr[tmp] , 0 , SEEK_SET);
                fprintf(fout,"EMPTY\n");
            }
        }
        else if(strcmp(operation,"SCAN") == 0) {    // 想當於多次的GET，但資料是連續的，因為資料都是排序過的，所以下一筆資料一定在上一筆資料後面
            if(fout == NULL)
                fout = fopen(output_name,"w"); 
            fscanf(fin,"%lld %lld\n",&key1,&key2);
            int smemory_pos = 0,lmemory_pos = 0;    
            // smemory_pos 為 smallmemory 的索引           lmemory_pos 為 find memory 的索引
            bool clean = false; // disk裡的資料是否是最新的，好像跟dirty一樣，有空再改@@
            for(long long int key = key1; key <= key2; key++) {
                bool find = false;
                int tmp = key/1000000000000000000;

                iter = my_map[tmp].find(key);  //  找small memory
                if(iter != my_map[tmp].end()) { // key存在  
                    fprintf(fout,"%s\n",kv[tmp][iter->second].value);
                    find = true;
                }

                if(find) {
                    if((key+1)/1000000000000000000!=tmp || key+1>key2) {    // 下個 key 在下個檔案或 這是最後一個 key，需要做一些初始化
                        smemory_pos = 0;
                        lmemory_pos = 0;
                        clean = false;
                        fseek (fin_arr[tmp] , 0 , SEEK_SET);
                    }
                    continue;
                }
                bool find_disk = true;  // 表示有沒有需要去disk裡找資料
                if(kvfind_src == tmp && !dirty) { 
                    if(kvfind_offset>0 && kvfind[0].key<key && key<kvfind[kvfind_offset-1].key) {
                        if(clean)
                            find_disk = false;
                        int middle,left = lmemory_pos,right = kvfind_offset-1;
                        // binary search
                        while(left <= right) {
                            middle = (right+left) / 2;
                            lmemory_pos = middle;
                            if(kvfind[middle].key == key) {
                                fprintf(fout,"%s\n",kvfind[middle].value);
                                find = true;
                                break;
                            }
                            else {
                                if(kvfind[middle].key < key) {
                                    left = middle + 1;
                                }
                                else {
                                    right = middle - 1;
                                }
                            }
                        }
                    }
                }
                if(find) {
                    if((key+1)/1000000000000000000!=tmp || key+1>key2) {
                        smemory_pos = 0;
                        lmemory_pos = 0;
                        clean = false;
                        fseek (fin_arr[tmp] , 0 , SEEK_SET);
                    }
                    continue;
                }

                if(find_disk) {
                // binary search disk
                    long long int middle,left = 0,right = key_number[tmp]-1;
                    while(left <= right) {
                        middle = (right+left) / 2;
                        fseek(fin_arr[tmp],sizeof(keyvalue) * middle,SEEK_SET);
                        fread(kv_seek,sizeof(keyvalue),1,fin_arr[tmp]);
                        if(kv_seek[0].key == key) {
                            kvfind_offset = fread(&kvfind[1],sizeof(keyvalue),SIZE-1,fin_arr[tmp]) + 1;
                            kvfind[0].key = kv_seek[0].key;
                            strcpy(kvfind[0].value,kv_seek[0].value);
                            find_front = kvfind[0].key;
                            find_end = kvfind[kvfind_offset].key;
                            dirty = false;
                            clean = true;
                            kvfind_src = tmp;
                            string tmpstr(kv_seek[0].value);
                            if(mermory_map.size() == MMSIZE) {
                                memory_iter = mermory_map.begin();
                                mermory_map.erase(memory_iter);
                            }
                            mermory_map.insert(pair<long long int,string>(key,tmpstr));
                            fprintf(fout,"%s\n",kv_seek[0].value);
                            find = true;
                            fseek (fin_arr[tmp] , 0 , SEEK_SET);
                            break;
                        }
                        else {
                            if(kv_seek[0].key < key) {
                                left = middle + 1;
                            }
                            else {
                                right = middle - 1;
                            }
                        }
                    }
                }
                if(!find) {
                    if((key+1)/1000000000000000000!=tmp || key+1>key2) {
                        smemory_pos = 0;
                        lmemory_pos = 0;
                        clean = false;
                        fseek (fin_arr[tmp] , 0 , SEEK_SET);
                    }
                    fprintf(fout,"EMPTY\n");
                }
            }
        }
        else
            continue;
    }
    // return 0;
    for(int tmp = 0; tmp < 10 ; tmp++) { 
        key_number[tmp] = 0;
        if(counter[tmp] == 0)
            continue;
            my_map[tmp].clear();
            sort(kv[tmp],kv[tmp]+counter[tmp],compare);
            FILE *tmpfile = fopen("storage/tmp.txt","w");
            int offset = 0; //  尚未寫入 disk 的 small memory 位置
            bool smallmemory_eof = false;
            while(true) {   //排序 disk
                int offsetwr = 0;   //  尚未寫入 disk 的 read memory 位置
                size_t remaindata_num = fread(kvwr,sizeof(keyvalue),SIZE,fin_arr[tmp]);
                if(remaindata_num == 0)   //  disk 資料全部讀完
                    break;
                if(smallmemory_eof || kv[tmp][offset].key > kvwr[remaindata_num - 1].key) {
                    fwrite(kvwr,sizeof(keyvalue),remaindata_num,tmpfile);
                    key_number[tmp] += remaindata_num;
                }
                else {
                    int count = 0,countwr = 0;  //  要寫入幾筆資料
                    for(int i = 0; i < remaindata_num; ) {
                        if(kv[tmp][offset].key > kvwr[remaindata_num - 1].key) {    //read memory 的值都比 small memory 小，直接寫入 disk
                            fwrite(&kvwr[offsetwr],sizeof(keyvalue),remaindata_num-offsetwr,tmpfile);
                            offsetwr += (remaindata_num-offsetwr);
                            key_number[tmp] += (remaindata_num-offsetwr);
                            break;
                        }
                        if(kvwr[i].key <= kv[tmp][offset].key) {
                            if(kvwr[i].key == kv[tmp][offset].key) {    //  key相同，要更新disk裡的值
                                strcpy(kvwr[i].value,kv[tmp][offset].value);
                                offset++;
                                if(offset == counter[tmp]) {
                                    smallmemory_eof = true;
                                    fwrite(&kvwr[offsetwr],sizeof(keyvalue),remaindata_num-offsetwr,tmpfile);
                                    key_number[tmp] += (remaindata_num-offsetwr);
                                    offsetwr += (remaindata_num-offsetwr);
                                    break;
                                }
                            }
                            countwr++;
                            i++;
                        }
                        else {
                            fwrite(&kvwr[offsetwr],sizeof(keyvalue),countwr,tmpfile);
                            key_number[tmp] += countwr;
                            offsetwr += countwr;
                            countwr = 0;
                            for(int j = offset; j < counter[tmp]; j++) {
                                if(kv[tmp][counter[tmp]-1].key<kvwr[i].key) {   // small memory 的值都比 read memory 小
                                    fwrite(&kv[tmp][offset],sizeof(keyvalue),counter[tmp] - offset,tmpfile);
                                    key_number[tmp] += (counter[tmp] - offset);
                                    fwrite(&kvwr[offsetwr],sizeof(keyvalue),remaindata_num-offsetwr,tmpfile);
                                    offsetwr += (remaindata_num-offsetwr);
                                    key_number[tmp] += (remaindata_num-offsetwr);
                                    offset = counter[tmp];
                                    smallmemory_eof = true;
                                    break;
                                }
                                if(kv[tmp][j].key < kvwr[i].key) {
                                    count++;
                                }
                                else {
                                    fwrite(&kv[tmp][offset],sizeof(keyvalue),count,tmpfile);
                                    key_number[tmp] += count;
                                    offset += count;
                                    count = 0;
                                    break;
                                }
                            }
                            if(smallmemory_eof)
                                break;
                        }
                    }
                    if(offsetwr != remaindata_num) {
                        fwrite(&kvwr[offsetwr],sizeof(keyvalue),remaindata_num-offsetwr,tmpfile);
                        key_number[tmp] += (remaindata_num-offsetwr);
                        offsetwr += (remaindata_num-offsetwr);
                    }
                }
            }
            if(offset != counter[tmp])  {  // small memory 還沒全部寫入 disk
                fwrite(&kv[tmp][offset],sizeof(keyvalue),counter[tmp] - offset,tmpfile);
                key_number[tmp] += (counter[tmp] - offset);
            }
            ss << tmp; 
            string tmp_file = "storage/" + ss.str() + ".txt";
            char tmp_filec[tmp_file.size()+1];
            strcpy(tmp_filec,tmp_file.c_str());
            fclose(fin_arr[tmp]);
            fclose(tmpfile);   
            remove(tmp_filec);
            rename("storage/tmp.txt",tmp_filec);
            // fin_arr[tmp] = fopen(tmp_filec,"r");
            ss.str("");
            ss.clear();
            counter[tmp] = 0;
                   
    }
    cout << "time consume: " << (double)(clock() - start) / CLOCKS_PER_SEC << endl;
    return 0;
}
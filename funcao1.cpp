#include <vector>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
typedef const char* V;
typedef std::vector<V> Record;

#define BYTE 8


int fixed_len_sizeof(Record *record){ 
        return  (sizeof(V) * record->size()) / BYTE;
}


void fixed_len_write(Record *record, unsigned char *buf){    
    unsigned const char *tmp;
    for(int i = 0; i < record->size() ; i++){
//                tmp = (unsigned const char*) (*record)[i];
//                buf[i]  = (unsigned char)tmp;

        strcpy((char*) buf,(*record)[0]);
    }
    printf("%s", buf); //(*record)[0]);

}

int main() {
    
    Record list;
    V a = (V) 'a';
    V b = (V) 'b';
    
    list.push_back(a);
    list.push_back(b);
//     void *uf;
    unsigned char *buf = (unsigned char*) malloc(sizeof(unsigned char)*fixed_len_sizeof(&list));
    
    fixed_len_write(&list, buf);
                    
//     printf(" %c %d",list.at(0), fixed_len_sizeof(&list));
    return 0;
}

/*
        V byte[BYTE];
        for(int i = 0 ; i < record.size() ; i++)
            for(int j = (BYTE - 1) ; j >=0 ; j--){
                *buf = (record[i] & (1 << j)) ? '1' : '0';
                 buf++;
            }*/
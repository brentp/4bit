#include "4bit.h"
//#include <stdio.h>
#include <fcntl.h>
#include "cdb.h"

/*
 * 4bit-cli fencode some.fasta # creates some.fasta.4bit, some.fasta.4bit.index 
 * 4bit-cli encode  < `cat somesequence` > some.4bit.bin 
 * 4bit-cli fdecode input.4bit.in start_position_in_bytes -e end_position_in_bytes ]
 * 4bit-cli decode < `cat some.bin` > somesequence.
 */
#define STR_EQ(a, b) (!strcmp((a), (b)))
#define CDB_ADD(db_ptr, k, v) cdb_make_add(db_ptr, k, strlen(k), v, strlen(v))

int pencode(void){
    unsigned char pair[3];
    pair[2] = '\0';
    while((pair[0] = getchar()) != '\n'){
        pair[1] = getchar();
        if(pair[1] == '\n') { pair[1] = '\0';};
        putchar(encode2chars(pair)); //printf("\n");
        if (pair[1] == '\0') { break; }
    }
    putchar('\0');
    return 0;
}

int pdecode(void){
    unsigned char encoded;
    unsigned char pair[3];
    pair[2] = '\0';
    while ((encoded = getchar()) != '\0'){
        if ((encoded == '\n')){ break; }

        if(encoded >= 255){ printf("NN"); }
        else {
            decode2chars(encoded, pair);
            putchar(pair[0]);
            putchar(pair[1]);
        }
    }
    return 0;
}

int cli_fencode(int argc, char **argv){
    char *fasta_name = argv[2];
    char *db_name = "asdf.db"; //strcpy(...);
    struct cdb_make db;
    int fd = open(db_name, O_RDWR | O_CREAT);
    //char *key, *val;

    cdb_make_start(&db, fd);
    CDB_ADD(&db, "hello", "goodbye");


    return 1;
}

int help(char *fname){
    printf("usage:%s COMMAND\n"
    "where COMMAND is one of:\n"
    "encode < some.fasta\t... or echo \"AC\" | ./4bit-cli encode\n"
    "decode < some.4bit\t... or echo \"-\" | ./4bit-cli decode\n"
    , fname);
    return 1;
}

int main(int argc, char **argv){
    if(argc < 2){ return help(argv[0]); }
    if(STR_EQ(argv[1], "encode")){ return pencode(); }
    if(STR_EQ(argv[1], "decode")){ return pdecode(); }
    if(STR_EQ(argv[1], "fencode")){ return cli_fencode(argc, argv); }

    return 1;
}

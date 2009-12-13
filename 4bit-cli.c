#include "4bit.h"
#include <getopt.h>
/*
 * 4bit-cli fencode some.fasta # creates some.fasta.4bit, some.fasta.4bit.index 
 * 4bit-cli encode  < `cat somesequence` > some.4bit.bin 
 * 4bit-cli fdecode input.4bit.in start_position_in_bytes -e end_position_in_bytes ]
 * 4bit-cli decode < `cat some.bin` > somesequence.
 */
#define STR_EQ(a, b) (!strcmp((a), (b)))

struct global_args {
} global_args;

const char *opt_string = "";

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
        decode2chars(encoded, pair);
        putchar(pair[0]);
        putchar(pair[1]);
    }
    return 0;
}

int help(char *fname){
    printf("usage:\n%s"
    "encode < `echo \"ACTG\"", fname);
    return 1;
}

int main(int argc, char *argv[]){
    if(argc < 2){ return help(argv[0]); }
    if(STR_EQ(argv[1], "encode")){ return pencode(); }
    if(STR_EQ(argv[1], "decode")){ return pdecode(); }

    return 1;
}

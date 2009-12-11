#include "4bit.h"
#include <stdio.h>
#include <stdlib.h>
#define TEST_FILE "afile.bin"

int error_count;
int test_count;

// test macros stolen from:
//  http://github.com/pkrumins/bithacks.h/blob/master/bithacks-test.c
#define TEST_OK(exp) do { \
    test_count++;\
    if (!(exp)) { \
        error_count++; \
        printf("Test '%s' at line %d failed.\n", #exp, __LINE__); \
    } } while(0)
 
#define TEST_END do { \
    if (error_count) { \
        printf("Testing failed: %d failed tests.\n", error_count); \
    } else { \
        printf("%i tests OK.\n", test_count); \
    } } while (0)

#define TEST_ENCODE_DECODE2(c2) do { \
           decode2chars(encode2chars((unsigned char *)c2), chars);\
           TEST_OK(!strcmp((char *)chars, (char *)c2)); \
       } while(0);

#define TEST_ENCODE_DECODE(c2) do { \
           decode(encode((unsigned char *)c2), char_str);\
           TEST_OK(!strcmp((char *)char_str, (char *)c2)); \
       } while(0);

#define PRINT_ENCODED(d, len) do { \
       int j=0; \
       do {\
         printf("d[%i]=%i\n", j, (int)d[j++]); \
         if (j >= len) { break; }\
       } while (1); } while(0);

void test_decoder(char *seq){
    unsigned char *encoded = encode((unsigned char *)seq);

    char *reseq = (char *)decode(encoded);   
    //printf("%s, %s\n", seq, reseq);

    TEST_OK(!strcmp(seq, reseq));
}

void test_struct(){
    FILE *fh;
    const f4bit *f4;
    char *seq = "ACC";
    size_t seq_len = strlen(seq);
    fh = fopen(TEST_FILE, "wb");
    size_t file_pos = ftell(fh);
    f4 = fencodes(fh, (unsigned char *)seq);
    // this tells us the file seek start/end positions
    // of the added seek in ->fh_start, ->fh_end resepctively.
    TEST_OK((seq_len + 1) / 2 == f4->fh_end - file_pos);
    // this tells us how many basepairs were added
    TEST_OK(seq_len == f4->bp_end - f4->bp_start);
    fflush(fh);
    free((void *)f4);
    fclose(fh);
    remove(TEST_FILE);

}

int test_odd_length_strings(){
    char *seq = "AGT\0\0\0\0";
    test_decoder(seq);

    seq = "AA\0A";
    test_decoder(seq);

    seq = "AAC\0A";
    test_decoder(seq);
    
    seq = "AaC\0A";
    test_decoder(seq);

    seq = "AAC";
    test_decoder(seq);

    seq = "A\0\0";
    test_decoder(seq);

    seq = "\0\0AAAAAAAAAAAAA";
    test_decoder(seq);

    return 1;
}

int test_encode_decode2chars(){

    unsigned char chars[3];
    unsigned char e2 = encode2chars((unsigned char *)"AA");

    decode2chars(e2, chars);
    TEST_ENCODE_DECODE2("\0A");
    TEST_ENCODE_DECODE2("A");
    TEST_ENCODE_DECODE2("AA");
    TEST_ENCODE_DECODE2("TA");
    TEST_ENCODE_DECODE2("AT");
    TEST_ENCODE_DECODE2("GC");
    TEST_ENCODE_DECODE2("CG");

    return 1;
}

void test_bad_chars(){
    
    char *seq = "ZZZAACCZZ";
    unsigned char *encoded = encode((unsigned char *)seq);
    size_t seq_len = strlen(seq);
    char *reseq = (char *)decode(encoded);
    TEST_OK(!strcmp("NNNAACCNN", reseq));
    free(reseq);

    char *seq2 = "ZZ";
    encoded = encode((unsigned char *)seq2);
    seq_len = strlen(seq2);
    reseq = (char *)decode(encoded);
    TEST_OK(!strcmp("NN", reseq));

}

int test_encode_decode(){
    unsigned char chars[3];
    unsigned char posns[10] = {24, 25, 18, 28, 18, 18, 30, 22, 24, 0};
    decode2chars(8, chars);

    char *seq = (char *)decode((unsigned char*)posns);

    // first 2 and last 2 letters are the sam.e
    TEST_OK(seq[0] == seq[16]);
    TEST_OK(seq[1] == seq[17]);

    TEST_OK(strlen(seq) == 9 * 2);
    const unsigned char *encoded = encode((unsigned char *)seq);
    unsigned int i;
    for(i=0; i < 9; i++){
        TEST_OK(encoded[i] == posns[i]);
    }
    free((void *)seq);
    free((void *)encoded);

    return 1;
}

void test(){
    unsigned char chars[3];
    decode2chars(1, chars);
}
void _test_read_write(char *seq, char mode[2]){
    FILE *fh;
    size_t seq_len = strlen(seq);
    fh = fopen(TEST_FILE, mode);
    size_t file_pos = ftell(fh);
    size_t pos = fencode(fh, (unsigned char *)seq);
    TEST_OK((seq_len + 1) / 2 == pos - file_pos);
    fflush(fh);
    fclose(fh);

    
    fh = fopen("afile.bin", "rb");
    fseek(fh, 0, 0);
    char *read_seq = (char *)fdecode(fh, file_pos * 2, pos * 2);
    TEST_OK(!strcmp(seq, read_seq));
    free(read_seq);
    fclose(fh);
}

void test_file(){
    _test_read_write("TCCGG", "wb");
    _test_read_write("A", "ab");
    _test_read_write("ACCGCCTC", "ab");

    _test_read_write("ACCcCCTC", "ab");

    _test_read_write("C", "ab");
    _test_read_write("CCCCCCccaaccCCaaCCaCaGAAAAAAAAAAAAAAACC", "ab");
    _test_read_write("", "wb");
    _test_read_write("A", "wb");
    // cleanup.
    remove(TEST_FILE);

}


int main(){
    test_count = 0;
    test();
    test_encode_decode2chars();
    test_encode_decode();
    test_odd_length_strings();
    test_file();
    test_bad_chars();

    TEST_END;

    return 1;
}

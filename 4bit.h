/*
 * 4bit encoding of DNA sequence with alphabet of: "-ACGNTXacgntx" other
 * characters are encoded as 'N'. The encoding works by putting 2 chars
 * into a single unsigned char. Since there are 255 unsigned chars, and
 * only 12 * 12 = 144 pairs in our alphabet, this works simply.
 * The code is contained entirely in this file: '4bit.h' with the API
 * function signatures defined and documented just below here.
 *
 * This code is release under the MIT license.
 * Brent Pedersen (bpederse@gmail.com)
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>


/* 
 * `encode`: encode a dna sequence string into a string of unsigned chars.
 * the returned value must be free'd.  
 * */
unsigned char *encode(const unsigned char *dna_seq);

/* 
 * `fencode`: encode the given dna_seq string and write it to the file.
 * the return value is the end fseek position of the written, encoded
 * sequence 
 * */
size_t fencode(FILE *fh, unsigned char *seq);

/*
 * `decode`: decode an encoded value into it's dna sequence
 * string. the return value must be free`d
 */
const unsigned char *decode(const unsigned char *positions);

/*
 * `fdecode`: return the dna sequence string encoded in the file
 * at (0-based) file positions fh_start to fh_end inclusive.
 * the return value must be free'd.
 */
const char *fdecode(FILE *fh, size_t fh_start, size_t fh_end);

// see `fencodes`
typedef struct f4bit {
    size_t bp_start; // 0 -based
    size_t bp_end; // 0 -based
    size_t fh_start;
    size_t fh_end;
} f4bit;

/*
 * `fencodes`: identical to `fencode` except the return value is
 * a f4bit struct pointer containing the base-pair and file-handle
 * start and end of the data -- useful for creating an index to save
 * where particular chromosomes start and stop within the file.
 * the return value must be free`d.
 */
const f4bit* fencodes(FILE *fh, unsigned char *seq);


// THESE 2 functions are most internal but may be useful for
// incremental encoding...

/*
 * `decode2chars`: encode 2 basepairs into a single unsigned
 * char which is returned.
 */
inline unsigned char encode2chars(unsigned char pair[3]);

/*
 * `decode2chars`: decode the unsigned char into the chars[] array
 * (which is sent to the function). the chars[] array will be filled
 * with the dna sequence.
 */
inline void decode2chars(unsigned char encoded, unsigned char chars[3]);


/////////////////////////////////////////////////////////////////////////////
/***************************************************************************/
// INTERNAL: use only the api functions defined above.                     //
/***************************************************************************/
/////////////////////////////////////////////////////////////////////////////
//
#define DNA_LEN 14
#define NIDX 5
#define DEBUG 0
#define CONST_FREE(x) free((void *)(x))

/* ordered alphabetically. 'Z' means end of string. */
const unsigned char DNA[] = "\0-ACGNTXacgntx";
//                            01234567890123

inline unsigned char dnapos(unsigned char p){
    unsigned char i = -1;
    while (p > DNA[++i]){}
    if(DNA[i] != p){ 
#if DEBUG
        printf("bad char:%c\n replacing with N", p);
#endif
        i = NIDX;
    }
    return i;
}

inline unsigned char encode2chars(unsigned char pair[3]){
    
    unsigned char pos0 = dnapos(pair[0]);
    unsigned char pos1 = dnapos(pair[1]);
    return (pos0 * DNA_LEN + pos1) ;
}

unsigned char *encode(const unsigned char *seq){
    // handle odd chars here.
    size_t seq_len = strlen((char *)seq);
    unsigned char *encoded = \
                (unsigned char *)malloc(sizeof(unsigned char) * ((seq_len + 1) / 2 + 1));
    unsigned int i = 0;
    unsigned char pair[3];
    pair[2] = '\0';
    for(i=0;i<seq_len;i+=2){
        pair[0] = seq[i];
        pair[1] = seq[i + 1];
        encoded[i / 2] = encode2chars(pair);
    }
    //encoded[i - (int)((seq_len + 1) % 2)] = '\0';
    encoded[(i + 1)/ 2] = '\0';
    return encoded;
}

size_t fencode(FILE *fh, unsigned char *seq){
    const unsigned char *encoded = encode(seq);
    unsigned int seq_len = strlen((char *)seq);
    // division truncates. so add 1.
    unsigned int n = (seq_len + 1) / 2;
    fwrite(encoded, sizeof(unsigned char), n, fh);
    CONST_FREE(encoded);
    // returns the encoded position in the file (double to conver to)
    return ftell(fh);
}

const f4bit *fencodes(FILE *fh, unsigned char *seq){
    f4bit *f4;
    f4 = (f4bit *)malloc((sizeof(f4bit)));
    const unsigned char *encoded = encode(seq);
    unsigned int seq_len = strlen((char *)seq);
    f4->fh_start = ftell(fh);
    f4->bp_start = (f4->fh_start + 1) / 2;
    // division truncates. so add 1.
    unsigned int n = (seq_len + 1) / 2;
    fwrite(encoded, sizeof(unsigned char), n, fh);
    CONST_FREE(encoded);
    // returns the encoded position in the file (double to conver to)
    f4->fh_end = ftell(fh);
    f4->bp_end = f4->bp_start + seq_len;
    return f4;
}

inline void decode2chars(unsigned char encoded, unsigned char chars[3]){
    if(encoded > 255){ encoded = 255; } // TODO: fix this.
    chars[0] =  DNA[encoded / DNA_LEN];
    chars[1] =  DNA[encoded % DNA_LEN];
    chars[2] = '\0';
}

const unsigned char *decode(const unsigned char *positions){
    size_t n_positions = strlen((char *)positions);
    unsigned char *seq = (unsigned char *)malloc(\
            sizeof(unsigned char) * (n_positions * 2 + 1));

    unsigned pos, i;
    for(i=0; i < 2 * n_positions + 1; i+=2){
        pos = positions[i/2];
        seq[i] = DNA[pos / DNA_LEN];
        seq[i + 1] = DNA[pos % DNA_LEN];
    }
    seq[i] = '\0';
    return seq;
}

const char *fdecode(FILE *fh, size_t fh_start, size_t fh_end){

    size_t n = fh_end - fh_start;
    if(n == 0){
        char * s = (char *)malloc(sizeof(char *) * 1);
        s[0] = '\0';
        return s;
    }
    unsigned char *encoded = (unsigned char *)(malloc(\
                sizeof(unsigned char) * (n + 1)));

    // seek to fh_start
    fseek(fh, fh_start, SEEK_SET);
    // read the desired amount
    fread(encoded, sizeof(unsigned char), n, fh);
    char *seq = (char *)decode(encoded);
    free(encoded);
    return seq;
}

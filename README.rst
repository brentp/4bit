========================================
4bit: binary dna sequence format library
========================================

:Author: Brent Pedersen (brentp)
:Email: bpederse@gmail.com
:License: MIT

.. contents ::

About
=====

4bit encoding of DNA sequence with alphabet of: "-ACGNTXacgntx" other
characters are encoded as 'N'. The encoding works by putting 2 chars
into a single unsigned char. Since there are 255 unsigned chars, and
only 12 * 12 = 144 pairs in our alphabet, this works simply.
The code is contained entirely in the file: '4bit.h' with the API
function signatures defined and documented below.

API
===

encode
------
encode a dna sequence string into a string of unsigned chars.
the returned value must be free'd.  
::

    const unsigned char *encode(unsigned char *dna_seq);


`fencode`
---------
encode the given dna_seq string and write it to the file.
the return value is the end fseek position of the written, encoded
sequence 
::

    size_t fencode(FILE *fh, unsigned char *seq);

`decode`
--------
decode an encoded value into it's dna sequence string. 
the return value must be free`d
::

    const unsigned char* decode(unsigned char *positions);

`fdecode`
---------
return the dna sequence string encoded in the file at (0-based) file 
positions fh_start to fh_end inclusive. the return value must be free'd.
::
    
    const char* fdecode(FILE *fh, size_t fh_start, size_t fh_end);

`f4bit struct`
--------------
::

    typedef struct f4bit {
        size_t bp_start; // 0 -based
        size_t bp_end; // 0 -based
        size_t fh_start;
        size_t fh_end;
    } f4bit;

`fencodes`
----------
identical to `fencode` except the return value is an `f4bit struct`_
pointer containing the base-pair and file-handle start and stop of
the data. useful for creating an index to save where particular
chromosomes start and stop within the file. the return value must be free`d.
::

    const f4bit* fencodes(FILE *fh, unsigned char *seq);

CLI
===
in progress. see 4bit-cli.c and test.sh

Development
===========
Please report any bugs, suggestions or code improvements. 
For example use and tests, see the `test-4bits.c` file.

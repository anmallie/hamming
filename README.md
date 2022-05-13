# Huffman (8, 4) Error Corrector

Encode and decode data using systematic Hamming(8,4) code, with one bit error correction and two bit error detection.  
Bytes are encoded through the bit matrix multiplication of nibbles with a generator matrix.  
These resulting codes can be decoded and checked for errors through multiplication with the transposed parity check matrix.

### Note
The code in this repository was created for Prof. Long's CSE-13s class at UCSC.

The following files were given for the assignment and are not my work:
- error.c
- entropy.c
- Header files {\*.h}

The rest of the code is my work unless stated otherwise.


## Files

* bv.{c, h}
  - Implementation of bit vector ADT.

* bm.{c, h} 
  - Implementation of the bit matrix ADT.

* hamming.{c, h}
  - Implementation of Hamming decoding and encoding logic.

* encode.c
  - Encoder program.

* decode.c 
  - Decoder program.

* error.c 
  - Program given by Prof. Long that injects errors into data.

* entropy.c
  - Program given by Prof. Long that calculates the entropy of data.

* Makefile

## Makefile

### Build
        $ make {all, encode, decode, error, entropy}
### Clean
	$ make clean

## Running

        $ ./encode -[h] -[i input] -[o output]
        $ ./decode -[h] -[v] -[i input] -[o output]

### Options

	-h           Prints help message to stdout.

	-v           (decode only) Enables verbose printing, will output error statistics to stderr.

	-i infile    Specify data input file. (default: stdin)

	-o outfile   Specify data output file. (default: stdout)

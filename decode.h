#ifndef DECODE_H
#define DECODE_H
#include <stdio.h>

#include "types.h" // Contains user defined types

/*
 * Structure to store information required for
 * decoding secret file from source Image
 * Info about magic string and output(secret file) 
 * also stored
 */

typedef struct _DecodeInfo
{
    /* Source Image info */
    char *output_image_fname; // To store the src image name
    FILE *fptr_output_image;  // To store the address of the src image
    

    /* Secret File Info */
    char *secret_fname;       // To store the secret file name
    FILE *fptr_secret;        // To store the secret file address
    char secret_data[100];    // To store the secret data
    long size_secret_file_extn;    // To store the size of the secret file extension
    long size_secret_data;   //To store the size of secret data
    

    /* Stego Image Info 
    char *stego_image_fname; // To store the dest file name
    FILE *fptr_stego_image;  // To store the address of stego image*/

} DecodeInfo;

/* Encoding function prototype */

/* Read and validate Encode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the encoding */
Status do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status open_files_decode(DecodeInfo *decInfo);

/* Store Magic String */
Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo);

/*Encode extension size*/
Status decode_secret_file_extn_size(DecodeInfo *decInfo);

/* Encode secret file extenstion */
Status decode_secret_file_extn(DecodeInfo *decInfo);

/* Encode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo);

/* Encode secret file data*/
Status decode_secret_file_data(DecodeInfo *decInfo);

/* Encode a byte into LSB of image data array */
Status decode_byte_to_lsb(char *image_buffer,char *data);

// Encode a size to lsb
Status decode_size_to_lsb(char *imageBuffer,int *data);


#endif

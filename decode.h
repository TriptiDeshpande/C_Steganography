#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types
#include <string.h>

/* 
 * Structure to store information required for
 * decoding secret file from stego Image
 * Info about output and intermediate data is
 * also stored
 */

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)


typedef struct _DecodeInfo
{
    /* Stego Image Info */
    char *d_stego_image_fname;
    FILE *fptr_d_stego_image;
    char decode_image_data[MAX_IMAGE_BUF_SIZE];
    char *d_data;
   

    /* Output File Info */
    char *decoded_fname;
    FILE *fptr_decoded;

    /* Secret File Info */
    int secret_file_size;
   
   
   

} DecodeInfo;

/* Decoding function prototype */


/* Read and validate Decode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status open_files_for_decoding(DecodeInfo *decInfo);

/* Decode Magic String */
Status decode_magic_string(DecodeInfo *decInfo);

/* Decode secret file extension size */
Status decode_secret_file_extn_size(DecodeInfo *decInfo);

/* Decode secret file extension */
Status decode_secret_file_extn(DecodeInfo *decInfo);

/* Decode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo);

/* Decode secret file data */
Status decode_secret_file_data(int size, DecodeInfo *decInfo);

/* Decode function, which does the real decoding */
Status decode_data_from_image(int size, DecodeInfo *decInfo);

/* Decode a byte from LSB of image data array */
char decode_byte_from_lsb(char data, char *image_buffer);

/* Decode the integer from the LSB */
int decode_size_from_LSB(char *buffer);

/* Perform the decoding */
Status do_decoding(DecodeInfo *decInfo);



#endif





#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "decode.h"
#include "types.h"
#include "common.h"

/* Function Definitions */


/* 
Read and validate the Decoding arguments from command line
*Input: Command line arguments(argv) and a pointer to DecodeInfo structure
*Output: Reads and sets the stego image filename and required decoded output file name
*Description: his function checks if the prvided arguments specify a valid stego image file
(with a .bmp extension) and assigns it to the DecodeInfo structure. 
 */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    if (argv[2] != NULL && strcmp(strstr(argv[2], "."), ".bmp") == 0)
    {
        decInfo->d_stego_image_fname = argv[2]; //Sets stego image filename for decoding
    }
    else
        return d_failure;
	
   if(argv[3] != NULL)
    {
        decInfo->decoded_fname = argv[3]; //Sets decoded secret filename if provided
    }
    else
        decInfo->decoded_fname = "decoded_secret.txt"; //Automatically assigns default filename if none provided

    return d_success;
}

/* Open files 
*Input: Stego image and decoded secret file
*Output: Opens above files for decoding 
*Description: Opens stego image for reading and the output file (decoded_secret.txt) for writing 
the decoded data.
*/
Status open_files_for_decoding(DecodeInfo *decInfo)
{
    // Stego Image file
    decInfo->fptr_d_stego_image = fopen(decInfo->d_stego_image_fname, "rb"); 
    
    // Do Error handling
    if (decInfo->fptr_d_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->d_stego_image_fname);
        return d_failure;
    }

    // Decoded Secret file
    decInfo->fptr_decoded = fopen(decInfo->decoded_fname, "wb");
    
    // Do Error handling
    if (decInfo->fptr_decoded == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->decoded_fname);
        return d_failure;
    }
    return d_success;
}

/* Decode Magic String
*Input: DecodeInfo structure
*Output: Status - d_success if decoding is performed successfully or else d_failure
*Description - Seeks to the predefined position in the stego image file
to extract the magic string, which indicates the start of the hidden data.
*/
Status decode_magic_string(DecodeInfo *decInfo)
{
        //Seek to the offset after BMP Header(54 bytes)
	fseek(decInfo -> fptr_d_stego_image, 54, SEEK_SET);
        
     	//Decode data from image based on the length of MAGIC STRING
	decode_data_from_image(strlen(MAGIC_STRING), decInfo);


        return d_success;

}


/* Decode Data from Image
*Input: Number of bytes(size) to decode and DecodeInfo Structure
Output: Decodes the data from the image
Description: Reads the specified number of bytes from the stego image file 
and decodes them into the hidden data. This function manages memory allocation for the decoded
data and processes each byte.
 */
Status decode_data_from_image(int size, DecodeInfo *decInfo)
{

      //Allocate memory for decoded datam including a null terminator.
      decInfo -> d_data = (char *)calloc((size +1), 1);

      //Do error handling
      if(decInfo -> d_data == NULL)
	{
		fprintf(stderr, "Error: Unable to allocate memory for decoded data.\n");
		return d_failure;
	}

       for (int i = 0; i < size; i++)
       {

	//Reads 8 bytes of data from stego.bmp
        size_t byte_read = fread(decInfo -> decode_image_data, 1, 8 , decInfo -> fptr_d_stego_image); 
	
	//Do error handling 
	if(byte_read != 8)
	{
		fprintf(stderr, "Error: Insufficient data read from stego image.\n"); 
		free(decInfo -> d_data);
		return d_failure;
	}

	//Decode each byte using the LSB method
	decInfo -> d_data[i] = decode_byte_from_lsb(decInfo -> d_data[i], decInfo -> decode_image_data);
       }
	
	//Null terminate the decoded string
       decInfo -> d_data[size] = '\0';

	//Check if the decoded data matches expected values.
	if((strcmp(decInfo -> d_data, MAGIC_STRING) == 0) || (strcmp(decInfo -> d_data, ".txt") == 0))
		return d_success;
	
	//Free memory if no match
	free(decInfo -> d_data);

	return d_failure;

}


/* 
Decode byte from LSB
*Input: Byte to decode and buffer contains the bits from stego image
Output: Decoded bytes
Description: Decodes byte by byte by extracting bits from the LSB of the provided image buffer.
This allows reconstruction of hidden data bit by bit. 
 */
char decode_byte_from_lsb(char data, char *image_buffer)
{
 	//Iterate through each bit in the byte
	for(int i = 0; i < 8; i++)
	{
		data <<= 1; //Shift the existing data to the left
		data |= (image_buffer[i] & 1); //Add the LSB from the image buffer
	}
	
	return data; //Return the reconstructure byte.

}


/* 
Decode the size(integer) from the LSB 
*Input: Buffer containing the bits representing the size
Output: Decoded integer size
Description: Decodes an integer size value by reading the LSB from a buffer.
This is crucialfor understanding how much data to expect in subsequent reads.
*/
int decode_size_from_LSB(char *buffer)
{

   int decoded_size = 0;
   
   //Loop through each bit position to reconstruct or decode the integer
   for(int bit_position = 0; bit_position < 32; bit_position++)
   {
	/*Extracts the LSB from the buffer and shifts it to its proper position in the integer,
	using a bitwise OR (|) to combine the bits */
	decoded_size |= ((buffer[bit_position] & 0x01) << (31 - bit_position));
   }

   return decoded_size;//Return the decoded size
}


/* Decode secret file extension size 
*Input: DecodeInfo Structure
Output: Decodes the secret file extension size
Description: Reads a fixed number of bytes(32) from the stego image to determine
the size of the secret file's extension. Utilizes the decode_size_from_LSB function to 
interpret these bits correctly
 */
Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
   //Buffer to hold the raw bytes read from the stego image
   char buffer[32];
   
   //Read 32 bytes from the stego image
   size_t bytes_read = fread(buffer, 1, 32, decInfo->fptr_d_stego_image);
   printf("Bytes read: %zu\n", bytes_read);

   //Do error handling
   if(bytes_read != 32)
   {
	fprintf(stderr, "Error: Failed to read 32 bytes for secret file extension size\n");
	return d_failure;
   }

   //Decode the size of the secret file extension from the buffer using LSB method
   decInfo -> secret_file_size = decode_size_from_LSB(buffer);
   printf("Decoded secret file extension size: %d bytes\n", decInfo->secret_file_size);
  
 return d_success;

}

/* Decode secret file extension 
*Input: DecodeInfo Structure
Output: Decodes the secret file extension
Description: Decodes the secret file extension from the stego image by
reading the number of bytes that represent the extension.
*/
Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    //Define the expected length of the secret file extension
    size_t extn_size = strlen(".txt");

    //Call function to decode the secret file extension from stego image
    if(decode_data_from_image(extn_size, decInfo) == d_success)
    {
	printf("Decoded secret file extension: %s\n", decInfo -> d_data);
	return d_success;
    }


    return d_failure;
 
}


/* Decode secret file size 
*Input: DecodeInfo Structure
Output: Decodes the secret file size
Description: Reads the secret file size encoded in the LSB of the steog image
by extracting the specified number of bits(32) that represent the size.
*/
Status decode_secret_file_size(DecodeInfo *decInfo)
{
    //Buffer to hold the size data read from stego image
    char str[32];

    //Read 32 bytes for file size
    size_t bytes_read = fread(str, 1, 32, decInfo->fptr_d_stego_image);
    
    //Do error handling
    if(bytes_read != 32)
    {
	fprintf(stderr, "Error: Failed to read 32 bytes for secret file size\n");
	return d_failure;
    }
     
    //Call function to decode the file from read bytes using LSB method
    decInfo -> secret_file_size = decode_size_from_LSB(str);
    printf("Decoded secret file size: %d bytes\n", decInfo->secret_file_size);

    return d_success;
   
}

/* Decode secret file data 
*Input: Size of secret file data to be decoded and DecodeInfo Structure
Output: Decodes the secret file data
Description: Retrives the actual secret data embedded in the stego image by reading
8 bits and decoding them from their LSB. 
*/
Status decode_secret_file_data(int size, DecodeInfo *decInfo)
{
    // Allocate memory for the secret data, including error handling
    char *decoded_data = (char *)malloc(size);
    if (!decoded_data)
    {
        fprintf(stderr, "Error: Memory allocation failed\n");
        return d_failure;
    }

    // Buffer to hold the raw data read from the stego image
    char buffer[8];
    size_t bytes_read;

    // Read the data in chunks of 8 bytes and decode in one go
    for (int i = 0; i < size; i++)
    {
        // Read 8 bytes from the stego image into the buffer
        bytes_read = fread(buffer, sizeof(char), sizeof(buffer), decInfo->fptr_d_stego_image);
        if (bytes_read < sizeof(buffer) && ferror(decInfo->fptr_d_stego_image))
        {
            fprintf(stderr, "Error: Failed to read from stego image\n");
            free(decoded_data);
            return d_failure;
        }

        // Decode the byte using LSB method and store it directly in the decoded data 
        decoded_data[i] = decode_byte_from_lsb(0, buffer);
    }

    // Write the decoded secret data to the output file
    fwrite(decoded_data, sizeof(char), size, decInfo->fptr_decoded);

    // Free the allocated memory for secret data
    free(decoded_data);

    return d_success; 
}


/* Perform the decoding 
*Input: DecodeInfo Structure
Output: Executes a series of decoding operations
Description: Calls the necessary functions to open files, decode magic string and secret file data,
and finally retrive the embedded secret file messages.
*/
Status do_decoding(DecodeInfo *decInfo)
{
    //Calling functions for decoding
    if (open_files_for_decoding(decInfo) == d_success)
    {
        printf("Successfully opened all the files\n");
	
        if (decode_magic_string(decInfo) == d_success)
        {
            printf("Magic string decoded successfully\n");
	     
            if (decode_secret_file_extn_size(decInfo) == d_success)
            {
                printf("Decoded secret file extension size successfully\n");

                if (decode_secret_file_extn(decInfo) == d_success)
                {
                    printf("Decoded secret file extension successfully\n");

                    if (decode_secret_file_size(decInfo) == d_success)
                    {
                        printf("Decoded secret file size successfully\n");

                        if (decode_secret_file_data(decInfo -> secret_file_size, decInfo) == d_success)
                        {
                            printf("Decoded secret file data successfully\n");
                        }
                        else
                        {
                            printf("ERROR: Failed to decode secret file data\n");
                            return d_failure;
                        }
                    }
                    else
                    {
                        printf("ERROR: Failed to decode secret file size\n");
                        return d_failure;
                    }
                }
                else
                {
                    printf("ERROR: Failed to decode secret file extension\n");
                    return d_failure;
                }
            }
            else
            {
                printf("ERROR: Failed to decode secret file extension size\n");
                return d_failure;
            }
        }
        else
        {
            printf("ERROR: Magic string was not decoded\n");
            return d_failure;
        }
    }
    else
    {
	
        printf("ERROR: Failed to open the required files\n");
        return d_failure;
    }

    return d_success;
}






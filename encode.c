/*
Name : Tripti Deshpande
Student id - 24001_003
Date : 23/09/2024
*/


#include <stdio.h>
#include "encode.h"
#include "common.h"
#include "types.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "rb");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "rb");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "wb");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}


/*
Read and Validate command line arguments
* Input: Command line arguments and EncodeInfo structure
*Output: Sets the source image, secret file, and stego image filenames
*Description: Reads and validates the filenames. 
*/
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
	if(argv[2] != NULL && strcmp(strstr(argv[2], "."), ".bmp") == 0)
	{
		encInfo -> src_image_fname = argv[2]; //Set source image filename
	}
	else
		return e_failure;
	if(argv[3] != NULL && strcmp(strstr(argv[3], "."), ".txt") == 0)
	{
		encInfo -> secret_fname = argv[3]; //Set secret file filename
	}
	else
		return e_failure;
	if(argv[4] != NULL)
	{
		encInfo -> stego_image_fname = argv[4]; //Set the stego image filename
	}
	else
		encInfo -> stego_image_fname = "stego.bmp";  //Default name if not provided

return e_success;
}

/* 
Check capacity
Input: EncodeInfo Structure
OUtput: e_success if there is enough capacity, e_failure otherwise
Description: Check if the image can hold the secret data
*/
Status check_capacity(EncodeInfo *encInfo)
{
	//Size of source image (beautiful.bmp)
	encInfo -> image_capacity = get_image_size_for_bmp(encInfo -> fptr_src_image);  

	//Size of secret file (secret.txt)
	encInfo -> size_secret_file = get_file_size(encInfo -> fptr_secret);  
	
	//Check capacity
	if(encInfo -> image_capacity > (54 + 16 + 32 + 32 + 32 + (encInfo -> size_secret_file * 8))) 
		return e_success;
	else
		return e_failure;
}

/*
Get File Size
* Input: FILE pointer to secret file
*Output: Size of the file in bytes
*Description: Moves the file pointer to the end of the file,
and then uses ftell to get the current position, which is the file size.
*/

uint get_file_size(FILE *fptr_secret)
{
	//Move to end of the file
	fseek(fptr_secret, 0, SEEK_END); 

	//Get the size of the file
	ftell(fptr_secret);

}

/*
Copy BMP Header
* Input: FILE pointer for the source and stego images
*Output: Copies the BMP header from the source image to the stego image
*Description: Reads the first 54 bytes (BMP header) from the
source image and writes them to the stego image. 
*/
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_stego_image)
{
	//Buffer to hold the header (54 bytes for BMP)
	char headerstr[54];

	//Move to the start of the source image 
	fseek(fptr_src_image, 0, SEEK_SET);

	//Reader header into the buffer
	fread(headerstr, 1, 54, fptr_src_image);

	//Write the header to the stego image
	fwrite(headerstr,1, 54, fptr_stego_image);

return e_success;
}


/*
Encode magic string
* Input: Magic string and EncodeInfo structure
*Output: Encodes the magic string into the stego image
*Description: Calls the function to encode data into the image. 
*/
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
	//Call the function to encode magic string data into the stego image
	encode_data_to_image(magic_string, strlen(magic_string), encInfo);
	return e_success;

}


/*
Encode data to image
* Input: Data to encode, size of the data, and EncodeInfo structure
*Output: e_success or e_failure based on encoding success
*Description: Encodes each byte of data into the LSBs of the image buffer. 
*/
Status encode_data_to_image(const char *data, int size, EncodeInfo *encInfo)
{
	for(int i =0; i < size; i++)
	{
	//Read 8 bytes of image data(RGB pixel data) one at a time encoding
	size_t bytes_read = fread(encInfo -> image_data, 1, 8, encInfo -> fptr_src_image);
   	
	//Do error handling
   	if(bytes_read != 8)
   	{
		fprintf(stderr, "Error: Failed to read 8 bytes of data from beautiful.bmp\n");
		return e_failure;
   	}

	//Call function to encode 1 byte of data into the LSBs
	encode_byte_to_lsb(data[i], encInfo -> image_data); 

	//write the encoded bytes back to stego.bmp
	fwrite(encInfo -> image_data, 1, 8, encInfo -> fptr_stego_image);
	}
	return e_success;	

}

/*
Encode byte to LSB
* Input: Data byte and image buffer
*Output: e_success after encoding
*Description: Modifies the LSB of each byte in the image buffer to 
hide the data byte.
*/
Status encode_byte_to_lsb(char data, char *image_buffer)
{
	for(int i = 0; i < 8; i++)
	{	
	//Modify the LSB of each byte to hide the data
	//0XFE(hexadecimal respresentation of 11111110) clears the LSB 
	//(data >> (7 - i)) & 1) - extracts the ith bit from data byte
	//'|' (bitwise OR) sets it to the value of the corresponding bit of data
	image_buffer[i] = (image_buffer[i] & 0xFE | ((data >> (7 - i)) & 1)); 
	}
	return e_success;
}


/*
Encode secret file extension size
* Input: Size of the secret file extension and EncodeInfo structure
*Output: Encodes the size into the LSBs of the BMP image
*Description: Reads 32 bytes from the source image and encodes the 
secret file extension size into the LSBs of these bytes. 
*/
Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo)
{
	
	char str[32];
	
	//Read 32 bytes from the source image to encode the size
	size_t bytes_read = fread(str, 1, 32,  encInfo -> fptr_src_image);
   	
	//Do error handling
   	if(bytes_read != 32)
   	{
		fprintf(stderr, "Error: Failed to read 32 bytes for secret file extension size\n");
		return e_failure;
   	}

	//Call function to encode the size into LSBs
	encode_size_to_LSB(size, str);

	if(encode_size_to_LSB(size, str) != e_success)
	{
		fprintf(stderr,"ERROR: Failed to encode size to LSB\n");
		return e_failure;
	}
	
	//Write the encoded size to the stego image
	fwrite(str, 1, 32,  encInfo -> fptr_stego_image);
	return e_success;
}


/*
Encode secret file extension
* Input: File extension as a string and EncodeInfo structure
*Output: Encodes the file extension into the stego image
*Description: Validates the file extension and encodes it into the 
BMP image using the LSB method.
*/
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
	//Check if the file extension is NULL
	if(file_extn == NULL)
	{
		fprintf(stderr, "ERROR: File extension is NULL\n");
		return e_failure;
	}	

	//Encode the file extension data into the image
	if(encode_data_to_image(file_extn, strlen(file_extn), encInfo) != e_success)
	{
		fprintf(stderr, "ERROR: Failed to encode secret file extension\n");
		return e_failure;
	}
	return e_success;
}

/*
Encode size to LSB
* Input: Size of unsigned integer and image buffer
*Output: e_success after encoding successfully
*Description: Modifies the LSB of each byte in the image buffer
to hide the size. 
*/
Status encode_size_to_LSB(unsigned int size, char *image_buffer)
{
	for(int i = 0; i < 32; i++)
	{	

	//'image_buffer[i] & 0xFE' - clears the LSB (set to zero)
	//'(size >> (31 - i)) & 1)' - extracts the i-th bit from size to be stored
	// '|' (bitwise OR) sets the LSB to new value
		image_buffer[i] = (image_buffer[i] & 0xFE | ((size >> (31 - i)) & 1));
	}

	return e_success;
}

/*
Encode secret file size
* Input: Size of the secret file and EncodeInfo Structure
*Output: Encodes the secret file size into the stego image
*Description: Reads the 32 bytes from the source image(beautiful.bmp) and encodes 
the size of the secret file into the LSBs of these bytes.
*/
Status encode_secret_file_size(long int file_size, EncodeInfo *encInfo)
{
	
	char str[32];

	//Read 32 bytes from source image to encode file size
	fread(str, 1, 32, encInfo -> fptr_src_image);

	//Call function to encode the size into the string using LSB method
	encode_size_to_LSB(file_size, str);

	//Write the encoded file size back to the stego image
	fwrite(str, 1, 32, encInfo -> fptr_stego_image);

	return e_success;
}

/*
Encode secret file data
* Input: EncodeInfo structure
*Output: Encodes the secret file data into the stego image
*Description: Reads the entire secret file and encodes its content
into the stego image using LSB method.
*/
Status encode_secret_file_data(EncodeInfo *encInfo)
{
	//Move to the start of the secret file
	fseek(encInfo -> fptr_secret, 0, SEEK_SET); 

	//String to hold secret file data, size is determined by the secret file size
	char str[encInfo -> size_secret_file]; 

	//Read the entire secret fule data into the string
	fread(str, 1, encInfo -> size_secret_file, encInfo -> fptr_secret); 

	//Call function to encode the secret file data into the image
	encode_data_to_image(str, strlen(str), encInfo);

	return e_success; 
}

/*
Copy remaining image data from source to stego image
* Input: EncodeInfo structure
*Output: Success status
*Description: Copy the remaining image data from beautiful.bmp
to stego image.
*/
Status copy_remaining_img_data(EncodeInfo *encInfo)
{
	char ch;

	//Read and copy the rest of the image data byte by byte
	while(fread(&ch, 1, 1, encInfo -> fptr_src_image) > 0)
	{
		fwrite(&ch, 1, 1, encInfo -> fptr_stego_image);
	}

        
	return e_success;
}


/*Perform the encoding
* Input: EncodeInfo structure
*Output: Executes a series of encoding operations
*Description: Opens the necessary files, checks capacity, 
copies the BMP header, encodes the magic string, secret file size,
extension, and data and finally copies the remaining image data.

*/

Status do_encoding(EncodeInfo *encInfo)
{
	//Calling functions for encoding
	if(open_files(encInfo) == e_success)
	{
		printf("Successfully opened all the files\n");
		if(check_capacity(encInfo) == e_success)
		{
			printf("Check capacity is successful\n");
			if(copy_bmp_header(encInfo ->fptr_src_image, encInfo -> fptr_stego_image) == e_success)
			{
				printf("Copy bmp header is successful\n");
				if(encode_magic_string(MAGIC_STRING, encInfo) == e_success)
				{
					printf("Magic string encoded successfully\n");
					strcpy(encInfo -> extn_secret_file, strstr(encInfo -> secret_fname, "."));
					if(encode_secret_file_extn_size(strlen(encInfo -> extn_secret_file), encInfo) == e_success)
					{
						printf("Encoding secret file extension size is successful\n");
						if(encode_secret_file_extn(encInfo -> extn_secret_file, encInfo) == e_success)
						{
							printf("Encoded secret file extension successfully\n");
							if(encode_secret_file_size(encInfo -> size_secret_file, encInfo) == e_success)
							{
								printf("Encoded secret file size successfully\n");
								if(encode_secret_file_data(encInfo) == e_success)
								{
									printf("Encoded secret file data successfully\n");
									if(copy_remaining_img_data(encInfo) == e_success)
									{
										printf("Copied remaining bytes successfully\n");	
									}
									else
									{
										printf("Failed to copy remaining bytes\n");
										return e_failure;
									}
								}
								else
								{
									printf("ERROR : Failed to encode secret file data\n");
								}
							}
							else
							{
								printf("ERROR : Failed to encode secret file size\n");
								return e_failure;
							}
						}
						else
						{
							printf("ERROR : Failed to encode secret file extension\n");
							return e_failure;
						}
					}
					else
					{
						printf("ERROR : Failed to encode secret file extension size\n");
						return e_failure;
					}
				}
				else
				{
					printf("ERROR : Magic string was not encoded\n");
					return e_failure;
				}
			}
			else 
			{
				printf("ERROR : Copy bmp header is a failure\n");
				return e_failure;	
			}
					
		}
		else
		{
			printf("ERROR : Check capacity is not successful\n");
			return e_failure;
		}
	
	}
	else
	{
		printf("ERROR : Failed to open the required files\n");
		return e_failure;
	}

return e_success;
}




























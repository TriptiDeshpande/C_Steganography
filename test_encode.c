/*

Name : Tripti Deshpande
Student id - 24001_003
Date : 23/09/2024

Project title: Steganography Using LSB Method

Description -
This project implements steganography using the Least Significant Bit (LSB) method to embed secret messages within BMP image files. 
The LSB method involves modifying the least significant bits of pixel values in an image to hide data, making the changes imperceptible to the human eye.

Input -
- Carrier Image: A BMP image file used to hide the secret message.
- Secret Message: Text or binary data to be hidden within the image.

Output -
- Stego Image: A BMP image file with the secret message embedded within it.
- Decoded Message: The extracted secret message from the stego image.

Methodology -
1. Encoding Process:
   - Image Selection: Choose a BMP image as the carrier.
   - Message Conversion: Convert the secret message into a binary format.
   - Embedding: Embed each bit of the secret message into the LSB of the image's pixel values.
   - File Extension and Size Encoding: Encode the file extension and size of the secret message within the image.
   - Save Stego Image: Save the modified image with the embedded message.

2. Decoding Process:
   - Extracting Data: Read the LSBs of the image pixels to retrieve the embedded binary data.
   - Message Reconstruction: Reassemble the binary data into the original secret message, including the file extension and size.
   - Save Decoded Message: Save the extracted message to a file.

*/




#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "decode.h"


int main(int argc, char *argv[])
{
	//Number of input arguments validation
	if(argc > 1 && argc < 8)
	{	
		//Encoding
		if(check_operation_type(argv) == e_encode)
		{
			printf("Selected Encoding\n");
			EncodeInfo encInfo;
			
			//Read the file name and validate
			if(read_and_validate_encode_args(argv, &encInfo) == e_success)
			{
				printf("Read and validate encode arguments is a success\n");
				
				if(do_encoding(&encInfo) == e_success)
					printf("Encoding completed successfully\n");
				else
					printf("ERROR : Encoding was not successful\n");
			}			
			else
				printf("ERROR : Read and validate encode arguments is a failure\n");
		}
		//Decoding
		else if(check_operation_type(argv) == e_decode)
		{
            		printf("Selected Decoding\n");
            		DecodeInfo decInfo;

            		// Read the file name and validate
            		if (read_and_validate_decode_args(argv, &decInfo) == d_success)
            		{
            			printf("Read and validated decode arguments successfully\n");

				// Do decoding
                		if (do_decoding(&decInfo) == d_success)
                		{
                    			printf("Decoding completed successfully\n");
                		}
                		else
                		{
                   			printf("ERROR: Decoding was not successful\n");
                		}
            		}
           		 else
            		{
               			 printf("ERROR: Read and validate decode arguments is a failure\n");
            		}
        	}

		else
			printf("ERROR : Invalid argument\nFor encoding : ./a.out -e beautiful.bmp secret.txt [stego.bmp]\nFor decoding : ./a.out -d stego.bmp [decode.txt]\n");
	
	}
	else 
	printf("ERROR : Invalid argument\nFor encoding : ./a.out -e beautiful.bmp secret.txt [stego.bmp]\nFor decoding : ./a.out -d stego.bmp [decode.txt]\n");
}

OperationType check_operation_type(char *argv[]){
	if(strcmp(argv[1], "-e") == 0)
		return e_encode;
	else if(strcmp(argv[1], "-d") == 0)
		return e_decode;
	else
		return e_unsupported;
}















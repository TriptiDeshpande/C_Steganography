
# Steganography Using Least Significant Bit (LSB) Technique

## Overview
This project implements steganography using the Least Significant Bit (LSB) method to embed secret messages within BMP image files. 
The LSB method involves modifying the least significant bits of pixel values in an image to hide data, making the changes imperceptible to the human eye.

---

## Objectives
- Conceal text or binary data within standard image formats (e.g., BMP, PNG).
- Ensure that stego images remain visually indistinguishable from the original cover images.
- Provide reliable encoding and decoding functionality with basic error handling.

---

## Features

- **LSB Encoding & Decoding**: Bit-level embedding of secret data within image pixels.
- **Support for Text & Binary Files**: Handles both plain text and file-based data.
- **Preserves Image Quality**: Minimal to no perceptible change in output image.
- **Command-line Interface (CLI)**: Lightweight tool for integration into pipelines or manual use.

---

## Methodology

### Encoding Process
1. **Pre-processing**:
   - Read and validate input files (cover image and secret message).
   - Check capacity to ensure the image can accommodate the message.
2. **Embedding**:
   - Embed a predefined magic string to identify the start of hidden content.
   - Embed file extension, size, and message content using LSB manipulation of pixel bytes.
3. **Post-processing**:
   - Save the modified pixel data to generate the stego image.

### Decoding Process
1. **Validation**:
   - Read and validate the stego image.
2. **Extraction**:
   - Locate the magic string to identify the embedded data start point.
   - Extract file extension, size, and reconstruct the original message from LSBs.
3. **Output**:
   - Save the extracted content to a file.

---

## Input

- **Cover Image**: Input image used to conceal the message (`.bmp`, `.png`, etc.)
- **Secret Message**: File containing the message to be embedded (e.g., `secret.txt`)

## Output

- **Stego Image**: Image with embedded data (e.g., `stego.bmp`)
- **Decoded Message**: Extracted message or file retrieved during decoding (e.g., `decoded_secret.txt`)

---

## How to Use
Encoding
```bash
./a.out -e beautiful.bmp secret.txt [stego.bmp]
```
## Decoding
```bash
./a.out -d stego.bmp [decode_secret.txt]
```
## File Descriptions
```
├── a.out                 # Compiled executable for encoding and decoding
├── beautiful.bmp         # Original cover image used to embed the secret message
├── stego.bmp             # Output image containing the hidden message
├── secret.txt            # Text file containing the secret message to be embedded
├── decoded_secret.txt    # Output file where the extracted message is saved
├── Terminal_Output.png   # Screenshot of terminal showing program output

├── common.h              # Header file with common macros and constants
├── types.h               # Header file defining enums and custom data types
├── encode.c              # Source file with functions to embed data into image
├── encode.h              # Header file for encode-related function declarations
├── decode.c              # Source file with functions to extract data from image
├── decode.h              # Header file for decode-related function declarations
├── test_encode.c         # Test program to validate and debug encoding functionality
```

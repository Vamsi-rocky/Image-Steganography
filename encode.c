#include <stdio.h>
#include<string.h>
#include "encode.h"
#include "types.h"
#include "common.h"

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

uint get_file_size(FILE *fptr)
{
    // Find the size of secret file data
    fseek(fptr, 0, SEEK_END);
    return ftell(fptr);
}

/*

 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    if(argv[2][0] != '.')
    {
        if(strstr(argv[2],".bmp") != NULL)
        {
            encInfo->src_image_fname = argv[2];
        }
        else
        {
            printf("ERROR : source image should be .bmp file\n");
            return e_failure;
        }
    }

    //check whether the file name is there or not(before .)
    //check the src file is having (.bmp) or not.
    //encinfo ->src_image_fname = argv[2];

    if(argv[3][0] != '.')
    {
        if(strstr(argv[3],".txt") != NULL||strstr(argv[3],".c") != NULL||strstr(argv[3],".h") != NULL||strstr(argv[3],".sh") != NULL)
        {
            encInfo ->secret_fname = argv[3];
            strcpy(encInfo->extn_secret_file, "txt");
        }
        else
        {
            printf("ERROR :secret file should be .txt/.c/.h/.sh file\n");
            return e_failure;
        }
    }
    if(argv[4]==NULL)
    {
        encInfo->stego_image_fname="default.bmp";
    }
    else
    {
        encInfo->stego_image_fname=argv[4];
    }
    return e_success;


    //check the whether the file name is there or not (before.)
    //check the src file is having(.txt ot .t ot .h or .sh) or not
    //encInfo ->src_image_frame = argv[3];

    //check whether the argv[4] is having NULL or not
    //encInfo -> stego_image_frame = "default . bmp";
    //check the whether the file name is there or not (before.)
    //check the src file is having(.bmp) or not
    //encInfo ->stego_image_frame = argv[4];

    //return failure
}


Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

        return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

        return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
        // print error message and return e_failure
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

        return e_failure;
    }

    // No failure return e_success
    return e_success;
}

// Check image capacity
Status check_capacity(EncodeInfo *encInfo)
{
    // Get the image size
    encInfo ->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
    // Get the secret file size
    encInfo ->size_secret_file = get_file_size(encInfo->fptr_secret);
    // Calculate the required size
    char *extn = strstr(encInfo->secret_fname, ".txt");
    // Calculate total bytes to be encoded
    int extn_size = strlen(extn); 
    // total bytes to be encoded
    int total_bytes = 54 + (strlen(MAGIC_STRING) + 8) + 32 + (extn_size *8) + 32 + (encInfo->size_secret_file * 8);
    
    if (encInfo->image_capacity > total_bytes)
    {
        // print sufficient capacity message and return e_success
        printf("Sufficient Capacity\n");
        return e_success;
    }
    else
    {
        return e_failure;
    }

}
// Copy BMP Image Header
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    char buffer[54];  // BMP header is 54 bytes
    rewind(fptr_src_image); // Move file pointer to the beginning
    fread(buffer,54,1,fptr_src_image); // Read BMP header from source image
    fwrite(buffer,54,1,fptr_dest_image); // Write BMP header to destination image
    if(ftell(fptr_src_image) == ftell(fptr_dest_image)) // Verify if header copied successfully
    {
        // print success message and return e_success
        printf("BMP Header Copied\n");
        return e_success;
    }
    else
      return e_failure;
}
// Encode Magic String
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    char image_buffer[8]; // Buffer to hold image bytes
    for(size_t i=0; i<strlen(magic_string); i++) // Loop through each character of magic string
    {
        fread(image_buffer,8,1,encInfo->fptr_src_image); // Read 8 bytes from source image
        encode_byte_to_lsb(magic_string[i],image_buffer); // Encode character into LSBs of image bytes
        fwrite(image_buffer,8,1,encInfo->fptr_stego_image); // Write modified bytes to stego image
    }
    return e_success;

}
// Encode Secret File Extension Size
Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo)
{
    char image_buffer[32]; // Buffer to hold image bytes
    fread(image_buffer,32,1,encInfo->fptr_src_image); // Read 32 bytes from source image
    encode_size_to_lsb(size,image_buffer); // Encode size into LSBs of image bytes
    fwrite(image_buffer,32,1,encInfo->fptr_stego_image); // Write modified bytes to stego image
    return e_success;
}

// Encode Secret File Extension
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    char image_buffer[8]; // Buffer to hold image bytes
    for(size_t i=0; i<strlen(file_extn); i++) // Loop through each character of file extension
    {
        fread(image_buffer,8,1,encInfo->fptr_src_image); // Read 8 bytes from source image
        encode_byte_to_lsb(file_extn[i],image_buffer); // Encode character into LSBs of image bytes
        fwrite(image_buffer,8,1,encInfo->fptr_stego_image); // Write modified bytes to stego image
    }
    return e_success;

}

// Encode Secret File Size  
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    char image_buffer[32]; // Buffer to hold image bytes
    fread(image_buffer,32,1,encInfo->fptr_src_image); // Read 32 bytes from source image
    encode_size_to_lsb((int) file_size,image_buffer); // Encode size into LSBs of image bytes
    fwrite(image_buffer,32,1,encInfo->fptr_stego_image); // Write modified bytes to stego image
    return e_success; // Return success

}

// Encode Secret File Data
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    rewind(encInfo->fptr_secret); // Move file pointer to the beginning of secret file
    fread(encInfo->secret_data,encInfo->size_secret_file,1,encInfo->fptr_secret); // Read secret file data into buffer
    char image_buffer[8]; // Buffer to hold image bytes
    for(size_t i=0; i<encInfo->size_secret_file; i++) // Loop through each byte of secret data
    {
        fread(image_buffer,8,1,encInfo->fptr_src_image); // Read 8 bytes from source image
        encode_byte_to_lsb(encInfo->secret_data[i],image_buffer); // Encode byte into LSBs of image bytes
        fwrite(image_buffer,8,1,encInfo->fptr_stego_image); // Write modified bytes to stego image
    }
    return e_success; // Return success
}

// Copy Remaining Image Data
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char ch; 
    while(fread(&ch,1,1,fptr_src)) // Read each byte from source image until EOF
    {
        fwrite(&ch,1,1,fptr_dest); // Write byte to destination image
    }
    return e_success;
}
 
// Encode a byte into LSBs of 8 bytes of image data
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for(size_t i=0;i<8;i++) // Loop through each bit of the byte
    {
        image_buffer[i] = image_buffer[i] & (~1) | ((data >> i) & 1); // Modify LSB of image byte to match bit of data
    }
}

// Encode a size (int) into LSBs of 32 bytes of image data
Status encode_size_to_lsb(int size, char *imageBuffer)
{
    for(size_t i=0;i<32;i++) // Loop through each bit of the size
    {
        imageBuffer[i] = imageBuffer[i] & (~1) | ((size >> i) & 1); // Modify LSB of image byte to match bit of size
    }
}

// Perform the encoding process
Status do_encoding(EncodeInfo *encInfo)
{
 if(open_files(encInfo) != e_success) return e_failure; // Open necessary files
    if(check_capacity(encInfo) != e_success) return e_failure; // Check if image has sufficient capacity
    if(copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image)!=e_success) return e_failure; // Copy BMP header
    if(encode_magic_string(MAGIC_STRING, encInfo)!=e_success) return e_failure; // Encode magic string
    if(encode_secret_file_extn_size(strlen(encInfo->extn_secret_file),encInfo)!=e_success) return e_failure; // Encode secret file extension size
    if (encode_secret_file_extn(encInfo->extn_secret_file, encInfo) != e_success) return e_failure; // Encode secret file extension
    if(encode_secret_file_size(encInfo->size_secret_file,encInfo)!=e_success) return e_failure; // Encode secret file size
    if (encode_secret_file_data(encInfo) != e_success) return e_failure; // Encode secret file data
    if (copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) != e_success) return e_failure; // Copy remaining image data
    printf("✅ Encoding completed successfully...\n"); // Print success message
    return e_success;
}

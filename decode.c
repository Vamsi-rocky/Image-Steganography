#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "decode.h"
#include "types.h"
#include "common.h"

// Function Definitions
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo) 
{
    //default.bmp(source file)
    if(argv[2][0] != '.')//check whether the file name is there or not before .
    {
        if(strstr(argv[2],".bmp") != NULL)//check the source file extension is matched or not(.bmp)
        {
            decInfo->output_image_fname = argv[2]; //store the source image name
        }
        else
        {
            printf("Error : source file should be .bmp file\n"); //print error message
            return e_failure;
        }
    }
    else
    {
        printf("Error : source file name should not start with '.' \n"); //print error message
        return e_failure;
    }

    //secret file
    if(argv[3] != NULL)
    {
        int len = strlen(argv[3]); //check whether the file name is there or not(before .)
        if(argv[3][len-1] != '.') //check the secret file is having (.) at last or not
        {
            char *ptr = strstr(argv[3],"."); //get the pointer to (.)
            *(ptr+1) = '\0';
        }
        decInfo->secret_fname = argv[3]; //store the secret file name
    }
    else
    {
        decInfo->secret_fname = malloc(20); //allocate memory to secret file name
        strcpy(decInfo->secret_fname,"output."); //default secret file name
    }
    return e_success;          
}

// Open files required for decoding
Status open_files_decode(DecodeInfo *decInfo)
{
    //Src Image file to decode
    decInfo->fptr_output_image = fopen(decInfo->output_image_fname,"r");
    //Do error handling
    if (decInfo->fptr_output_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->output_image_fname); //print error message

        return e_failure;
    }

    //Move 54 bytes to decode original data
    fseek(decInfo->fptr_output_image,54,SEEK_SET);

    //No failure return e_success
    return e_success;
}

// Decode Magic String
Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo)
{
    char buffer[8]; // Buffer to hold image bytes
    char ch ;
    char str[5];
    size_t i;
    for(i = 0;i<strlen(magic_string);i++) // Loop through each character of magic string
    {
        ch = 0;
        fread(buffer,8,1,decInfo->fptr_output_image); // Read 8 bytes from stego image
        decode_byte_to_lsb(buffer,&ch); // Decode byte from LSBs of image bytes
        str[i] = ch;
    }
    str[i] = '\0';
    printf("Decoded magic string is %s\n",str); // Print decoded magic string
    if(strcmp(magic_string,str) == 0)
    {
        printf("Magic string is decoded succesfully\n"); // Print success message
    }
    else
    {
        printf("Magic string is not decoded successfully\n"); // Print failure message
        return e_failure;
    }
    return e_success;

}

// Decode Secret File Extension Size
Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
    char buffer[32];
    int size = 0;
    fread(buffer,32,1,decInfo->fptr_output_image);
    decode_size_to_lsb(buffer,&size);
    decInfo->size_secret_file_extn = size;
    //printf("In struct %ld\n",decInfo->size_secret_file_extn);
   // printf("Length of secret file extension is : %d\n",size);
    return e_success;
}

// Decode Secret File Extension
Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    char buffer[8];
    char ch = 0;
    char str[5];
    size_t i;
    for(i=0;i<decInfo->size_secret_file_extn;i++) // Loop through each character of file extension
    {
        ch = 0;
        fread(buffer,8,1,decInfo->fptr_output_image); // Read 8 bytes from stego image
        decode_byte_to_lsb(buffer,&ch); // Decode byte from LSBs of image bytes
        str[i] = ch;
    }
    str[i] = '\0';
   // printf("Extension of secret file is %s\n",str);
    strcat(decInfo->secret_fname,str);
    //printf("Secret file name is %s\n",decInfo->secret_fname);
    return e_success;

}

Status decode_secret_file_size(DecodeInfo *decInfo)
{
    char buffer[32];
    int secret_file_size = 0;
    fread(buffer,32,1,decInfo->fptr_output_image);
    decode_size_to_lsb(buffer,&secret_file_size);
    decInfo-> size_secret_data = secret_file_size;
    //printf("secret file size is %ld\n",decInfo-> size_secret_data);
    return e_success;
}

// Decode Secret File Data
Status decode_secret_file_data(DecodeInfo *decInfo)
{
    decInfo->fptr_secret = fopen(decInfo->secret_fname,"w"); //Open secret file to write decoded data
    //Do error handling
    if (decInfo->fptr_secret == NULL) //If file pointer is NULL
    {
        printf("Error in opening the file"); //Print error message

        return e_failure;
    }
    char buffer[8];
    char ch;
    size_t i;
    for(i=0;i<decInfo->size_secret_data;i++) // Loop through each byte of secret data
    {
        ch = 0;
        fread(buffer,8,1,decInfo->fptr_output_image); // Read 8 bytes from stego image
        decode_byte_to_lsb(buffer,&ch); // Decode byte from LSBs of image bytes
        fwrite(&ch,1,1,decInfo->fptr_secret); // Write decoded byte to secret file
    }
    printf("✅Secret message is decoded successfully\n");
    return e_success;
}

//  Decode a byte from LSBs of 8 bytes of image data
Status decode_byte_to_lsb(char *image_buffer,char *data)
{
    for(size_t i = 0; i < 8; i++)
    {
        *data = (image_buffer[i] & 1) << i | *data;  // Extract LSB and construct byte
    }
    return e_success;

}
// Decode a size (int) from LSBs of 32 bytes of image data
Status decode_size_to_lsb(char *imageBuffer,int *data)
{
    for(size_t i = 0; i < 32; i++)
    {
        *data = (imageBuffer[i] & 1) << i | *data;  // Extract LSB and construct size
    }
    return e_success;
}

// Perform the decoding process
Status do_decoding(DecodeInfo *decInfo)
{
    if(open_files_decode(decInfo) != e_success) return e_failure; // Open necessary files
    if(decode_magic_string(MAGIC_STRING,decInfo) != e_success) return e_failure; // Decode magic string
    if(decode_secret_file_extn_size(decInfo) != e_success) return e_failure; // Decode secret file extension size
    if(decode_secret_file_extn(decInfo) != e_success) return e_failure; // Decode secret file extension
    if(decode_secret_file_size(decInfo) != e_success) return e_failure; // Decode secret file size
    if(decode_secret_file_data(decInfo) != e_success) return e_failure; // Decode secret file data
    return e_success;
    
}

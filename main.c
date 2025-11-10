#include <stdio.h> 
#include <string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"
// Function prototype
OperationType check_operation_type(char *);
// Main function
int main(int argc, char *argv[])
{
    // Step 1: Check if argc >= 4
    if (argc >= 3)
    {
        // Step 2: Check operation type
        if (check_operation_type(argv[1]) == e_encode)
        {
            // Encoding operation
            EncodeInfo enc_info;
            // Step 3: Read and validate encoding arguments
            if (read_and_validate_encode_args(argv, &enc_info) == e_success)
            {
                // Step 4: Print success message
                printf("Read and Validate Success\n");
            }
            else
            {
                // Step 5: Print failure message and return
                printf("Read and Validate Failure\n");
                return 0;
            }
            // Step 6: Perform encoding
            if (do_encoding(&enc_info) != e_success)
            {
                // Step 7: Print encoding failure message and return
                printf("Encoding Failed\n");
                return 0;
            }
            else
            {
                // Step 8: Print encoding success message
                printf("Encoding Success\n");
            }
        }
        // Decoding operation
        else if (check_operation_type(argv[1]) == e_decode)
        {
            DecodeInfo dec_info;
            // Read and validate decoding arguments
            if (read_and_validate_decode_args(argv, &dec_info) == e_success)
            {
                // Print success message
                printf("Read and Validate Success\n");
            }
            else
            {
                // Print failure message and return
                printf("Read and Validate Failure\n");
                return 0;
            }
            // Perform decoding
            if (do_decoding(&dec_info) != e_success)
            {
                // Print decoding failure message and return
                printf("Decoding Failed\n");
                return 0;
            }
            else
            {
                // Print decoding success message
                printf("Decoding Success\n");
            }
        }
        else
        {
            // Unsupported operation type
            printf("Unsupported operation type.\n");
        }
    }
    else
    {
        // Insufficient arguments provided
        printf("Insufficient arguments provided.\n");       
    }   
    return 0;
}

// Function to check operation type
OperationType check_operation_type(char *symbol)
{
    // Check for encoding mode
    if (strcmp(symbol, "-e") == 0)
    {
        // Print encoding mode message and return encoding type
        printf("Encoding Mode\n");
        return e_encode;
    }
    // Check for decoding mode
    else if (strcmp(symbol, "-d") == 0)
    {
        // Print decoding mode message and return decoding type
        printf("Decoding Mode\n");
        return e_decode;
    }
    else
    {
        // Print unsupported mode message and return unsupported type
        printf("Unsupported Mode\n");
        return e_unsupported;
    }
}

/******************************************************************************
*  @file  encode_decode.hpp
*  @brief functions for encodeing and decoding messages for sending over socket
*
*  @brief first 100 byte are destination names part
*  @brief from 100 to 128 byte are message info part
*  @brief from 128 byte are message
*  @brief message structure (dots are NULL or 0x00)
*
*  @brief Tom,Sophia...........................................................
*         ...............................Private.....................Hello!!!..
*         .....................................................................
*         .....................................................................
*         .....................................................................
*         .....................................................................
*         .....................................................................
*         .............................
*
*  @brief encode_message() encodes input from terminal to mesaage structure
*  @brief decode_message() decodes from message structure and sets variables
*  @brief make_encoded_message() makes message structure from variables
*
*  @version 2.0.0
*  @author Ara Khachatryan
******************************************************************************/


void encode_message( const char * input, char * output, size_t io_buffer_length );

void decode_message( const char * encoded_message, size_t io_buffer_length,
        std::vector<std::string>& dest_names, std::string& message_info, std::string&  message );

void make_encoded_message( const char * dest_name, const char * message_info,
        const std::string& message, size_t io_buffer_length, char * output );

void print_encoded_message( const char * encoded_message, size_t io_buffer_length );


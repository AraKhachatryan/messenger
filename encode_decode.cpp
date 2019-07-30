/******************************************************************************
*  @file  encode_decode.cpp
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

#include <vector>
#include <string>
#include <sstream>
#include <iostream>


void encode_message( const char * input, char * output, size_t io_buffer_length )
{
    // check for private mode pattern
    bool is_private = 0;
    for( size_t i = 0; i < 100; ++i )
    {
        if ( input[0] == '@' && input[i] == ':'  )
        {
            is_private = 1;
            break;
        }
    }


    if( is_private )
    {
        size_t i = 0;
        size_t j = 0;

        // Destination names part    //////////////////////////////////////////
        bool name_end_flag = 0;
        bool name_start_flag = 0;
        bool at_last_one_name_exist = 0;

        while( input[i] != ':' )
        {
            // destination name validation
            if ( ((input[i] >= '0' && input[i] <= '9') ||
                 (input[i] >= 'A' && input[i] <= 'Z') ||
                 (input[i] >= 'a' && input[i] <= 'z')) && !name_end_flag )
            {
                if( name_start_flag && at_last_one_name_exist )
                {
                    output[j] = ',';
                    ++j;
                }
                output[j] = input[i];
                ++j;

                name_start_flag = 0;
                at_last_one_name_exist = 1;
            // for more destination names
            } else if ( input[i] == '@' ) {
                name_end_flag = 0;
                name_start_flag = 1;

            // destination name ended
            } else {
                name_end_flag = 1;
            }
            ++i;
        }

        // fill output[] after destination names with 0x00
        // destination names part can contain up to 100 simbols
        for( ; j < 100; )
        {
            output[j] = 0x00;
            ++j;
        }

        ++i; // for going over ':'

        // for going over spaces after ':'
        while( input[i] == ' ' )
        {
            ++i;
        }
        // END destination names part /////////////////////////////////////////


        // Message info part    ///////////////////////////////////////////////
        if( at_last_one_name_exist )
        {
            for( auto c: "Private" ){
                output[j] = c;
                ++j;
            }
            // fill output[] after Message info part with 0x00
            // Message info part can contain up to 28 simbols
            for( ; j < 128; )
            {
                output[j] = 0x00;
                ++j;
            }
        } else {
            for( auto c: "Public" ){
                output[j] = c;
                ++j;
            }
            // fill output[] after Message info part with 0x00
            // Message info part can contain up to 28 simbols
            for( ; j < 128; )
            {
                output[j] = 0x00;
                ++j;
            }
        }
        // END message info part //////////////////////////////////////////////


        // Message part    ////////////////////////////////////////////////////
        // copy input message to output from position 128 until reaching 0x00
        for( j = 128; j < io_buffer_length; ++i )
        {
            if( input[i] == 0x00 )
            {
                break;
            }
            output[j] = input[i];
            ++j;
        }

        // fill output[] after destination message
        for( ; j < io_buffer_length; )
        {
            output[j] = 0x00;
             ++j;
        }
        // END message part ///////////////////////////////////////////////////

    }



    if ( !is_private )
    {
        // Message info part    ///////////////////////////////////////////////
        size_t j = 100;
        for( auto c: "Public" ){
            output[j] = c;
            ++j;
        }
        // fill output[] after Message info part with 0x00
        // Message info part can contain up to 28 simbols
        for( ; j < 128; )
        {
            output[j] = 0x00;
            ++j;
        }
        // END message info part //////////////////////////////////////////////


        // Message part    ////////////////////////////////////////////////////
        // if no destination names, copy whole input to output from position 128
        j = 128;
        for( size_t i = 0; i < io_buffer_length; ++i )
        {
            output[j] = input[i];
            ++j;
        }
        // END message part ///////////////////////////////////////////////////
    }


}


void make_encoded_message( const char * dest_name, const char * message_info,
                          const std::string& message, size_t io_buffer_length, char * output )
{
    size_t j = 0;

    for( size_t i = 0; dest_name[i] != 0; ++i )
    {
        output[j] = dest_name[i];
        ++j;
    }

    for( ; j < 100; )
    {
        output[j] = 0x00;
        ++j;
    }

    for(  size_t i = 0; message_info[i] != 0; ++i  )
    {
        output[j] = message_info[i];
        ++j;
    }

    for( ; j < 128; )
    {
        output[j] = 0x00;
        ++j;
    }

    for(auto& c: message)
    {
        output[j] = c;
        ++j;
    }

        for( ; j < io_buffer_length; )
    {
        output[j] = 0x00;
        ++j;
    }

}


void decode_message( const char * encoded_message, size_t io_buffer_length,
                    std::vector<std::string>& dest_names, std::string& message_info, std::string&  message )
{
    size_t name_char_count = 0;
    size_t name_char_firs_pos = 0;
    bool new_name_flag = 0;


    for( size_t i = 0; i < 100; ++i )
    {
        if ( encoded_message[0] == 0x00 )
        {
            break;
        }

        if ( (encoded_message[i] == ',') || (encoded_message[i] == 0x00) )
        {
            if ( new_name_flag )
            {
                name_char_firs_pos += name_char_count + new_name_flag;
            }
            name_char_count = i - name_char_firs_pos;

            dest_names.push_back(std::string(encoded_message + name_char_firs_pos, name_char_count));

            if ( encoded_message[i] == 0x00 )
            {
                break;
            }

            if ( encoded_message[i] == ',' )
            {
                new_name_flag = 1;
                continue;
            }
        }
    }

    // copy message info part to message_info until reaching 0x00
    name_char_count = 0;
    for( size_t i = 100; i < 128; ++i )
    {
        if ( encoded_message[100] == 0x00 )
        {
            break;
        }

        if ( encoded_message[i] == 0x00 )
        {
            name_char_count = i - 100;

            message_info = std::string(encoded_message + 100, name_char_count);

            if ( encoded_message[i] == 0x00 )
            {
                break;
            }
        }

    }

    // copy message part to message until reaching 0x00
    name_char_count = 0;
    for( size_t i = 128; i < io_buffer_length; ++i )
    {
        if ( encoded_message[128] == 0x00 )
        {
            break;
        }

        if ( encoded_message[i] == 0x00 )
        {
            name_char_count = i - 128;

            message = std::string(encoded_message + 128, name_char_count);

            if ( encoded_message[i] == 0x00 )
            {
                break;
            }
        }

    }

}


// Helper function
void print_encoded_message( const char * encoded_message, size_t io_buffer_length )
{
    std::cout << std::endl;
    for( size_t i = 0; i < io_buffer_length; ++i )
    {
        if(encoded_message[i] == 0x00){
            std::cout << ".";
        }else{
            std::cout << encoded_message[i];
        }
    }
    std::cout << std::endl;
}

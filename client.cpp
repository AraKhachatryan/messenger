#include <sys/socket.h> // Core BSD socket functions and data structures
#include <arpa/inet.h>  // for manipulating IP addresses, for inet_addr()
#include <netdb.h>      // for gethostname()
#include <unistd.h>     // for close()
#include <strings.h>    // for bzero()
#include <sys/ioctl.h>  // for ioctl() - terminal sizes
#include <pthread.h>    // POSIX threcv_thread

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <cerrno>       // for errno
#include <cstring>      // for std::strerror()


#include "encode_decode.hpp"

#include "namespace_terminal.hpp"
bool terminal_color = true;

void * send_thread( void * arg );
void * recv_thread( void * arg );


int main (int argc, char* argv[])
{
    if(argc < 2)
    {
        std::cerr << "Syntax : ./client <server ip address>" << std::endl;
        return 0;
    }

    const char * ip_addres = argv[1];
    const int port_number = 2025;

    int socket_desc;
    struct sockaddr_in server_addr;


    // create socket
    socket_desc = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(socket_desc == -1)
    {
        std::cerr << "Cannot open socket" << std::endl;
        close(socket_desc);
        return 0;
    }


    bzero((char *) &server_addr, sizeof(server_addr));

    // Set remote server information
	server_addr.sin_addr.s_addr = inet_addr( ip_addres );
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons( port_number );


    if ( connect(socket_desc, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1 )
    {
        std::cerr << "Cannot connect!" << std::endl;
        return 0;
    }


    pthread_t send_thread_id;
    pthread_t recv_thread_id;

    pthread_create(&send_thread_id, NULL, send_thread, (void*)&socket_desc);
    pthread_detach(send_thread_id);

    pthread_create(&recv_thread_id, NULL, recv_thread, (void*)&socket_desc);
    pthread_join(recv_thread_id, NULL);

    pthread_exit(NULL);

    return 0;

}



void * send_thread( void * arg )
{
    int socket_desc = *(int*)arg;

    char client_name[50];
    std::vector<std::string> dest_names;
    std::string message_info;
    std::string message;

    char input[392];
    char input_encoded[512];

    bzero(client_name, sizeof(client_name));

    std::cout << "Write your name: " << terminal::TEXT_BOLD << terminal::TEXTCOLOR_CYAN;
    std::cin.getline(client_name, 50);
    std::cout << terminal::RESET_ALL;

	send(socket_desc, (char*)&client_name, sizeof(client_name), 0);

	std::cout << "Private message pattern: @<Destination name>: <message>" << std::endl;
	std::cout << "Public message typed as is: <message>" << std::endl;
	std::cout << "Write your first message !!!" << std::endl << std::endl;

    while(1)
    {
        // clear all data buffers
        bzero(input, sizeof(input));
        bzero(input_encoded, sizeof(input_encoded));
        dest_names.clear();
        message_info.clear();
        message.clear();

        std::cin.getline(input, sizeof(input));


        // Erase input from terminal   ////////////////////////////////////////
        int input_size = std::cin.gcount();
        struct winsize w;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        int erase_line_count = 0;
        if( (input_size % w.ws_col) != 0 )
        {
            erase_line_count = (int)(input_size / w.ws_col) + 1;
        } else {
            erase_line_count = (int)(input_size / w.ws_col);
        }
        while( erase_line_count-- != 0 )
        {
            std::cout << terminal::Cursor_Previous_Line() << terminal::Erase_Line();
        }
        ///////////////////////////////////////////////////////////////////////


        // If input is empty (Enter key is pressed) do nothing
        if ( input_size == 1 )
        {
            continue;
        }


        encode_message(input, input_encoded, sizeof(input_encoded));
        //print_encoded_message(input_encoded, sizeof(input_encoded));

        send(socket_desc,(char*)&input_encoded, sizeof(input_encoded), 0);

        // decoding for input formatting in terminal
        decode_message(input_encoded, sizeof(input_encoded), dest_names, message_info, message);


        // Formatting input (encoded -> decoded input message)   ////////////////////
        // Private message is send
        if ( !dest_names.empty() )
        {
            std::cout << terminal::TEXT_BOLD << terminal::TEXTCOLOR_BLUE;

            for( auto it = dest_names.begin(); it != dest_names.end(); ++it )
            {
                std::cout << *it;
                if ( *it != dest_names.back() )
                {
                    std::cout << ", ";
                }
            }

            std::cout << terminal::RESET_ALL << "<- " << terminal::TEXT_BOLD << terminal::TEXTCOLOR_YELLOW
                        << message << terminal::RESET_ALL << std::endl;

        // Public message is send (if no dest_names)
        } else {
            std::cout << terminal::TEXT_BOLD << terminal::TEXTCOLOR_YELLOW
                        << message << terminal::RESET_ALL << std::endl;

        }
        ///////////////////////////////////////////////////////////////////////



    } // End while()

    pthread_exit(NULL);
}


void * recv_thread( void * arg )
{
    char input[512];

	std::vector<std::string> name_from;
	std::string message_info;
	std::string message_from;

	int socket_desc = *(int*)arg;

	while(1)
    {
        name_from.clear();
        message_info.clear();
        message_from.clear();

        int rv = recv(socket_desc, input, sizeof(input), 0);
		if ( rv == -1 ){
            std::cerr << "Error reading from server: " << std::strerror(errno) << std::endl;
            break;
        } else if ( rv  == 0 ) {
            break;
        }

        decode_message(input, sizeof(input), name_from, message_info, message_from);

        // Private incoming messages
        if( !message_info.compare("Private") )
        {
            std::cout << terminal::TEXT_BOLD << terminal::TEXTCOLOR_RED << name_from.front()
                        << terminal::RESET_ALL <<  "-> " << terminal::TEXT_BOLD << terminal::TEXTCOLOR_GREEN
                        << message_from << terminal::RESET_ALL << std::endl;
        // Public incoming messages
        } else if ( !message_info.compare("Public") ) {
            std::cout << terminal::TEXT_BOLD << terminal::TEXTCOLOR_WHITE << name_from.front()
                        << terminal::RESET_ALL <<  "-> " << terminal::TEXT_BOLD << terminal::TEXTCOLOR_GREEN
                        << message_from << terminal::RESET_ALL << std::endl;
        // New active clients notifications
        } else if ( !message_info.compare("Server:online") ) {
            std::cout << terminal::TEXT_BOLD << terminal::TEXTCOLOR_CYAN;
            for( auto it = name_from.begin(); it != name_from.end(); ++it )
            {
                std::cout << *it;
                if( *it != name_from.back() )
                {
                    std:: cout << ", ";
                }
            }
            std::cout << terminal::RESET_ALL << terminal::TEXT_BOLD << terminal::TEXTCOLOR_WHITE
                        << message_from << terminal::RESET_ALL << std::endl;
        }


	} // End while()

	std::cout << "Server is down!!! Closing recv_thread and socket..." << std::endl;
	close(socket_desc);
	pthread_exit(NULL);
}


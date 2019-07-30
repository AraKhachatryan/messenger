#include <sys/socket.h> // Core BSD socket functions and data structures
#include <arpa/inet.h>  // for manipulating IP addresses, for inet_addr()
#include <unistd.h>     // for close()
#include <strings.h>    // for bzero()
#include <pthread.h>    // POSIX threads

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <cerrno>       // for errno
#include <cstring>      // for std::strerror()


#include "encode_decode.hpp"


std::map <std::string, int> list_clients;

void * client_thread( void * );


int main(int argc, char* argv[])
{
    const int port_number = 2025;
    const int max_clients = 10;

    int master_socket_fd;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr[max_clients];
    const int opt = 1;

    int client_socket_fd;
    pthread_t thread[max_clients];

    // create socket
    master_socket_fd = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
    if( master_socket_fd == -1)
    {
        close(master_socket_fd);
        std::cerr << "Cannot open socket" << std::endl;
        return 0;
    }

    // Forcefully attaching socket to the same port again
    if( setsockopt(master_socket_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, (const void *)&opt, (socklen_t)sizeof(opt)) == -1 )
    {
        std::cerr << "setsockopt" << std::endl;
        return 0;
    }

    bzero((void *)&server_addr, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port_number);

    // bind socket
    if( bind(master_socket_fd, (struct sockaddr *)&server_addr, (socklen_t)sizeof(server_addr)) == -1 )
    {
        std::cerr << "Cannot bind" << std::endl;
        return 0;
    }


    if ( listen(master_socket_fd, max_clients) == -1 )
    {
        std::cerr << "Cannot listen" << std::endl;
        return 0;
    }

    std::cout << "Listening socket_fd " << master_socket_fd << " on port " << port_number << "..." << std::endl;

    for ( int i = 0; i < max_clients; ++i )
    {
        socklen_t client_addr_len = sizeof(client_addr[i]);
        bzero((void *)&client_addr[i], sizeof(client_addr[i]));

        // Accept incoming connections
        client_socket_fd = accept(master_socket_fd, (struct sockaddr *)&client_addr[i], &client_addr_len);
        if ( client_socket_fd == -1 )
        {
            std::cerr << "Cannot accept connection" << std::endl;
        }
        else
        {
            std::cout << "Incoming connection from: " << inet_ntoa(client_addr[i].sin_addr) << " on socket_fd: " << client_socket_fd << std::endl;
        }

        pthread_create( &thread[i], NULL, client_thread, (void*)&client_socket_fd );
    }

    return 0;

}


void * client_thread ( void * client_socket_fd )
{
    char client_name[50];
    bzero(client_name, sizeof(client_name));

    char encoded_message[512];
    std::vector<std::string> dest_names;
    std::string message_info;
    std::string message;
    char output[512];


    int fd = *(int *)client_socket_fd;

    recv(fd, client_name, sizeof(client_name), 0);


    // Notify to all clients that this connection is online
    std::string active_clients;
    size_t clinets_count = 0;
    for( const auto& client: list_clients )
    {
        active_clients += client.first;
        ++clinets_count;
        if ( clinets_count < list_clients.size() ) {
            active_clients += ",";
        }

        make_encoded_message(client_name, "Server:online", std::string(" is online"), sizeof(output), output);
        //print_encoded_message(output, sizeof(output));

        send(client.second, output, sizeof(output), 0);
    }

    // Notify to this connection for available online clients
    if ( !list_clients.empty() )
    {
        std::string is_are;
        if( clinets_count > 1 )
        {
            is_are = std::string(" are online");
        } else {
            is_are = std::string(" is online");
        }
        make_encoded_message(active_clients.c_str(), "Server:online", is_are, sizeof(output), output);
        //print_encoded_message(output, sizeof(output));

        send(fd, output, sizeof(output), 0);
    }


    list_clients.insert(std::pair<std::string, int>(std::string(client_name), fd));


    std::cout << "Thread ID: " << pthread_self() << " socket_fd: " << fd << " Name: " << client_name << std::endl;

    while(1)
    {

        bzero(encoded_message, sizeof(encoded_message));
        dest_names.clear();
        message_info.clear();
        message.clear();
        bzero(output, sizeof(output));


        int rv = recv(fd, encoded_message, sizeof(encoded_message), 0);
        if ( rv  == -1 )
        {
            std::cerr << "Error reading from client: " << std::strerror(errno) << std::endl;
            break;
        } else if ( rv  == 0 ) {
            break;
        }

        //print_encoded_message(encoded_message, sizeof(encoded_message));

        decode_message(encoded_message, sizeof(encoded_message), dest_names, message_info, message);


        // Private messages
        if ( !dest_names.empty() )
        {
            // for each destination names in encoded_message do
            for( const auto& d_name: dest_names )
            {
                int dest_socket_desc;

                // find dest_socket_desc
                std::map <std::string, int>::iterator it = list_clients.find(d_name);
                if (it != list_clients.end()) {
                    dest_socket_desc = it->second;
                } else {
                    std::cout << "From \"" << client_name << "\": destination name \"" << d_name << "\" not found" << std::endl;
                    continue;
                }

                std::cout << client_name << " -> " << d_name << ": " << message << std::endl;

                make_encoded_message(client_name, "Private", message, sizeof(output), output);

                // and send message to destination
                send(dest_socket_desc, output, sizeof(output), 0);

            }
        // Public messages
        } else {
            for( const auto& client: list_clients )
            {
                // send message to all except sender
                if ( client.first.compare(client_name) == 0 )
                {
                    continue;
                }
                std::cout << client_name << " -> " << client.first << ": " << message << std::endl;

                make_encoded_message(client_name, "Public", message, sizeof(output), output);

                send(client.second, output, sizeof(output), 0);
            }
        }

    }

    // removing connection information from list_clients
    list_clients.erase(client_name);

    // Notify to all clients that this connection is leaving
    for( const auto& client: list_clients )
    {
        make_encoded_message(client_name, "Server:online", std::string(" left the chat"), sizeof(output), output);
        //print_encoded_message(output, sizeof(output));

        send(client.second, output, sizeof(output), 0);
    }

    std::cout << "Closing connection with socket_fd: " << fd << ".  Name: " << client_name
                << ".  Exiting the thread." << std::endl;
    close(fd);
    pthread_exit(NULL);

}


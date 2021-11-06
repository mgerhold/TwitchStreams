/* A wrapper around berkeley sockets for tcp and udp */

// License: MIT License, you can find the full license at the bottom of the file
// Author: Clemens aka Cunicus
// Creation Date: 2021.11.01
// Last Edited: 2021.11.04

// Note that this library has only been written in a couple of days
// So I haven't had the time to test much
// Expect bugs, leaks, usability problems, and compile errors
// In theory it should work on Windows and Posix

// Structure
// The library consists of a C-API (starting on line 73)
// and an C++-API that wraps that C-API (starting on line 1496)
// The library consists of the following subparts
//   - IPAddresses and domain resolution (C/C++: line 73, C++: 1527)
//   - TCP Sockets (reliable stream oriented) (C: line 154, C++: 1538)
//   - UDP Sockets (unreliable message oriented) (C: line 245, C++: 1738)
//   - Socket State API (C: line 289)
//   - SocketCollection/Socol (Callback API) (C: line 334, C++ line 1884)

//  IPv6 variations can be found beneath their corresponding IPv4 version 

// Usage
// This is a "single-header" library, meaning api and implementaion are contained
// within the same file (this file)
// To include the headers just use
//
// #include <net.h>
//
// for just the C-API and 
//
// #define NET_USE_CPP
// #include <net.h>
//
// For the C and C++ API
//
// Also include the implementation in one translation unit using
//
// #define NET_IMPLEMENTATION
// #include <net.h>
//
// #define NET_USE_CPP
// #define NET_IMPLEMENTATION
// #include <net.h>
//
// For just C and C/C++ respectively

// You can also uncomment these lines right now if you will only use the C++-API
// #ifndef NET_USE_CPP
// #define NET_USE_CPP
// #endif

// Examples
// Example code can be found on the libraries github page

#ifndef NET_H
#define NET_H

#include "inttypes.h"

#ifndef __cplusplus
#define bool int
#endif

#ifdef _WIN32
#define SocketHandle int64_t
#else
#define SocketHandle int
#endif

// An IPv4 Adress including a port
typedef struct IPAddress {
    uint32_t ip;
    uint16_t port;
} IPAddress;

// An IPv6 Adress including a port and its flowlabel
typedef struct IPAddress6 {
    uint8_t address[16];
    uint16_t port;
    uint32_t flowlabel;
} IPAddress6;

// Creates an IPv4 address including a port 
// Example: ip_address(127,0,0,1, 80) would create an address for port 80 on the local host
IPAddress ip_address(uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint16_t port);

// Same as ip_address, but for IPv6
IPAddress6 ip_address6(uint8_t address[16], uint16_t port, uint32_t flowlabel);

// Resolves an ip address based on the given domain and port.
// The domain can be an ip address like "127.0.0.1", 
// a label like "localhost" or a domain name like "wikipedia.com"
// The port can be either a number like "80" or a service name like "http"
// domain and port can be null in which case the localhost and port 0 are returned respectively
// Returns the resolved address in address and returns whether the call succeeded
bool resolve_ip_address(IPAddress* address, const char* domain, const char* port);

// Same as resolve_ip_address, but for IPv6
// Note that linux doesn't accept "localhost" for IPv6, so use "::1" instead
// at least the linux i was testing on 
bool resolve_ip_address6(IPAddress6* address, const char* domain, const char* port);

// Enum used in Socket to signal different ErrorCodes
typedef enum SocketError {
    // No error has occured, everything is according to plan
    SocketError_None,

    // The operating systems socket system failed or couldn't be initialized
    SocketError_SubsytemFailure,

    // The creation of a socket has failed 
    SocketError_FailedToCreateSocket,

    // The send operation has failed
    SocketError_SendFailed,
    
    // The receive operation has failed
    SocketError_ReceiveFailed,
    
    // The listen operation has failed
    SocketError_ListenFailed,

    // The binding to a port in udp_open has failed
    SocketError_BindFailed,

    // The connect operation failed, e.g. because of a timeout
    SocketError_ConnectFailed,
    
    // For listening sockets: the accept operation failed
    // For client sockets: there is currently no connection pending
    SocketError_AcceptFailed,

    // Failed to set the blocking mode of the socket
    SocketError_FailedToSetBlocking,

    // The resolve_domain operation inside a connect_domain call has failed
    // e.g. because the given domain doesn't exist
    SocketError_FailedToResolveDomain,

    // Not strictly an error, but indicates that the socket has been closed
    SocketError_Closed,
} SocketError;

// The TCP/UDP IPv4/IPv6 Socket
typedef struct Socket {
    SocketError error;
    SocketHandle handle;
} Socket;


/* TCP Socket API */

// Creates a new TCP socket for IPv4
Socket create_tcp_socket();

// Creates a new TCP socket for IPv6
Socket create_tcp_socket6();

// Sends the given data (size in bytes)
// Returns the actual number of bytes send, which may be less than the requested count
// Note that in blocking mode this function will block until 
// all given data was send or an error occured.
int tcp_send(Socket* socket, const void* data, int size);

// Same as tcp_send, but blocks for non-blocking sockets
bool tcp_send_exact(Socket* socket, const void* data, int size);

// Receives data from the socket and writes it into the buffer (size in bytes)
// Returns the actual number of bytes received
// the bytes received might be less than the specified number of bytes, 
// this is true even for blocking sockets. 
// Use tcp_receive_exact, if you need exact byte counts
int tcp_receive(Socket* socket, void* buffer, int buffer_size);

// Same as tcp_receive, but guarantees number of bytes read
// Returns true if data could be read, and false on error / close
// Note that this function might block, even for non-blocking sockets
bool tcp_receive_exact(Socket* socket, void* data, int size);

// Connects the socket to the given ip address
// This function will block in blocking mode until a connection
// has been established or an error occured
// In non-blocking mode it will return false immediately.
// You can then call socket_wait or socket_query_state
// (alternatively you can call tcp_connect_ip repeatedly) 
bool tcp_connect_ip(Socket* socket, IPAddress address);

// Same as resolve_ip_address, but for IPv6 
bool tcp_connect_ip6(Socket* socket, IPAddress6 address);

// convenience method for tcp_connect_ip
bool tcp_connect(Socket* socket, uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint16_t port);

// convenience method for tcp_connect using resolve_ip_address
bool tcp_connect_domain(Socket* socket, const char* domain, const char* port);

// convenience method for tcp_connect6 using resolve_ip_address6
bool tcp_connect_domain6(Socket* socket, const char* domain, const char* port);

// Sets the socket to blocking or non-blocking mode
void tcp_set_blocking(Socket* socket, bool blocking);

// Makes the socket listen for incoming connections on the given port
// Will return true on success and false on error
bool tcp_listen(Socket* socket, uint16_t port);

// Same as tcp_listen, but for IPv6 
bool tcp_listen6(Socket* socket, uint16_t port);

// Must be called after tcp_listen has already been called
// Returns a newly established connection in client
// Returns true if a new connection was established
// and false on error or for non-blocking sockets
// when no new connection is pending 
bool tcp_accept(Socket* socket, Socket* client, IPAddress* address);

// Same as tcp_accept, but for IPv6 
bool tcp_accept6(Socket* socket, Socket* client, IPAddress6* address);

// Same as tcp_accept, but always returns a Socket object
// Sets Socket.error to SocketError_AcceptFailed in case of pending connection
Socket tcp_accept_socket(Socket* socket, IPAddress* address);

// Same as tcp_accept_socket, but for IPv6 
Socket tcp_accept_socket6(Socket* socket, IPAddress6* address);

// Closes the specified socket (Cleanup)
void tcp_close(Socket* socket);

// Enum used as input to tcp_shutdown
typedef enum SocketShutdown {
    SocketShutdown_Receive,
    SocketShutdown_Send,
    SocketShutdown_Both
} SocketShutdown;

// Will partially close the socket to release some
// of its resources, like the send/receive buffer 
void tcp_shutdown(Socket* socket, SocketShutdown how);


/* UDP Socket API */

// Creates a new UDP socket for IPv4
Socket create_udp_socket();

// Creates a new UDP socket for IPv6
Socket create_udp_socket6();

// Binds the udp socket to the given port
// Returns true on success and false otherwise
// Note that udp sockets must be bound, before they
// can be used for sending / receiving 
bool udp_open(Socket* socket, uint16_t port);

// Same as udp_open, but for IPv6 
bool udp_open6(Socket* socket, uint16_t port, uint32_t flow_info);

// Sends the given data to the target ip address
// Returns the actual number of bytes send, which may be less than the requested count
// Note that in blocking mode this function will block until 
// the given data was send or an error occured.
int udp_send_ip(Socket* socket, IPAddress address, const void* data, int size);

// convenience method for udp_send_ip
int udp_send(Socket* socket, uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint16_t port, const void* data, int size);

// Same as udp_send_ip, but for IPv6 
int udp_send_ip6(Socket* socket, IPAddress6 address, const void* data, int size);

// Receives data from the socket and writes it into the buffer
// Returns the actual number of bytes received 
// and writes the sender's ip address into address
int udp_receive_from(Socket* socket, IPAddress* address, void* buffer, int buffer_size);

// Same as udp_receive_from, but for IPv6 
int udp_receive_from6(Socket* socket, IPAddress6* address, void* buffer, int buffer_size);

// Sets the socket to blocking or non-blocking mode
void udp_set_blocking(Socket* socket, bool blocking);

// Closes the specified socket (Cleanup)
void udp_close(Socket* socket);


/* Socket State API */

// Bitset indicating the current state of a socket
typedef enum SocketState {
    // no other State is applicable 
    SocketState_None     = 0,
    
    // new bufferspace is available in the sockets send buffer
    // Note that sends can still block, when too much data is given
    SocketState_Writeable = 1,  
    
    // new data is available, guarantees the next receive won't block
    SocketState_Readable  = 2,
    
    // a connection attempt failed (could also be: OOB data is available, but disabled)
    SocketState_Exception = 4,  

    // for listening sockets, indicates a new incoming connection
    SocketState_NewConnectionAccepted = SocketState_Readable,

    // indicates that a connecting call has finished
    SocketState_ConnectSucceeded      = SocketState_Writeable,
    
    // indicates the socket is in every state simultaneously
    SocketState_All = SocketState_Readable | SocketState_Writeable | SocketState_Exception
} SocketState;

// Queries the SocketStates for an array of Sockets 
// and writes their corresponding state into the states array 
// This function won't block, if you want to wait use sockets_wait
// Returns the number of non-SocketState_None sockets
int sockets_query_states(Socket* sockets, SocketState* states, int count);

// Similar to sockets_query_states, but will block until at least
// one socket has one of the states in states_to_wait_for
// Returns the number of non SocketState_None sockets
int sockets_wait(Socket* sockets, SocketState* states, int count, SocketState states_to_wait_for);

// convenience method for sockets_query_states for a single socket
SocketState socket_query_state(Socket* socket);

// convenience method for sockets_wait for a single socket
SocketState socket_wait(Socket* socket, SocketState states_to_wait_for);


/* SocketCollection (socol) API  */

/* Callbacks to be used with SocketCollection */

typedef struct Socol Socol;

// Callback for a newly accepted IPv4 Connection
typedef void (*SocketOnConnection)(Socol* socol, void* userblob, Socket new_socket, IPAddress address);

// Callback for a newly accepted IPv6 Connection
typedef void (*SocketOnConnection6)(Socol* socol, void* userblob, Socket new_socket, IPAddress6 address);

// Callback, if a socket encountered an error
typedef void (*SocketOnError)(void* userblob, Socket* socket);

// Callback for when a connecting socket has finished connecting
typedef void (*SocketOnConnected)(void* userblob, Socket* socket);

// Callback for when a socket is in the SocketState_Readable
typedef void (*SocketOnReadable)(void* userblob, Socket* socket);

// Callback for when a socket is in the SocketState_Writeable
typedef void (*SocketOnWriteable)(void* userblob, Socket* socket);

// Callback for when new data is available from the socket
// This won't be called, if a SocketOnReadable is defined
typedef void (*SocketOnData)(void* userblob, Socket* socket, void* data, int size);

// Enum used in SocketCollection to signal different Error codes 
typedef enum SocketCollectionError {
    // No error has occured, everything is according to plan
    SocketCollectionError_None,

    // An allocation has failed
    SocketCollectionError_AllocationFailure,
} SocketCollectionError;

// Represents a collection of sockets, offering a callback api
// Note that callbacks won't be called from a multithreaded environment
// but rather called synchronously, when calling socol_update
typedef struct Socol {
    // All members are for internal use only
    SocketCollectionError error;
    
    void* allocator;
    void* (*allocate)(void* allocator, int size);
    void (*deallocate)(void* allocator, void* ptr, int size);

    struct Colsock* listening;
    struct Colsock* listening6;
    struct Colsock* connecting;
    struct Colsock* connecting6;
    struct Colsock* udp;
    struct Colsock* udp6;
    struct Colsock* tcp;
    struct Colsock* tcp6;
} Socol;

// Creates a socket collection
Socol create_socket_collection();

// Destroys a socket collection and frees all its resources
void socol_destroy(Socol* socol);

// Sets the SocketCollections allocator providing a userpointer, an allocate and deallocate (free) function
void socol_allocator(Socol* socol, void* allocator, void* (allocate)(void*, int), void (deallocate)(void*, void*, int));

// Adds a new socket listening on the given port for IPv4 TCP connections
// Returns a handle to a SocketCollection socket 
// that can be used to set the callback functions
Socket* socol_listen(Socol* sockets, uint16_t port);

// Same as socol_listen, but for IPv6
Socket* socol_listen6(Socol* sockets, uint16_t port);

// Stops the socket listening on the given port
void socol_stop_listen(Socol* sockets, uint16_t port);

// Same as socol_stop_listen, but for IPv6
void socol_stop_listen6(Socol* sockets, uint16_t port);

// Adds a new IPv4 UDP socket bound to the given port
// Returns a handle to a SocketCollection socket 
// that can be used to set the callback functions
Socket* socol_udp(Socol* sockets, uint16_t port);

// Same as socol_udp, but for IPv6
Socket* socol_udp6(Socol* sockets, uint16_t port, uint32_t flowlabel);

// Adds a new IPv4 TCP socket that connects to the given IP address
// Returns a handle to a SocketCollection socket 
// that can be used to set the callback functions
Socket* socol_tcp_ip(Socol* sockets, IPAddress address);

// Same as socol_tcp_ip, but for IPv6
Socket* socol_tcp_ip6(Socol* sockets, IPAddress6 address);

// convenience method for socol_tcp_ip using resolve_address 
Socket* socol_tcp(Socol* sockets, const char* domain, const char* port);

// convenience method for socol_tcp6_ip using resolve_address 
Socket* socol_tcp6(Socol* sockets, const char* domain, const char* port);

// closes the socket given by its socol handle
void socol_close(Socket* socket);

// For udp sockets: Returns the current packet's sender address
// Note that these functions can only be used within
// the on_data and on_readable callbacks
IPAddress* socol_sender_ip(Socket* socket);

// Same as socol_sender_ip, but for IPv6
IPAddress6* socol_sender_ip6(Socket* socket);

// Sets the user blob for the given socket
// A user blob is like a userpointer, but instead of being a pointer
// you can specify an arbitrary data block, that gets copied and passed to your callbacks 
// The blob_destroyer gets called, before the userblob gets deallocated
void socol_userblob(Socol* socol, Socket* socket, void* userblob, int blobsize, void (*blob_destroyer)(void* userblob));

// Returns the pointer to the userblob for a given socket handle
void* socol_get_userblob(Socket* socket);

// Sets the on_connection callback called for an incoming IPv4 connection
void socol_on_connection(Socket* socket, SocketOnConnection on_connection);

// Same as socol_on_connection, but for IPv6
void socol_on_connection6(Socket* socket, SocketOnConnection6 on_connection);

// Sets the on_error callback called if an socket encountered an error (or close) 
void socol_on_error(Socket* socket, SocketOnError on_error);


// Sets the on_writeable callback called if an socket is writeable 
void socol_on_writeable(Socket* socket, SocketOnWriteable on_writeable);

// Sets the on_connnected callback called if a pending connection was successful 
void socol_on_connected(Socket* socket, SocketOnConnected on_connected);

// Sets the on_readable callback called if a socket is readable 
void socol_on_readable(Socket* socket, SocketOnReadable on_readable);

// Sets the on_data callback called if new data was received 
// This callback won't be called if an on_readable callback was specified 
void socol_on_data(Socket* socket, SocketOnData on_data);

// Adds a socket that was already connected, 
// e.g. the one received in the on_connection callback
Socket* socol_add_already_connected_socket(Socol* socol, Socket socket);

// Same as socol_add_already_connected_socket, but for IPv6
Socket* socol_add_already_connected_socket6(Socol* socol, Socket socket);

// Queries the states of all sockets in the collection
// and calls the specified callbacks
// Callbacks are always called synchronously to you calling this function 
void socol_update(Socol* sockets);

#endif

#ifdef NET_IMPLEMENTATION

#ifndef __cplusplus
#define true 1
#define false 0
#endif

#include <stdlib.h>

#ifdef _WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

#define last_error WSAGetLastError()
#define socket_error SOCKET_ERROR
#define invalid_socket INVALID_SOCKET
#define e_would_block WSAEWOULDBLOCK
#define e_already WSAEALREADY
#define e_isconn WSAEISCONN
// #define SocketHandle SOCKET
#define close_socket closesocket
#else
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>

#define last_error errno
#define socket_error (-1)
#define invalid_socket (-1)
#define e_would_block EWOULDBLOCK
#define e_already EALREADY
#define e_isconn EISCONN
#define close_socket close
#endif

#ifdef _WIN32

WSADATA wsa;
bool wsa_init = false;

bool init_wsa() {
    if (wsa_init) return true;
    int res = WSAStartup(MAKEWORD(2,2), &wsa);
    if (res != NO_ERROR) return false;
    
    wsa_init = true;
    return true;
}

#define init_networking() init_wsa()
#else

#define init_networking() true
#endif

IPAddress ip_address(uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint16_t port) {
    return (IPAddress){
        .ip = ( (uint32_t)a << 24 ) | ( (uint32_t)b << 16 ) | ( (uint32_t)c << 8 ) | (uint32_t)d,
        .port = port
    };
}

IPAddress6 ip_address6(uint8_t address[16], uint16_t port, uint32_t flowlabel) {
    IPAddress6 ip;
    memcpy(ip.address, address, 16);
    ip.port = port;
    ip.flowlabel = flowlabel;
    return ip;
}

bool resolve_ip_address(IPAddress* address, const char* domain, const char* port) {
    bool result = false;
    struct addrinfo* info;
    if (getaddrinfo(domain, port, 0, &info) == 0) {
        while (info) {            
            if (info->ai_family == AF_INET) {
                struct sockaddr_in* a = (struct sockaddr_in*)info->ai_addr;
                address->ip = ntohl(a->sin_addr.s_addr);
                address->port = ntohs(a->sin_port);
                result = true;
                break;
            }
            
            info = info->ai_next;
        }
    }

    freeaddrinfo(info);
    return result;
}

bool resolve_ip_address6(IPAddress6* address, const char* domain, const char* port) {
    if (!init_networking()) return false;
    bool result = false;
    struct addrinfo* info;
    if (getaddrinfo(domain, port, 0, &info) == 0) {
        while (info) {            
            if (info->ai_family == AF_INET6) {
                struct sockaddr_in6* a = (struct sockaddr_in6*)info->ai_addr;
                memcpy(address->address, &a->sin6_addr, 16);
                address->port = ntohs(a->sin6_port); 
                address->flowlabel = ntohl(a->sin6_flowinfo); 
                result = true;
                break;
            }
            info = info->ai_next;
        }
    }

    freeaddrinfo(info);
    return result;
}


Socket create_socket_internal(int family, int sock, int protocoll) {
    Socket s = { SocketError_None };
    if (!init_networking()) {
        s.error = SocketError_SubsytemFailure;
    } 
    else {
        s.handle = socket(family, sock, protocoll);

        if (s.handle == invalid_socket)
            s.error = SocketError_FailedToCreateSocket;
    }
    return s;
}

Socket create_tcp_socket() {
    return create_socket_internal(AF_INET, SOCK_STREAM, IPPROTO_TCP);
}

Socket create_tcp_socket6() {
    return create_socket_internal(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
}

int tcp_send(Socket* socket, const void* data, int size) {
    if (socket->error) return 0;
    int res = send(socket->handle, (const char*)data, size, 0);
    if (res != socket_error) return res;

    int error = last_error;
    if (error == e_would_block) return 0;

    socket->error = SocketError_SendFailed;
    return 0;
}

bool tcp_send_exact(Socket* socket, const void* data, int size) {
    uint8_t* data8 = (uint8_t*)data;
    while (size) {
        if (socket->error) return false;
        socket_wait(socket, SocketState_Writeable);
        int count = tcp_send(socket, data8, size);
        data8 += count;
        size -= count;
    }
    return true;
}

int tcp_receive(Socket* socket, void* buffer, int buffer_size) {
    if (socket->error) return 0;
    int res = recv(socket->handle, (char*)buffer, buffer_size, 0);
    if (res == socket_error) {
        int error = last_error;
        if (error == e_would_block) return 0;
        socket->error = SocketError_ReceiveFailed;
        return 0;
    }

    if (res == 0) socket->error = SocketError_Closed;

    return res;
}

bool tcp_receive_exact(Socket* socket, void* data, int size) {
    uint8_t* data8 = (uint8_t*)data;
    while (size) {
        if (socket->error) return false;
        socket_wait(socket, SocketState_Readable);
        int count = tcp_receive(socket, data8, size);
        data8 += count;
        size -= count;
    }
    return true;
}

bool tcp_connect_ip(Socket* socket, IPAddress address) {
    if (socket->error) return false;
    
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(address.ip);
    addr.sin_port = htons(address.port);

    int res = connect(socket->handle, (struct sockaddr*)&addr, sizeof(addr));
    if (res == socket_error) {
        int error = last_error;

        #ifdef _WIN32
        if (error ==     WSAEINVAL) return false;
        #endif
        if (error ==     e_already) return false;
        if (error == e_would_block) return false;
        
        if (error ==     e_isconn) return true;

        socket->error = SocketError_ConnectFailed;
        return false;
    }

    return true;
}

bool tcp_connect_ip6(Socket* socket, IPAddress6 address) {
    if (socket->error) return false;
    
    struct sockaddr_in6 addr = {0};
    addr.sin6_family = AF_INET6;
    memcpy(&addr.sin6_addr, address.address, 16);
    addr.sin6_port = htons(address.port);
    addr.sin6_flowinfo = htonl(address.flowlabel);
    addr.sin6_scope_id = 0;

    int res = connect(socket->handle, (struct sockaddr*)&addr, sizeof(addr));
    if (res == socket_error) {
        int error = last_error;

        #ifdef _WIN32
        if (error ==     WSAEINVAL) return false;
        #endif
        if (error ==     e_already) return false;
        if (error == e_would_block) return false;
        
        if (error ==     e_isconn) return true;

        socket->error = SocketError_ConnectFailed;
        return false;
    }

    return true;
}

bool tcp_connect(Socket* socket, uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint16_t port) {
    return tcp_connect_ip(socket, ip_address(a,b,c,d, port));
}

bool tcp_connect_domain(Socket* socket, const char* domain, const char* port) {
    IPAddress address;
    if (!resolve_ip_address(&address, domain, port)) {
        socket->error = SocketError_FailedToResolveDomain;
        return false;
    }

    return tcp_connect_ip(socket, address);
}

bool tcp_connect_domain6(Socket* socket, const char* domain, const char* port) {
    IPAddress6 address;
    if (!resolve_ip_address6(&address, domain, port)) {
        socket->error = SocketError_FailedToResolveDomain;
        return false;
    }

    return tcp_connect_ip6(socket, address);
}

void tcp_set_blocking(Socket* socket, bool blocking) {
    #ifdef _WIN32
    unsigned long nonblocking = !blocking;
    int res = ioctlsocket(socket->handle, FIONBIO, &nonblocking);
    if (res != NO_ERROR) socket->error = SocketError_FailedToSetBlocking;
    #else
    int flags = fcntl(socket->handle, F_GETFL, 0);
    if (flags == -1) {
        socket->error = SocketError_FailedToSetBlocking;
        return;
    }
    
    blocking ? (flags |= O_NONBLOCK) : (flags &= (~O_NONBLOCK));
    int res = fcntl(socket->handle, F_SETFL, flags);
    if (res == -1) socket->error = SocketError_FailedToSetBlocking;
    #endif
}

bool tcp_listen(Socket* socket, uint16_t port) {
    if (socket->error) return false;

    struct sockaddr_in address = {};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    int res = bind(socket->handle, (struct sockaddr*)&address, sizeof(address));
    if (res == socket_error) {
        socket->error = SocketError_ListenFailed;
        return false;
    }

    res = listen(socket->handle, SOMAXCONN);
    if (res == socket_error) {
        socket->error = SocketError_ListenFailed;
        return false;
    }

    return true;
}

bool tcp_listen6(Socket* socket, uint16_t port) {
    if (socket->error) return false;

    struct sockaddr_in6 address = {0};
    address.sin6_family = AF_INET6;
    // address.sin6_addr = 0;
    address.sin6_port = htons(port);

    int res = bind(socket->handle, (struct sockaddr*)&address, sizeof(address));
    if (res == socket_error) {
        socket->error = SocketError_ListenFailed;
        return false;
    }

    res = listen(socket->handle, SOMAXCONN);
    if (res == socket_error) {
        socket->error = SocketError_ListenFailed;
        return false;
    }

    return true;
}

bool tcp_accept(Socket* socket, Socket* client, IPAddress* address) {
    struct sockaddr_in addr;
    socklen_t size = sizeof(addr);
    
    SocketHandle handle = accept(socket->handle, (struct sockaddr*)&addr, &size);
    if (handle == invalid_socket) {
        int error = last_error;
        if (error != e_would_block)
            socket->error = SocketError_AcceptFailed;
        return false;
    }

    if (address) {
        address->ip = ntohl(addr.sin_addr.s_addr);
        address->port = ntohs( addr.sin_port );
    }

    *client = (Socket){ SocketError_None };
    client->handle = handle;
    return true;
}

bool tcp_accept6(Socket* socket, Socket* client, IPAddress6* address) {
    struct sockaddr_in6 addr;
    socklen_t size = sizeof(addr);

    SocketHandle handle = accept(socket->handle, (struct sockaddr*)&addr, &size);
    if (handle == invalid_socket) {
        int error = last_error;
        if (error != e_would_block)
            socket->error = SocketError_AcceptFailed;
        return false;
    }

    if (address) {
        memcpy(address->address, &addr.sin6_addr, 16);
        address->port = ntohs(addr.sin6_port);
        address->flowlabel = ntohl(addr.sin6_flowinfo);
    }

    *client = (Socket){ SocketError_None };
    client->handle = handle;
    return true;
}

Socket tcp_accept_socket(Socket* socket, IPAddress* address) {
    Socket client = { SocketError_None };
    if (!tcp_accept(socket, &client, address)) {
        client.handle = invalid_socket;
        client.error = SocketError_AcceptFailed;
    }
    return client;
}

Socket tcp_accept_socket6(Socket* socket, IPAddress6* address) {
    Socket client = { SocketError_None };
    if (!tcp_accept6(socket, &client, address)) {
        client.handle = invalid_socket;
        client.error = SocketError_AcceptFailed;
    }
    return client;
}

void tcp_close(Socket* socket) {
    if (socket->handle != invalid_socket) {
        close_socket(socket->handle);
        socket->handle = invalid_socket;
    }
}

void tcp_shutdown(Socket* socket, SocketShutdown how) {
    shutdown(socket->handle, how);
}

Socket create_udp_socket() {
    return create_socket_internal(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
}

Socket create_udp_socket6() {
    return create_socket_internal(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
}

void udp_set_blocking(Socket* socket, bool blocking) {
    tcp_set_blocking(socket, blocking);
}

void udp_close(Socket* socket) {
    tcp_close(socket);
}

bool udp_open(Socket* socket, uint16_t port) {
    if (socket->error) return false;

    struct sockaddr_in address = {0};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    int res = bind(socket->handle, (struct sockaddr*)&address, sizeof(address));
    if (res == socket_error) {
        socket->error = SocketError_BindFailed;
        return false;
    }

    return true;
}

bool udp_open6(Socket* socket, uint16_t port, uint32_t flow_info) {
    if (socket->error) return false;
    
    struct sockaddr_in6 address = {0};
    address.sin6_family = AF_INET6;
    // address.sin6_addr = 0;
    address.sin6_port = htons(port);
    address.sin6_flowinfo = htonl(flow_info);
    address.sin6_scope_id = 0;

    int res = bind(socket->handle, (struct sockaddr*)&address, sizeof(address));
    if (res == socket_error) {
        socket->error = SocketError_BindFailed;
        return false;
    }

    return true;
}

int udp_send_ip(Socket* socket, IPAddress address, const void* data, int size) {
    if (socket->error) return 0;
    
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl( address.ip );
    addr.sin_port = htons( address.port );

    int res = sendto(socket->handle, (const char*)data, size, 0, (struct sockaddr*)&addr, sizeof(addr));
    if (res != socket_error) return res;

    int error = last_error;
    if (error == e_would_block) return 0;

    socket->error = SocketError_SendFailed;
    return 0;
}

int udp_send_ip6(Socket* socket, IPAddress6 address, const void* data, int size) {
    if (socket->error) return 0;
    
    struct sockaddr_in6 addr = {0};
    addr.sin6_family = AF_INET6;
    memcpy(&addr.sin6_addr, address.address, 16);
    addr.sin6_port = htons(address.port);
    addr.sin6_flowinfo = htonl(address.flowlabel);
    addr.sin6_scope_id = 0;

    int res = sendto(socket->handle, (const char*)data, size, 0, (struct sockaddr*)&addr, sizeof(addr));
    if (res != socket_error) return res;

    int error = last_error;
    if (error == e_would_block) return 0;

    socket->error = SocketError_SendFailed;
    return 0;
}

int udp_send(Socket* socket, uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint16_t port, const void* data, int size) {
    return udp_send_ip(socket, ip_address(a,b,c,d, port), data, size);
}

int udp_receive_from(Socket* socket, IPAddress* address, void* buffer, int buffer_size) {
    if (socket->error) return 0;
    struct sockaddr_in addr;
    socklen_t addr_size = sizeof(addr);

    int res = recvfrom(socket->handle, (char*)buffer, buffer_size, 0, (struct sockaddr*)&addr, &addr_size);
    if (res == socket_error) {
        int error = last_error;
        if (error == e_would_block) return 0;
        socket->error = SocketError_ReceiveFailed;
        return 0;
    }

    if (res == 0) socket->error = SocketError_Closed;

    if (address) {
        address->ip = ntohl(addr.sin_addr.s_addr);
        address->port = ntohs(addr.sin_port);
    }

    return res;
}

int udp_receive_from6(Socket* socket, IPAddress6* address, void* buffer, int buffer_size) {
    if (socket->error) return 0;
    struct sockaddr_in6 addr;
    socklen_t addr_size = sizeof(addr);

    int res = recvfrom(socket->handle, (char*)buffer, buffer_size, 0, (struct sockaddr*)&addr, &addr_size);
    if (res == socket_error) {
        int error = last_error;
        if (error == e_would_block) return 0;
        socket->error = SocketError_ReceiveFailed;
        return 0;
    }

    if (res == 0) socket->error = SocketError_Closed;

    if (address) {
        memcpy(address->address, &addr.sin6_addr, 16);
        address->port = ntohs(addr.sin6_port);
        address->flowlabel = ntohl(addr.sin6_flowinfo);
    }

    return res;
}

int sockets_query_states_t(Socket* sockets, SocketState* states, int count, struct timeval* timeout, SocketState interested_states) {
    if (count == 0) return 0;
    fd_set readfds, writefds, exceptfds;
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    FD_ZERO(&exceptfds);
    for (int i = 0; i < count; ++i) {
        FD_SET(sockets[i].handle, &readfds);
        FD_SET(sockets[i].handle, &writefds);
        FD_SET(sockets[i].handle, &exceptfds);
    }
    
    int max_fd = 0;
    #ifndef _WIN32
    for (int i = 0; i < count; ++i) {
        if (sockets[i].handle > max_fd)
            max_fd = sockets[i].handle;
    }
    #endif

    int res = select(max_fd + 1, 
        (interested_states & SocketState_Readable) ? &readfds : 0, 
        (interested_states & SocketState_Writeable) ? &writefds : 0, 
        (interested_states & SocketState_Exception) ? &exceptfds : 0,
        timeout
    );

    if (res == socket_error) {
        for (int i = 0; i < count; ++i)
            states[i] = SocketState_None;
        return 0;
    }

    for (int i = 0; i < count; ++i) {
        SocketState state = SocketState_None;
        if (FD_ISSET(sockets[i].handle,   &readfds)) state = (SocketState)(state | SocketState_Readable); 
        if (FD_ISSET(sockets[i].handle,  &writefds)) state = (SocketState)(state | SocketState_Writeable); 
        if (FD_ISSET(sockets[i].handle, &exceptfds)) state = (SocketState)(state | SocketState_Exception);
        states[i] = state;
    }

    return res;
}

int sockets_query_states(Socket* sockets, SocketState* states, int count) {
    struct timeval t = {0};
    return sockets_query_states_t(sockets, states, count, &t, SocketState_All);
}

int sockets_wait(Socket* sockets, SocketState* states, int count, SocketState states_to_wait_for) {
    return sockets_query_states_t(sockets, states, count, 0, states_to_wait_for);
}

SocketState socket_query_state(Socket* socket) {
    if (socket->error) return SocketState_None;
    SocketState state;
    sockets_query_states(socket, &state, 1);
    return state;
}

SocketState socket_wait(Socket* socket, SocketState states_to_wait_for) {
    if (socket->error) return SocketState_None;
    SocketState state;
    sockets_wait(socket, &state, 1, states_to_wait_for);
    return state;
}


/////////////////////////////////////////////////////
void* mallocate(void* allocator, int size) {
    return malloc(size);
}

void mdeallocate(void* allocator, void* ptr, int size) {
    free(ptr);
}

Socol create_socket_collection() {
    Socol socol = { SocketCollectionError_None };
    socol.allocate = mallocate;
    socol.deallocate = mdeallocate;
    return socol;
}

void socol_allocator(Socol* socol, void* allocator, void* (allocate)(void*, int), void (deallocate)(void*, void*, int)) {
    socol->allocator = allocator;
    socol->allocate = allocate;
    socol->deallocate = deallocate;
}

typedef struct Colsock Colsock;
struct Colsock {
    Socket socket;
    uint16_t port;

    union {
        IPAddress* address;
        IPAddress6* address6;
    };

    void* userblob;
    int   blobsize;
    void (*blob_destroyer)(void* userblob);

    SocketOnError       on_error;
    SocketOnConnection  on_connection;    
    SocketOnConnection6 on_connection6;    
    SocketOnConnected   on_connected;
    SocketOnReadable    on_readable;
    SocketOnWriteable   on_writeable;
    SocketOnData        on_data;
    Colsock* next;
};

void socol_free_colsock(Socol* sockets, Colsock* s) {
    tcp_close(&s->socket);
    if (s->blob_destroyer) s->blob_destroyer(s->userblob);
    if (s->userblob) sockets->deallocate(sockets->allocator, s->userblob, s->blobsize);
    sockets->deallocate(sockets->allocator, s, sizeof(Colsock));
}

void socol_free_list(Socol* socol, Colsock* list) {
    while (list) {
        Colsock* s = list;
        list = s->next;
        socol_free_colsock(socol, s);
    }
}

void socol_destroy(Socol* socol) {
    socol_free_list(socol, socol->listening);
    socol_free_list(socol, socol->listening6);
    socol_free_list(socol, socol->connecting);
    socol_free_list(socol, socol->connecting6);
    socol_free_list(socol, socol->udp);
    socol_free_list(socol, socol->udp6);
    socol_free_list(socol, socol->tcp);
    socol_free_list(socol, socol->tcp6);
}


Socket* socol_listen_internal(Socol* sockets, uint16_t port, Socket socket, Colsock** list) {
    tcp_set_blocking(&socket, false);
    if (socket.error || !tcp_listen(&socket, port)) {
        tcp_close(&socket);
        return 0;
    }

    Colsock* s = (Colsock*)sockets->allocate(sockets->allocator, sizeof(Colsock));
    if (!s) {
        sockets->error = SocketCollectionError_AllocationFailure;
        return 0;
    }
    memset(s, 0, sizeof(Colsock));
    s->socket = socket;
    s->next = *list;
    s->port = port;
    *list = s;
    
    return &s->socket;
}

Socket* socol_listen(Socol* sockets, uint16_t port) {
    return socol_listen_internal(sockets, port, create_tcp_socket(), &sockets->listening);
}

Socket* socol_listen6(Socol* sockets, uint16_t port) {
    return socol_listen_internal(sockets, port, create_tcp_socket6(), &sockets->listening6);
}

void socol_stop_internal(Socol* sockets, uint16_t port, Colsock** list) {
    while(*list) {
        Colsock* s = *list;
        if (s->port == port) {
            *list = s->next;
            socol_free_colsock(sockets, s);
            break;
        }
        list = &(*list)->next;
    }
}

void socol_stop_listen(Socol* sockets, uint16_t port) {
    socol_stop_internal(sockets, port, &sockets->listening);
}

void socol_stop_listen6(Socol* sockets, uint16_t port) {
    socol_stop_internal(sockets, port, &sockets->listening6);
}


Colsock* socol_create_internal(Socol* sockets, Socket socket, Colsock** list) {
    tcp_set_blocking(&socket, false);
    if (socket.error) {
        tcp_close(&socket);
        return 0;
    }

    Colsock* s = (Colsock*)sockets->allocate(sockets->allocator, sizeof(Colsock));
    if (!s) {
        sockets->error = SocketCollectionError_AllocationFailure;
        return 0;
    }
    memset(s, 0, sizeof(Colsock));
    s->socket = socket;
    s->next = *list;
    *list = s;
    
    return s;
}

Socket* socol_udp(Socol* sockets, uint16_t port) {
    Colsock* s = socol_create_internal(sockets, create_udp_socket(), &sockets->udp);
    if (!s) return 0;
    if (udp_open(&s->socket, port))
        return &s->socket;
    sockets->udp = s->next;
    socol_free_colsock(sockets, s);
    return 0;
}

Socket* socol_udp6(Socol* sockets, uint16_t port, uint32_t flowlabel) {
    Colsock* s = socol_create_internal(sockets, create_udp_socket6(), &sockets->udp6);
    if (!s) return 0;
    if (udp_open6(&s->socket, port, flowlabel))
        return &s->socket;
    sockets->udp6 = s->next;
    socol_free_colsock(sockets, s);
    return 0;
}

Socket* socol_tcp_ip(Socol* sockets, IPAddress address) {
    Colsock* s = socol_create_internal(sockets, create_tcp_socket(), &sockets->connecting);
    if (!s) return 0;
    tcp_connect_ip(&s->socket, address);
    return &s->socket;
}

Socket* socol_tcp_ip6(Socol* sockets, IPAddress6 address) {
    Colsock* s = socol_create_internal(sockets, create_tcp_socket6(), &sockets->connecting6);
    if (!s) return 0;
    tcp_connect_ip6(&s->socket, address);
    return &s->socket;
}

Socket* socol_tcp(Socol* sockets, const char* domain, const char* port) {
    Colsock* s = socol_create_internal(sockets, create_tcp_socket(), &sockets->connecting);
    if (!s) return 0;
    tcp_connect_domain(&s->socket, domain, port);
    return &s->socket;
}

Socket* socol_tcp6(Socol* sockets, const char* domain, const char* port) {
    Colsock* s = socol_create_internal(sockets, create_tcp_socket6(), &sockets->connecting6);
    if (!s) return 0;
    tcp_connect_domain6(&s->socket, domain, port);
    return &s->socket;
}

void socol_close(Socket* socket) {
    if (!socket->error) socket->error = SocketError_Closed;
}

void socol_userblob(Socol* socol, Socket* socket, void* userblob, int blobsize, void (*blob_destroyer)(void* userblob)) {
    Colsock* s = (Colsock*)socket;
    s->userblob = socol->allocate(socol->allocator, blobsize);
    s->blobsize = blobsize;
    s->blob_destroyer = blob_destroyer;
    memcpy(s->userblob, userblob, blobsize);
}

void* socol_get_userblob(Socket* socket) {
    Colsock* s = (Colsock*)socket;
    return s->userblob;
}

void socol_on_connection(Socket* socket, SocketOnConnection on_connection) {
    Colsock* s = (Colsock*)socket;
    s->on_connection = on_connection;
}

void socol_on_connection6(Socket* socket, SocketOnConnection6 on_connection) {
    Colsock* s = (Colsock*)socket;
    s->on_connection6 = on_connection;
}

void socol_on_error(Socket* socket, SocketOnError on_error) {
    Colsock* s = (Colsock*)socket;
    s->on_error = on_error;
}

void socol_on_readable(Socket* socket, SocketOnReadable on_readable) {
    Colsock* s = (Colsock*)socket;
    s->on_readable = on_readable;
}

void socol_on_writeable(Socket* socket, SocketOnWriteable on_writeable) {
    Colsock* s = (Colsock*)socket;
    s->on_writeable = on_writeable;
}

void socol_on_connected(Socket* socket, SocketOnConnected on_connected) {
    Colsock* s = (Colsock*)socket;
    s->on_connected = on_connected;
}

void socol_on_data(Socket* socket, SocketOnData on_data) {
    Colsock* s = (Colsock*)socket;
    s->on_data = on_data;
}

Socket* socol_add_already_connected_socket(Socol* socol, Socket socket) {
    return &socol_create_internal(socol, socket, &socol->tcp)->socket;
}

Socket* socol_add_already_connected_socket6(Socol* socol, Socket socket) {
    return &socol_create_internal(socol, socket, &socol->tcp6)->socket;
}

IPAddress* socol_sender_ip(Socket* socket) {
    Colsock* s = (Colsock*)socket;
    return s->address;
}

IPAddress6* socol_sender_ip6(Socket* socket) {
    Colsock* s = (Colsock*)socket;
    return s->address6;
}

void socol_handel_sockets(Socol* sockets, Colsock** last, bool udp, bool ipv6) {
    char buffer[1024];
    IPAddress address;
    IPAddress6 address6;
    while (*last) {
        Colsock* s = *last; 
        SocketState state = socket_query_state(&s->socket);
        if (state & SocketState_Exception) {
            s->socket.error = SocketError_Closed;
        }
        else if (state & SocketState_Readable) {
            if (s->on_readable) {
                s->on_readable(s->userblob, &s->socket);
            } else {
                int res = ipv6 
                    ? (udp
                        ? udp_receive_from6(&s->socket, &address6, buffer, sizeof(buffer))
                        : tcp_receive(&s->socket, buffer, sizeof(buffer)))
                    : (udp
                        ? udp_receive_from(&s->socket, &address, buffer, sizeof(buffer))
                        : tcp_receive(&s->socket, buffer, sizeof(buffer)));
                if (udp &&  ipv6) s->address6 = &address6;
                if (udp && !ipv6) s->address  = &address;
                if (s->on_data) s->on_data(s->userblob, &s->socket, buffer, res);
            }
        }
        else if (state & SocketState_Writeable) {
            if (s->on_writeable) s->on_writeable(s->userblob, &s->socket);
        }

        if (s->socket.error) {
            if (s->on_error) s->on_error(s->userblob, &s->socket);
            *last = s->next;
            socol_free_colsock(sockets, s);
            continue;
        }

        last = &s->next;
    }
}

void socol_handel_connects(Socol* sockets, Colsock** last, Colsock** new_list) {
    while (*last) {
        Colsock* s = *last;
        SocketState state = socket_query_state(&s->socket);
        if (state & SocketState_Exception) {
            s->socket.error = SocketError_ConnectFailed;
        }
        else if (state & SocketState_ConnectSucceeded) {
            if (s->on_connected) s->on_connected(s->userblob, &s->socket);
            *last = s->next;
            s->next = *new_list;
            *new_list = s;
            continue;
        }
        
        if (s->socket.error) {
            if (s->on_error) s->on_error(s->userblob, &s->socket);
            *last = s->next;
            socol_free_colsock(sockets, s);
            continue;
        }

        last = &s->next;
    }
}

void socol_handle_listen(Socol* sockets, bool ipv6, Colsock** last) {
    IPAddress address;
    IPAddress6 address6;
    while (*last) {
        Colsock* s = *last;
        Socket newsock = ipv6 
            ? tcp_accept_socket6(&s->socket, &address6)
            : tcp_accept_socket (&s->socket, &address);
        if (!newsock.error) {
            if (ipv6 && s->on_connection6)
                s->on_connection6(sockets, s->userblob, newsock, address6);
            else if (!ipv6 && s->on_connection)
                s->on_connection(sockets, s->userblob, newsock, address);
            else tcp_close(&newsock);
        }

        if (s->socket.error) {
            if (s->on_error) s->on_error(s->userblob, &s->socket);
            *last = s->next;
            socol_free_colsock(sockets, s);
            continue;
        }
        
        last = &s->next;
    }
}

void socol_update(Socol* sockets) {
    socol_handle_listen(sockets, false, &sockets->listening);    
    socol_handle_listen(sockets, true, &sockets->listening6);    

    socol_handel_connects(sockets, &sockets->connecting,  &sockets->tcp );
    socol_handel_connects(sockets, &sockets->connecting6, &sockets->tcp6);

    socol_handel_sockets(sockets, &sockets->tcp , false, false);
    socol_handel_sockets(sockets, &sockets->tcp6, false,  true);
    socol_handel_sockets(sockets, &sockets->udp ,  true, false);
    socol_handel_sockets(sockets, &sockets->udp6 , true,  true);   
}


#endif


#ifdef NET_USE_CPP
#ifndef NETCPP_H
#define NETCPP_H

#include <iostream>
#include <fstream>
#include <optional>
#include <functional>

/* C++ API */ 

// Offers a std::streambuf interface for TCP Sockets
// Used within SocketStream
struct SocketStreamBuffer : public std::streambuf {
    Socket* socket;
    char buffer[64];

    // Creates the buffer for the given socket
    SocketStreamBuffer(Socket* socket);
protected: // Member functions needed to implement std::streambuf
    std::streamsize xsputn(const char_type* s, std::streamsize n) override;
    int_type overflow(int_type ch) override;
    int_type underflow() override;
};

// Offers a std::iostream interface for TCP Sockets
// For usage see TCPSocket.stream()
struct SocketStream : public std::iostream {
    SocketStreamBuffer buffer;

    // Creates a socket stream for the given socket
    SocketStream(Socket* socket);

    // Copy constructor
    SocketStream(const SocketStream& stream);
};

// Symbols and functions from the C API that are also Part of the C++ API
// IPAddress, IPAddress6, ip_address, ip_address6, SocketError, SocketState, SocketCollectionError

// Resolves an ip address based on the given domain and port.
// The domain can be an ip address like "127.0.0.1", 
// a label like "localhost" or a domain name like "wikipedia.com"
// The port can either be a number like "80" or a service name like "http"
// domain and port can be null in which case the localhost and port 0 are returned respectively
// Returns the resolved address, if the call succeeded and the domain could be resolved 
std::optional<IPAddress> resolve_ip_address(const char* domain, const char* port);

// Same as resolve_ip_address, but for IPv6 
std::optional<IPAddress6> resolve_ip_address6(const char* domain, const char* port);

// Represents an IPv4 TCP Socket
class TCPSocket {
    bool owner;
    Socket* socket;
public:
    // Creates the socket from the C-API socket and takes ownership over the socket 
    TCPSocket(Socket socket);

    // Creates the socket from the C-API socket and does NOT take ownership over the socket 
    TCPSocket(Socket* socket);
    
    // Creates a new TCP socket
    TCPSocket();
    TCPSocket(const TCPSocket&) = delete;
    TCPSocket(TCPSocket&& other);
    
    // Destructor
    ~TCPSocket();

    // Returns the current error code, see SocketError in net.h
    // for more details on what error does are possible
    // Note that SocketError_Closed is also considered an error 
    SocketError error();

    // Returns whether the socket is valid (i.e. no SocketError is present)
    // And actually refers to a socket
    bool valid();

    // Sends the given data (size in bytes)
    // Returns the actual number of bytes send, which may be less than the requested count
    // Note that in blocking mode this function will block until 
    // all given data was send or an error occured.
    int send(const void* data, uint32_t size);

    // Same as send, but blocks for non-blocking sockets
    int send_exact(const void* data, uint32_t size);

    // Receives data from the socket and writes it into the buffer (size in bytes)
    // Returns the actual number of bytes received
    // the bytes received might be less than the specified number of bytes, 
    // this is true even for blocking sockets. 
    // Use receive_exact, if you need exact byte counts
    int receive(void* buffer, int buffer_size);

    // Same as tcp_receive, but guarantees number of bytes read
    // Returns true if data could be read, and false on error / close
    // Note that this function might block, even for non-blocking sockets
    int receive_exact(void* data, int size);

    // Connects the socket to the given ip address
    // This function will block in blocking mode until a connection
    // has been established or an error occured
    // In non-blocking mode it will return false immediately.
    // You can then call wait or query_state
    // (alternatively you can call tcp_connect_ip repeatedly) 
    bool connect(IPAddress address);

    // convenience method for tcp_connect_ip
    bool connect(uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint16_t port);

    // convenience method for connect using resolve_ip_address
    bool connect(const std::string& domain, const std::string& port);

    // Sets the socket to blocking or non-blocking mode
    void set_blocking(bool blocking);

    // Makes the socket listen for incoming connections on the given port
    // Will return true on success and false on error
    bool listen(uint16_t port);

    // Must be called after tcp_listen has already been called
    // Returns a newly established connection
    // The returned socket won't be .valid() on error
    // or for non-blocking sockets if no connection is pending
    TCPSocket accept(IPAddress& address);

    // convenience method for accept(IPAddress&)
    TCPSocket accept();

    // Closes the specified socket (Cleanup)
    // This function will already be called by the
    // destructor of the owning socket
    void close();

    // Will partially close the socket to release some
    // of its resources, like the send/receive buffer 
    void shutdown(SocketShutdown how);

    // Queries the SocketState of the socket 
    // This function won't block, if you want to block use wait
    SocketState query_state();

    // Similar to query_state, but will block until the socket
    // is in one of the states specified in states_to_wait_for
    SocketState wait(SocketState states_to_wait_for);

    // Returns an std::iostream compatible object for the socket
    // The resulting stream will always block, even for non blocking sockets
    SocketStream stream();
};

// Represents an IPv6 TCP Socket
class TCPSocket6 {
    bool owner;
    Socket* socket;
public:
    // Creates the socket from the C-API socket and takes ownership over the socket 
    TCPSocket6(Socket socket);

    // Creates the socket from the C-API socket and does NOT take ownership over the socket 
    TCPSocket6(Socket* socket);
    
    // Creates a new TCP socket
    TCPSocket6();
    TCPSocket6(const TCPSocket6&) = delete;
    TCPSocket6(TCPSocket6&& other);
    
    // Destructor
    ~TCPSocket6();

    // Returns the current error code, see SocketError in net.h
    // for more details on what error does are possible
    // Note that SocketError_Closed is also considered an error 
    SocketError error();

    // Returns whether the socket is valid (i.e. no SocketError is present)
    // and actually refers to a socket (i.e. was not returned from an unsuccessful accept call)
    bool valid();

    // Sends the given data (size in bytes)
    // Returns the actual number of bytes send, which may be less than the requested count
    // Note that in blocking mode this function will block until 
    // all given data was send or an error occured.
    int send(const void* data, uint32_t size);

    // Same as send, but blocks for non-blocking sockets
    int send_exact(const void* data, uint32_t size);

    // Receives data from the socket and writes it into the buffer (size in bytes)
    // Returns the actual number of bytes received
    // the bytes received might be less than the specified number of bytes, 
    // this is true even for blocking sockets. 
    // Use receive_exact, if you need exact byte counts
    int receive(void* buffer, int buffer_size);

    // Same as tcp_receive, but guarantees number of bytes read
    // Returns true if data could be read, and false on error / close
    // Note that this function might block, even for non-blocking sockets
    int receive_exact(void* data, int size);

    // Connects the socket to the given ip address
    // This function will block in blocking mode until a connection
    // has been established or an error occured
    // In non-blocking mode it will return false immediately.
    // You can then call wait or query_state
    // (alternatively you can call tcp_connect_ip repeatedly) 
    bool connect(IPAddress6 address);

    // convenience method for connect using resolve_ip_address
    bool connect(const std::string& domain, const std::string& port);

    // Sets the socket to blocking or non-blocking mode
    void set_blocking(bool blocking);

    // Makes the socket listen for incoming connections on the given port
    // Will return true on success and false on error
    bool listen(uint16_t port);

    // Must be called after tcp_listen has already been called
    // Returns a newly established connection
    // The returned socket won't be .valid() on error
    // or for non-blocking sockets if no connection is pending
    TCPSocket6 accept(IPAddress6& address);

    // convenience method for accept(IPAddress&)
    TCPSocket6 accept();

    // Closes the specified socket (Cleanup)
    // This function will already be called by the
    // destructor of the owning socket
    void close();

    // Will partially close the socket to release some
    // of its resources, like the send/receive buffer 
    void shutdown(SocketShutdown how);

    // Queries the SocketState of the socket 
    // This function won't block, if you want to block use wait
    SocketState query_state();

    // Similar to query_state, but will block until the socket
    // is in one of the states specified in states_to_wait_for
    SocketState wait(SocketState states_to_wait_for);

    // Returns an std::iostream compatible object for the socket
    // The resulting stream will always block, even for non blocking sockets
    SocketStream stream();
};


// Represents an IPv4 UDP Socket
class UDPSocket {
    bool owner;
    Socket* socket;
public:
    // Creates the socket from the C-API socket and takes ownership over the socket 
    UDPSocket(Socket socket);

    // Creates the socket from the C-API socket and does NOT take ownership over the socket 
    UDPSocket(Socket* socket);
    
    // Creates a new TCP socket
    UDPSocket();
    UDPSocket(const UDPSocket&) = delete;
    UDPSocket(UDPSocket&& other);
    
    // Destructor
    ~UDPSocket();

    // Returns the current error code, see SocketError in net.h
    // for more details on what error does are possible
    // Note that SocketError_Closed is also considered an error 
    SocketError error();

    // Returns whether the socket is valid (i.e. no SocketError is present)
    // and actually refers to a socket
    bool valid();

    // Binds the udp socket to the given port
    // Returns true on success and false otherwise
    // Note that udp sockets must be bound, before they
    // can be used for sending / receiving 
    bool open(Socket* socket, uint16_t port = 0);

    
    // Sends the given data to the target ip address
    // Returns the actual number of bytes send, which may be less than the requested count
    // Note that in blocking mode this function will block until 
    // the given data was send or an error occured.
    int send(IPAddress address, const void* data, int size);

    // convenience method for send
    int send(uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint16_t port, const void* data, int size);

    // Receives data from the socket and writes it into the buffer
    // Returns the actual number of bytes received 
    // and writes the sender's ip address into address
    int receive_from(IPAddress& address, void* buffer, int buffer_size);

    // convenience method for receive_from
    int receive_from(void* buffer, int buffer_size);

    // Sets the socket to blocking or non-blocking mode
    void set_blocking(bool blocking);

    // Closes the specified socket (Cleanup)
    // This function will already be called by the
    // destructor of the owning socket
    void close();

    // Will partially close the socket to release some
    // of its resources, like the send/receive buffer 
    void shutdown(SocketShutdown how);

    // Queries the SocketState of the socket 
    // This function won't block, if you want to block use wait
    SocketState query_state();

    // Similar to query_state, but will block until the socket
    // is in one of the states specified in states_to_wait_for
    SocketState wait(SocketState states_to_wait_for);
};

// Represents an IPv6 UDP Socket
class UDPSocket6 {
    bool owner;
    Socket* socket;
public:
    // Creates the socket from the C-API socket and takes ownership over the socket 
    UDPSocket6(Socket socket);

    // Creates the socket from the C-API socket and does NOT take ownership over the socket 
    UDPSocket6(Socket* socket);
    
    // Creates a new TCP socket
    UDPSocket6();
    UDPSocket6(const UDPSocket6&) = delete;
    UDPSocket6(UDPSocket6&& other);
    
    // Destructor
    ~UDPSocket6();

    // Returns the current error code, see SocketError in net.h
    // for more details on what error does are possible
    // Note that SocketError_Closed is also considered an error 
    SocketError error();

    // Returns whether the socket is valid (i.e. no SocketError is present)
    // and actually refers to a socket
    bool valid();

    // Binds the udp socket to the given port
    // Returns true on success and false otherwise
    // Note that udp sockets must be bound, before they
    // can be used for sending / receiving 
    bool open(uint16_t port = 0, uint32_t flowlabel = 0);

    
    // Sends the given data to the target ip address
    // Returns the actual number of bytes send, which may be less than the requested count
    // Note that in blocking mode this function will block until 
    // the given data was send or an error occured.
    int send(IPAddress6 address, const void* data, int size);

    // Receives data from the socket and writes it into the buffer
    // Returns the actual number of bytes received 
    // and writes the sender's ip address into address
    int receive_from(IPAddress6& address, void* buffer, int buffer_size);

    // convenience method for receive_from
    int receive_from(void* buffer, int buffer_size);

    // Sets the socket to blocking or non-blocking mode
    void set_blocking(bool blocking);

    // Closes the specified socket (Cleanup)
    // This function will already be called by the
    // destructor of the owning socket
    void close();

    // Will partially close the socket to release some
    // of its resources, like the send/receive buffer 
    void shutdown(SocketShutdown how);

    // Queries the SocketState of the socket 
    // This function won't block, if you want to block use wait
    SocketState query_state();

    // Similar to query_state, but will block until the socket
    // is in one of the states specified in states_to_wait_for
    SocketState wait(SocketState states_to_wait_for);
};

// Handles used in SocketCollection
typedef Socket* SocolHandle;

// Represents a collection of sockets, offering a callback api
// Note that callbacks won't be called from a multithreaded environment
// but rather called synchronously, when calling socol_update
typedef class SocketCollection {
    Socol socol;
    SocolHandle last;

    void init_blob(SocolHandle handle);
public:
    
    // Creates a socket collection
    SocketCollection();
    
    // Destructor
    ~SocketCollection();

    // Sets the SocketCollections allocator providing a userpointer, an allocate and deallocate (free) function
    void set_allocator(void* allocator, void* (allocate)(void*, int), void (deallocate)(void*, void*, int));

    // Adds a new socket listening on the given port for IPv4 TCP connections
    // This function can be piped to allow easier setting of callbacks 
    SocketCollection& listen(uint16_t port);
    
    // Same as listen, but for IPv6
    SocketCollection& listen6(uint16_t port);

    // Stops the socket listening on the given port
    void stop_listen(uint16_t port);

    // Same as stop_listen, but for IPv6
    void stop_listen6(uint16_t port);

    // Adds a new IPv4 TCP socket that connects to the given IP address
    // This function can be piped to allow easier setting of callbacks
    SocketCollection& tcp(IPAddress address);

    // convenience method for tcp using resolve_address 
    SocketCollection& tcp(const char* domain, const char* port);

    // Same as tcp, but for IPv6
    SocketCollection& tcp6(Socol* sockets, IPAddress6 address);

    // convenience method for tcp6 using resolve_address 
    SocketCollection& tcp6(Socol* sockets, const char* domain, const char* port);

    // Adds a new IPv4 UDP socket bound to the given port
    // This function gets piped to allow easier setting of callbacks
    SocketCollection& udp(uint16_t port);
    
    // Same as udp, but for IPv6
    SocketCollection& udp6(uint16_t port, uint32_t flowlabel = 0);

    // closes the socket given by its socol handle
    void close(SocolHandle handle);

    // Returns whether the last call that created a socket has succeeded
    bool succeeded();
    
    // Returns the last created socket handle
    SocolHandle handle();

    // Sets the on_connection callback called for an incoming IPv4 connection
    // Note that the callback will receive a C-API Socket, that you must first convert
    // to a C++ Socket (i.e. TCPSocket cpp_socket{socket}) when necessary.
    // Alternatively you can pass the Socket to add_already_connected_socket to add it to the SocketCollection
    SocketCollection& on_connection(std::function<void(Socket socket, IPAddress address)> cb);

    // Same as on_connection, but for IPv6
    SocketCollection& on_connection6(std::function<void(Socket socket, IPAddress6 address)> cb);

    // Sets the on_error callback called if an socket encountered an error (or close) 
    // Note that the callback will receive a C-API Socket, that you must first convert
    // to a C++ Socket (i.e. TCPSocket cpp_socket{socket}) when necessary.
    SocketCollection& on_error(std::function<void(Socket* socket)> cb);

    // Sets the on_connnected callback called if a pending connection was successful 
    // Note that the callback will receive a C-API Socket, that you must first convert
    // to a C++ Socket (i.e. TCPSocket cpp_socket{socket}) when necessary.
    SocketCollection& on_connected(std::function<void(Socket* socket)> cb);

    // Sets the on_readable callback called if a socket is readable 
    SocketCollection& on_readable(std::function<void(TCPSocket  socket)> cb);
    SocketCollection& on_readable(std::function<void(TCPSocket6 socket)> cb);
    SocketCollection& on_readable(std::function<void(UDPSocket socket)> cb);
    SocketCollection& on_readable(std::function<void(UDPSocket6 socket)> cb);
    
    // Sets the on_writeable callback called if an socket is writeable 
    SocketCollection& on_writeable(std::function<void(TCPSocket  socket)> cb);
    SocketCollection& on_writeable(std::function<void(TCPSocket6 socket)> cb);
    SocketCollection& on_writeable(std::function<void(UDPSocket socket)> cb);
    SocketCollection& on_writeable(std::function<void(UDPSocket6 socket)> cb);

    // Sets the on_data callback called if new data was received 
    // This callback won't be called if an on_readable callback was specified
    // Note that the callback will receive a C-API Socket, that you must first convert
    // to a C++ Socket (i.e. TCPSocket cpp_socket{socket}) when necessary.
    SocketCollection& on_data(std::function<void(Socket* socket, void* data, int size)> cb);

    // Adds a socket that was already connected, 
    // e.g. the one received in the on_connection callback
    SocketCollection& add_already_connected_socket(Socket socket);

    // Same as add_already_connected_socket, but for IPv6
    SocketCollection& add_already_connected_socket6(Socket socket);

    // Queries the states of all sockets in the collection
    // and calls the specified callbacks
    // Callbacks are always called synchronously to you calling this function 
    void update();

} SocketCollection;

#endif

#ifdef NET_IMPLEMENTATION

std::streamsize SocketStreamBuffer::xsputn(const SocketStreamBuffer::char_type* s, std::streamsize n) {
    return tcp_send(socket, s, (int)n);
}

SocketStreamBuffer::int_type SocketStreamBuffer::overflow(SocketStreamBuffer::int_type ch) { 
    char c = ch;
    return tcp_send_exact(socket, &c, 1);
}

SocketStreamBuffer::int_type SocketStreamBuffer::underflow() { 
    int res = tcp_receive(socket, buffer, sizeof(buffer));
    if (res == 0)
        res = tcp_receive_exact(socket, buffer, 1);
    setg(buffer, buffer, buffer+res);
    return (res==0) ? std::streambuf::traits_type::eof() : buffer[0];
}

SocketStreamBuffer::SocketStreamBuffer(Socket* socket) : socket(socket) {}


SocketStream::SocketStream(Socket* socket) : buffer(socket), std::iostream(&buffer) {}
SocketStream::SocketStream(const SocketStream& stream) : buffer(stream.buffer.socket), std::iostream(&buffer) {}

std::optional<IPAddress> resolve_ip_address(const char* domain, const char* port) {
    IPAddress address;
    if (resolve_ip_address(&address, domain, port)) return address;
    else return std::nullopt; 
}


std::optional<IPAddress6> resolve_ip_address6(const char* domain, const char* port) {
    IPAddress6 address;
    if (resolve_ip_address6(&address, domain, port)) return address;
    else return std::nullopt; 
}

TCPSocket::TCPSocket(Socket socket) 
    : socket(new Socket), owner(true) {
    *this->socket = socket;
}

TCPSocket::TCPSocket(Socket* socket) 
    : socket(socket), owner(false) {}

TCPSocket::TCPSocket() : TCPSocket(create_tcp_socket()) {}
TCPSocket::TCPSocket(TCPSocket&& other) : socket(other.socket), owner(other.owner) {
    other.owner = false;
}

TCPSocket::~TCPSocket() { 
    if (owner) { close(); delete socket; } 
}

SocketError TCPSocket::error() { return socket->error; }

bool TCPSocket::valid() { return socket && socket->error == SocketError_None; }

int TCPSocket::send(const void* data, uint32_t size) {
    return tcp_send(socket, data, size);
}

int TCPSocket::send_exact(const void* data, uint32_t size) {
    return tcp_send_exact(socket, data, size);
}

int TCPSocket::receive(void* buffer, int buffer_size) {
    return tcp_receive(socket, buffer, buffer_size);
}

int TCPSocket::receive_exact(void* data, int size) {
    return tcp_receive_exact(socket, data, size);
}

bool TCPSocket::connect(IPAddress address) {
    return tcp_connect_ip(socket, address);
}

bool TCPSocket::connect(uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint16_t port) {
    return tcp_connect(socket, a,b,c,d, port);
}

bool TCPSocket::connect(const std::string& domain, const std::string& port) {
    return tcp_connect_domain(socket, domain.c_str(), port.c_str());
}

void TCPSocket::set_blocking(bool blocking) {
    tcp_set_blocking(socket, blocking);
}

bool TCPSocket::listen(uint16_t port) {
    return tcp_listen(socket, port);
}

TCPSocket TCPSocket::accept(IPAddress& address) {
    Socket client;
    if (tcp_accept(socket, &client, &address))
        return TCPSocket(client);
    return TCPSocket(nullptr);
}

TCPSocket TCPSocket::accept() {
    IPAddress address;
    return accept(address);
}

void TCPSocket::close() { tcp_close(socket); }

void TCPSocket::shutdown(SocketShutdown how) { tcp_shutdown(socket, how); }

SocketState TCPSocket::query_state() { return socket_query_state(socket); }

SocketState TCPSocket::wait(SocketState states_to_wait_for) { return socket_wait(socket, states_to_wait_for); }

SocketStream TCPSocket::stream() { return SocketStream(socket); }

 // Creates the socket from the C-API socket and takes ownership over the socket 
TCPSocket6::TCPSocket6(Socket socket) 
    : socket(new Socket), owner(true) {
    *this->socket = socket;
}

TCPSocket6::TCPSocket6(Socket* socket) 
    : socket(socket), owner(false) {}

TCPSocket6::TCPSocket6() : TCPSocket6(create_tcp_socket6()) {}
TCPSocket6::TCPSocket6(TCPSocket6&& other) : socket(other.socket), owner(other.owner) {
    other.owner = false;
}

TCPSocket6::~TCPSocket6() { if (owner) { close(); delete socket; } }

SocketError TCPSocket6::error() { return socket->error; }

bool TCPSocket6::valid() { return socket && socket->error == SocketError_None; }

int TCPSocket6::send(const void* data, uint32_t size) {
    return tcp_send(socket, data, size);
}

int TCPSocket6::send_exact(const void* data, uint32_t size) {
    return tcp_send_exact(socket, data, size);
}

int TCPSocket6::receive(void* buffer, int buffer_size) {
    return tcp_receive(socket, buffer, buffer_size);
}

int TCPSocket6::receive_exact(void* data, int size) {
    return tcp_receive_exact(socket, data, size);
}

bool TCPSocket6::connect(IPAddress6 address) {
    return tcp_connect_ip6(socket, address);
}

bool TCPSocket6::connect(const std::string& domain, const std::string& port) {
    return tcp_connect_domain6(socket, domain.c_str(), port.c_str());
}

void TCPSocket6::set_blocking(bool blocking) {
    tcp_set_blocking(socket, blocking);
}

bool TCPSocket6::listen(uint16_t port) {
    return tcp_listen6(socket, port);
}

TCPSocket6 TCPSocket6::accept(IPAddress6& address) {
    Socket client;
    if (tcp_accept6(socket, &client, &address))
        return TCPSocket6(client);
    return TCPSocket6(nullptr);
}

TCPSocket6 TCPSocket6::accept() {
    IPAddress6 address;
    return accept(address);
}

void TCPSocket6::close() { tcp_close(socket); }

void TCPSocket6::shutdown(SocketShutdown how) { tcp_shutdown(socket, how); }

SocketState TCPSocket6::query_state() { return socket_query_state(socket); }

SocketState TCPSocket6::wait(SocketState states_to_wait_for) { return socket_wait(socket, states_to_wait_for); }

SocketStream TCPSocket6::stream() { return SocketStream(socket); }

UDPSocket::UDPSocket(Socket socket) 
    : socket(new Socket), owner(true) {
    *this->socket = socket;
}

UDPSocket::UDPSocket(Socket* socket) 
    : socket(socket), owner(false) {}

UDPSocket::UDPSocket() : UDPSocket(create_udp_socket()) {}
UDPSocket::UDPSocket(UDPSocket&& other) : socket(other.socket), owner(other.owner) {
    other.owner = false;
}

UDPSocket::~UDPSocket() { if (owner) { close(); delete socket; } }

SocketError UDPSocket::error() { return socket->error; }

bool UDPSocket::valid() { return socket && socket->error == SocketError_None; }

bool UDPSocket::open(Socket* socket, uint16_t port) {
    return udp_open(socket, port);
}


int UDPSocket::send(IPAddress address, const void* data, int size) {
    return udp_send_ip(socket, address, data, size);
}

int UDPSocket::send(uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint16_t port, const void* data, int size) {
    return udp_send(socket, a,b,c,d, port, data, size);
}

int UDPSocket::receive_from(IPAddress& address, void* buffer, int buffer_size) {
    return udp_receive_from(socket, &address, buffer, buffer_size);
}

int UDPSocket::receive_from(void* buffer, int buffer_size) {
    return udp_receive_from(socket, nullptr, buffer, buffer_size);
}

void UDPSocket::set_blocking(bool blocking) {
    tcp_set_blocking(socket, blocking);
}

void UDPSocket::close() { tcp_close(socket); }

void UDPSocket::shutdown(SocketShutdown how) { tcp_shutdown(socket, how); }

SocketState UDPSocket::query_state() { return socket_query_state(socket); }

SocketState UDPSocket::wait(SocketState states_to_wait_for) { return socket_wait(socket, states_to_wait_for); }


UDPSocket6::UDPSocket6(Socket socket) 
    : socket(new Socket), owner(true) {
    *this->socket = socket;
}

UDPSocket6::UDPSocket6(Socket* socket) 
    : socket(socket), owner(false) {}

UDPSocket6::UDPSocket6() : UDPSocket6(create_udp_socket6()) {}
UDPSocket6::UDPSocket6(UDPSocket6&& other) : socket(other.socket), owner(other.owner) {
    other.owner = false;
}

UDPSocket6::~UDPSocket6() { if (owner) { close(); delete socket; } }

SocketError UDPSocket6::error() { return socket->error; }

bool UDPSocket6::valid() { return socket && socket->error == SocketError_None; }

bool UDPSocket6::open(uint16_t port, uint32_t flowlabel) {
    return udp_open6(socket, port, flowlabel);
}


int UDPSocket6::send(IPAddress6 address, const void* data, int size) {
    return udp_send_ip6(socket, address, data, size);
}

int UDPSocket6::receive_from(IPAddress6& address, void* buffer, int buffer_size) {
    return udp_receive_from6(socket, &address, buffer, buffer_size);
}

int UDPSocket6::receive_from(void* buffer, int buffer_size) {
    return udp_receive_from6(socket, nullptr, buffer, buffer_size);
}

void UDPSocket6::set_blocking(bool blocking) {
    tcp_set_blocking(socket, blocking);
}

void UDPSocket6::close() { tcp_close(socket); }

void UDPSocket6::shutdown(SocketShutdown how) { tcp_shutdown(socket, how); }

SocketState UDPSocket6::query_state() { return socket_query_state(socket); }

SocketState UDPSocket6::wait(SocketState states_to_wait_for) { return socket_wait(socket, states_to_wait_for); }

// A Blob of callbacks used internally
struct SocketCollectionCppBlob {
    std::function<void(Socket, IPAddress)> on_connection;
    std::function<void(Socket, IPAddress6)> on_connection6;
    std::function<void(Socket*)> on_error;
    std::function<void(Socket*)> on_connected;
    std::function<void(Socket*)> on_readable;
    std::function<void(Socket*)> on_writeable;
    std::function<void(Socket*, void*, int)> on_data;
    ~SocketCollectionCppBlob() {}
};

void SocketCollection::init_blob(SocolHandle handle) {
    if (!last) return;
    char init[sizeof(SocketCollectionCppBlob)];
    socol_userblob(&socol, handle, init, sizeof(init), [](void* blob){
        auto callbacks = (SocketCollectionCppBlob*)blob;
        callbacks->~SocketCollectionCppBlob();
    });
    auto blob = (SocketCollectionCppBlob*)socol_get_userblob(handle);
    new (blob) SocketCollectionCppBlob;
}


SocketCollection::SocketCollection() : socol(create_socket_collection()) {}

SocketCollection::~SocketCollection() { socol_destroy(&socol); }

void SocketCollection::set_allocator(void* allocator, void* (allocate)(void*, int), void (deallocate)(void*, void*, int)) {
    socol_allocator(&socol, allocator, allocate, deallocate);
}

SocketCollection& SocketCollection::listen(uint16_t port) {
    last = socol_listen(&socol, port);
    init_blob(last);
    return *this;
}

SocketCollection& SocketCollection::listen6(uint16_t port) {
    last = socol_listen6(&socol, port);
    init_blob(last);
    return *this;
}

void SocketCollection::stop_listen(uint16_t port) {
    socol_stop_listen(&socol, port);
}

void SocketCollection::stop_listen6(uint16_t port) {
    socol_stop_listen(&socol, port);
}

SocketCollection& SocketCollection::tcp(IPAddress address) {
    last = socol_tcp_ip(&socol, address);
    init_blob(last);
    return *this;
}

SocketCollection& SocketCollection::tcp(const char* domain, const char* port) {
    last = socol_tcp(&socol, domain, port);
    init_blob(last);
    return *this;
}

SocketCollection& SocketCollection::tcp6(Socol* sockets, IPAddress6 address) {
    last = socol_tcp_ip6(&socol, address);
    init_blob(last);
    return *this;
}

SocketCollection& SocketCollection::tcp6(Socol* sockets, const char* domain, const char* port) {
    last = socol_tcp6(&socol, domain, port);
    init_blob(last);
    return *this;
}

SocketCollection& SocketCollection::udp(uint16_t port) {
    last = socol_udp(&socol, port);
    init_blob(last);
    return *this;
}

SocketCollection& SocketCollection::udp6(uint16_t port, uint32_t flowlabel) {
    last = socol_udp6(&socol, port, flowlabel);
    init_blob(last);
    return *this;
}

void SocketCollection::close(SocolHandle handle) { socol_close(handle); }

bool SocketCollection::succeeded() { return last != nullptr; }

SocolHandle SocketCollection::handle() { return last; }

SocketCollection& SocketCollection::on_connection(std::function<void(Socket socket, IPAddress address)> cb) {
    socol_on_connection(last, [](Socol* socol, void* userblob, Socket new_socket, IPAddress address) {
        auto b = (SocketCollectionCppBlob*)userblob;
        b->on_connection(new_socket, address);
    });
    ((SocketCollectionCppBlob*)socol_get_userblob(last))->on_connection = cb;
    return *this;
}

SocketCollection& SocketCollection::on_connection6(std::function<void(Socket socket, IPAddress6 address)> cb) {
    socol_on_connection6(last, [](Socol* socol, void* userblob, Socket new_socket, IPAddress6 address) {
        auto b = (SocketCollectionCppBlob*)userblob;
        b->on_connection6(new_socket, address);
    });
    ((SocketCollectionCppBlob*)socol_get_userblob(last))->on_connection6 = cb;
    return *this;
}

SocketCollection& SocketCollection::on_error(std::function<void(Socket* socket)> cb) {
    socol_on_error(last, [](void* userblob, Socket* socket) {
        auto b = (SocketCollectionCppBlob*)userblob;
        b->on_error(socket);
    });
    ((SocketCollectionCppBlob*)socol_get_userblob(last))->on_error = cb;
    return *this;
}

SocketCollection& SocketCollection::on_connected(std::function<void(Socket* socket)> cb) {
    socol_on_connected(last, [](void* userblob, Socket* socket) {
        auto b = (SocketCollectionCppBlob*)userblob;
        b->on_connected(socket);
    });
    ((SocketCollectionCppBlob*)socol_get_userblob(last))->on_connected = cb;
    return *this;
}

#define on_readable_function(SocketType) \
    SocketCollection& SocketCollection::on_readable(std::function<void(SocketType socket)> cb) {      \
        socol_on_readable(last, [](void* userblob, Socket* socket) {                \
            auto b = (SocketCollectionCppBlob*)userblob;                            \
            b->on_writeable(socket);                                                \
        });                                                                         \
        ((SocketCollectionCppBlob*)socol_get_userblob(last))->on_readable = [=](Socket* socket) { \
            cb(SocketType{socket});                                                 \
        };                                                                          \
        return *this;                                                               \
    }

on_readable_function(TCPSocket)
on_readable_function(TCPSocket6)
on_readable_function(UDPSocket)
on_readable_function(UDPSocket6)

#undef on_readable_function

#define on_writeable_function(SocketType) \
    SocketCollection& SocketCollection::on_writeable(std::function<void(SocketType socket)> cb) {     \
        socol_on_writeable(last, [](void* userblob, Socket* socket) {               \
            auto b = (SocketCollectionCppBlob*)userblob;                            \
            b->on_writeable(socket);                                                \
        });                                                                         \
        ((SocketCollectionCppBlob*)socol_get_userblob(last))->on_writeable = [=](Socket* socket) { \
            cb(SocketType{socket});                                                 \
        };                                                                          \
        return *this;                                                               \
    }

on_writeable_function(TCPSocket)
on_writeable_function(TCPSocket6)
on_writeable_function(UDPSocket)
on_writeable_function(UDPSocket6)

#undef on_writeable_function

SocketCollection& SocketCollection::on_data(std::function<void(Socket* socket, void* data, int size)> cb) {
    socol_on_data(last, [](void* userblob, Socket* socket, void* data, int size) {
        auto b = (SocketCollectionCppBlob*)userblob;
        b->on_data(socket, data, size);
    });
    ((SocketCollectionCppBlob*)socol_get_userblob(last))->on_data = cb;
    return *this;
}

SocketCollection& SocketCollection::add_already_connected_socket(Socket socket) {
    last = socol_add_already_connected_socket(&socol, socket);
    init_blob(last);
    return *this;
}

SocketCollection& SocketCollection::add_already_connected_socket6(Socket socket) {
    last = socol_add_already_connected_socket6(&socol, socket);
    init_blob(last);
    return *this;
}

void SocketCollection::update() { socol_update(&socol); }

#endif // NETCPP_H
#endif // NET_USE_CPP

/*
MIT License

Copyright (c) 2021 Cunicus

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
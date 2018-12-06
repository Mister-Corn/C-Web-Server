/**
 * webserver.c -- A webserver written in C
 * 
 * Test with curl (if you don't have it, install it):
 * 
 *    curl -D - http://localhost:3490/
 *    curl -D - http://localhost:3490/d20
 *    curl -D - http://localhost:3490/date
 * 
 * You can also test the above URLs in your browser! They should work!
 * 
 * Posting Data:
 * 
 *    curl -D - -X POST -H 'Content-Type: text/plain' -d 'Hello, sample data!' http://localhost:3490/save
 * 
 * (Posting data is harder to test from a browser.)
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/file.h>
#include <fcntl.h>
#include "net.h"
#include "file.h"
#include "mime.h"
#include "cache.h"

#define PORT "3490"  // the port users will be connecting to

#define SERVER_FILES "./serverfiles"
#define SERVER_ROOT "./serverroot"

#define DEBUG 1
/**
 * Send an HTTP response
 *
 * header:       "HTTP/1.1 404 NOT FOUND" or "HTTP/1.1 200 OK", etc.
 * content_type: "text/plain", etc.
 * body:         the data to send.
 * 
 * Return the value from the send() function.
 */
int send_response(int fd, char *header, char *content_type, void *body, int content_length)
{
    const int max_response_size = 254000;
    char response[max_response_size];
    // Time info from https://www.ibm.com/support/knowledgecenter/en/ssw_ibm_i_71/rtref/asctime.htm
    struct tm *newtime;
    time_t ltime;
    // Build HTTP response and store it in response

    ///////////////////
    // IMPLEMENT ME! //
    ///////////////////
    // Construct response string and put it in response
    // Return value of sprintf is the number of
    // characters written. We can use that for
    // response_length
    time(&ltime);
    newtime = localtime(&ltime);
    char *formatted_time = asctime(newtime);

    int response_length = sprintf(response, 
        "%s\n"
        "Connection: close\n"
        "Date: %s"
        "Content-Length: %d\n"
        "Content-Type: %s\n\n",
        header,
        formatted_time,
        content_length,
        content_type
    );

    printf("sprintf response_length: %d\n", response_length);
    response_length = strlen(response);

    memcpy(response + response_length, body, content_length);

    int total_length = response_length + content_length;
    #if DEBUG
    puts("---RESPONSE---");
    printf("response_length:\t%d\n", response_length);
    printf("content_length:\t\t%d\n", content_length);
    printf("response: %s\n", response);
    printf("total_length:\t\t%d\n", total_length);
    // printf("send_response RES: \n%s\n", response);
    puts("---------");
    #endif
    // Send it all!
    int rv = send(fd, response, total_length, 0);

    if (rv < 0) {
        perror("send");
    }

    return rv;
}


/**
 * Send a /d20 endpoint response
 */
void get_d20(int fd)
{
    // Generate a random number between 1 and 20 inclusive
    int result = rand() % 21;
    ///////////////////
    // IMPLEMENT ME! //
    ///////////////////
    char header[] = "HTTP/1.1 200 OK";
    char content_type[] = "text/plain";
    char body[8];
    // Copy result to body string
    int content_length = sprintf(body, "%d", result);
    // Use send_response() to send it back as text/plain data
    send_response(fd, header, content_type, body, content_length);

    ///////////////////
    // IMPLEMENT ME! //
    ///////////////////
}

/**
 * Send a 404 response
 */
void resp_404(int fd)
{
    char filepath[4096];
    struct file_data *filedata; 
    char *mime_type;

    // Fetch the 404.html file
    snprintf(filepath, sizeof filepath, "%s/404.html", SERVER_FILES);
    filedata = file_load(filepath);

    if (filedata == NULL) {
        // TODO: make this non-fatal
        fprintf(stderr, "cannot find system 404 file\n");
        exit(3);
    }

    mime_type = mime_type_get(filepath);

    send_response(fd, "HTTP/1.1 404 NOT FOUND", mime_type, filedata->data, filedata->size);

    file_free(filedata);
}

/**
 * Read and return a file from disk or cache
 */
void get_file(int fd, struct cache *cache, char *request_path)
{
    ///////////////////
    // IMPLEMENT ME! //
    ///////////////////
    char filepath[4096];
    struct file_data *filedata;
    char *mime_type;

    snprintf(filepath, sizeof filepath, "%s%s", SERVER_ROOT, request_path);
    
    filedata = file_load(filepath);

    if (filedata == NULL) {
        fprintf(stderr, "cannot find system 404 file\n");
        resp_404(fd);
        return;
    }

    #if DEBUG
    puts("---get_file---");
    printf("Request Path: %s\n", request_path);
    printf("Filepath: %s\n", filepath);
    printf("filedata->size: %d\n", filedata->size);
    puts("---------");
    #endif


    mime_type = mime_type_get(filepath);

    send_response(fd, "HTTP/1.1 200 OK", mime_type, filedata->data, filedata->size);

    file_free(filedata);
}

/**
 * Search for the end of the HTTP header
 * 
 * "Newlines" in HTTP can be \r\n (carriage return followed by newline) or \n
 * (newline) or \r (carriage return).
 */
char *find_start_of_body(char *header)
{
    ///////////////////
    // IMPLEMENT ME! // (Stretch)
    ///////////////////
}

/**
 * Handle HTTP request and send response
 */
void handle_http_request(int fd, struct cache *cache)
{
    const int request_buffer_size = 65536; // 64K
    char request[request_buffer_size];

    // Read request
    int bytes_recvd = recv(fd, request, request_buffer_size - 1, 0);

    if (bytes_recvd < 0) {
        perror("recv");
        return;
    }

    
    ///////////////////
    // IMPLEMENT ME! //
    ///////////////////

    // Read the three components of the first request line
    char operation[256];
    char endpoint[256];
    char protocol[256];

    sscanf(request, "%s %s %s",
        &operation, &endpoint, &protocol);

    #if DEBUG
    printf("OP: %s\nENDPOINT: %s\nPROTOCOL: %s\n",
    operation, endpoint, protocol);
    #endif

    // If GET, handle the get endpoints
    if (strcmp(operation, "GET") == 0) {
        //    Check if it's /d20 and handle that special case
        if (strcmp(endpoint, "/d20") == 0) {
            #if DEBUG
            printf("/d20 condition TRUE\n");
            #endif
            get_d20(fd);
            return;
        }
        else {
            get_file(fd, cache, endpoint);
            return;
        }
        //    Otherwise serve the requested file by calling get_file()
    }
    // If we get here, we didn't find the op/endpoint:
    // resp_404(fd);
    


    // (Stretch) If POST, handle the post request
}

/**
 * Main
 */
int main(void)
{
    srand(time(NULL));
    int newfd;  // listen on sock_fd, new connection on newfd
    struct sockaddr_storage their_addr; // connector's address information
    char s[INET6_ADDRSTRLEN];

    struct cache *cache = cache_create(10, 0);

    // Get a listening socket
    int listenfd = get_listener_socket(PORT);

    if (listenfd < 0) {
        fprintf(stderr, "webserver: fatal error getting listening socket\n");
        exit(1);
    }

    printf("webserver: waiting for connections on port %s...\n", PORT);

    // This is the main loop that accepts incoming connections and
    // forks a handler process to take care of it. The main parent
    // process then goes back to waiting for new connections.
    
    while(1) {
        socklen_t sin_size = sizeof their_addr;

        // Parent process will block on the accept() call until someone
        // makes a new connection:
        newfd = accept(listenfd, (struct sockaddr *)&their_addr, &sin_size);
        if (newfd == -1) {
            perror("accept");
            continue;
        }

        // Print out a message that we got the connection
        inet_ntop(their_addr.ss_family,
            get_in_addr((struct sockaddr *)&their_addr),
            s, sizeof s);
        printf("server: got connection from %s\n", s);
        
        // newfd is a new socket descriptor for the new connection.
        // listenfd is still listening for new connections.

        handle_http_request(newfd, cache);

        close(newfd);
    }

    // Unreachable code

    return 0;
}


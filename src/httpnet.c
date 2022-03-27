#include <stdio.h>
#include <sys/socket.h>
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <unistd.h>
#include "httpnet.h"


int GetAddrByName(char *hostname, char **first_ip){
    struct hostent *host;
    struct in_addr **addr_list;

    host = gethostbyname(hostname);
    if (host == NULL){
        perror("Get Host By Name Error!");
        return -1;
    }

    addr_list = (struct in_addr **)host->h_addr_list;
    if (addr_list[0] != NULL){
        *first_ip = inet_ntoa(*addr_list[0]);
    }

    switch(host->h_addrtype){
        case AF_INET:
            return IPV4;
        case AF_INET6:
            return IPV6;
        default:
            perror("Unknown Address Type!");
            return -2;
    }
}


int CreateSocketConnect(char *domain,
                        int socket_type,
                        int protocol_type){
    /*
     * -1: Create Socket Error.
     * -2: IP Type Error.
     * -3: IP Convert into ASCII Error.
     * -4: Connect to Server Error.
     */
    struct sockaddr_in socket_addr;
    char *ip_str = NULL;
    int ip_type = 0, port = 0;
    int sockfd, addr_convert_flag, connect_flag;

    ip_type = GetAddrByName(domain, &ip_str);
    switch(ip_type){
        case IPV4:
            ip_type = AF_INET;
            port = HTTPPORT;
            break;
        case IPV6:
            ip_type = AF_INET6;
            port = HTTPSPORT;
            break;
        default:
            return -2;
    }

    sockfd = socket(ip_type, socket_type, protocol_type);
    if (sockfd < 0){
        perror("Create Socket Failed!");
        return -1;
    }

    memset(&socket_addr, 0, sizeof(socket_addr));
    socket_addr.sin_family = ip_type;
    socket_addr.sin_port = htons(port);

    addr_convert_flag = inet_pton(ip_type, ip_str, &socket_addr.sin_addr);
    if (addr_convert_flag <= 0){
        perror("Convert IP Address Falied!");
        return -3;
    }

    connect_flag = connect(sockfd, (struct sockaddr *)&socket_addr, sizeof(socket_addr));
    if (connect_flag < 0){
        perror("Connect to Server Falied!");
        return -4;
    }

    return sockfd;
}

int SendAll(int sockfd, char *send_data){
    char buffer[BUFFERSIZE] = {0};
    int send_flag, send_len = 0;

    while (*send_data != 0){
        for (int i = 0; i < BUFFERSIZE; i++){
            buffer[i] = *send_data++;
            send_len++;
            if (*send_data == 0){
                break;
            }
        }
        send_flag = send(sockfd, buffer, send_len, 0);
        send_len = 0;
        if (send_flag < 0){
            perror("Send Data Failed!");
            return -1;
        }
    }

    return 0;
}

int RecvAll2(int sockfd,
            char **recv_data,
            ulong *recv_len){
    char buffer[BUFFERSIZE];
    char s;
    int recv_flag = -1;
    LNode *datalink, *data_ptr;

    datalink = (LNode *)malloc(sizeof(LNode));
    datalink->len = 0;
    datalink->next = NULL;

    while (1){
        memset(buffer, 0, BUFFERSIZE);
        for (int i = 0; i < BUFFERSIZE; i++){
            if (recv(sockfd, &s, 1, MSG_PEEK | MSG_DONTWAIT) == 1 || 
                    errno == EINTR || errno == EWOULDBLOCK || 
                    errno == EAGAIN){
                recv_flag = recv(sockfd, &s, 1, 0);
                buffer[i] = s;
                (*recv_len)++;
            }
            else {
                close(sockfd);
                break;
            }
        }
        // *recv_len += recv_flag;
        PushData(datalink, buffer, recv_flag);
    }

    *recv_data = (char *)malloc(sizeof(char)*(BUFFERSIZE*datalink->len));
    data_ptr = datalink->next;
    for (int i = 0; i < datalink->len; i++){
        memcpy(*recv_data+BUFFERSIZE*i, data_ptr->data, BUFFERSIZE);
        data_ptr = data_ptr->next;
    }

    return 0;
}

int RecvAll(int sockfd,
            char **recv_data,
            ulong *recv_len){
    char buffer[BUFFERSIZE];
    int recv_flag = -1;
    LNode *datalink, *data_ptr;

    datalink = (LNode *)malloc(sizeof(LNode));
    datalink->len = 0;
    datalink->next = NULL;

    while (1){
        memset(buffer, 0, BUFFERSIZE);
        recv_flag = recv(sockfd, buffer, BUFFERSIZE, MSG_WAITALL);
        if (recv_flag < 0){
            perror("Receive Data Failed!");
            close(sockfd);
            return errno;
        }
        else if (recv_flag == 0){
            close(sockfd);
            break;
        }
        *recv_len += recv_flag;
        PushData(datalink, buffer, recv_flag);
    }

    *recv_data = (char *)malloc(sizeof(char)*(BUFFERSIZE*datalink->len));
    data_ptr = datalink->next;
    for (int i = 0; i < datalink->len; i++){
        memcpy(*recv_data+BUFFERSIZE*i, data_ptr->data, BUFFERSIZE);
        data_ptr = data_ptr->next;
    }

    return 0;
}

int SendRequestHeader(int sockfd, 
                      ReqHeader header,
                      char *post_data){
    int header_length = 0, send_flag = 0;
    char *header_str = NULL;
    ulong data_length = post_data==NULL?0:strlen(post_data);

    for (int i = 0; i < (int)(sizeof(header)/sizeof(char *)); i++){
        if (header.header_params[i] != NULL){
            header_length += strlen(header.header_params[i]);
        }
    }
    header_str = (char *)malloc(sizeof(char)*(header_length + data_length + 5));
    memset(header_str, 0, header_length+3);
    for (int i = 0; i < (int)(sizeof(header)/sizeof(char *)); i++){
        if (header.header_params[i] != NULL){
            strcat(header_str, header.header_params[i]);
        }
    }
    strcat(header_str, "\r\n");
    if (data_length != 0){
        strcat(header_str, post_data);
    }
    send_flag = send(sockfd, header_str, strlen(header_str), 0);
    if (send_flag < 0){
        return -1;
    }

    return 0;
}


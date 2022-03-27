#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "requests.h"
#include "httpnet.h"

int RA(int sockfd, char **recv_data, ulong *recv_len){
    char buffer[BUFFERSIZE];
    int recv_flag = -1;
    LNode *datalink, *data_ptr;

    datalink = (LNode *)malloc(sizeof(LNode));
    datalink->len = 0;
    datalink->next = NULL;

    while (1) {
        memset(buffer, 0, BUFFERSIZE);

        
    }
}

int main(){
    const char link = "http://www.baidu.com";
    const int link_len = strlen(link);
    int sockfd;
    ulong recv_len = 0, dest_len = 0;
    char linkaddr[link_len];
    char *http_secure = NULL, *host = NULL, *url = NULL;
    char *recv_data = NULL;
    uchar *uncompressed_data = NULL;
    ReqHeader header;
    Response *res;

    strcpy(linkaddr, link);

    flag = SplitLink(linkaddr, &http_secure, &host, &url);
    if (flag < 0) exit(-1);

    for (int i = 0; i < (int)(sizeof(header) / sizeof(char *)); i++)
        header.header_params[i] = custom_header.header_params[i];
    flag = CreateRequestHeader(&header, url, "GET", host);
    if (flag < 0) exit(-1);

    sockfd = CreateSocketConnect(host, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd < 0) exit(-1);

    flag = SendRequestHeader(sockfd, header, NULL);
    if (flag < 0) exit(-1);

}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "requests.h"
#include "httpnet.h"
// #include "parser.h"


int HttpGet(char *link, 
            ReqHeader custom_header, 
            Response **response){
    const int link_len = strlen(link);
    int sockfd;
    int flag = 1;
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

    flag = RecvAll(sockfd, &recv_data, &recv_len);
    if (flag < 0) exit(-1);

    flag = FindContent(recv_data, recv_len, &res);
    if (flag < 0) exit(-1);

    if (res->content_encoding != NULL){
        if (strncmp(res->content_encoding, "gzip", 4) == 0){
            flag = UnCompressGzip(res->content, res->content_len, &uncompressed_data, &dest_len);
            if (flag < 0) exit(-1);
            res->content = (char *)uncompressed_data;
            res->content_len = dest_len;
        }
        else{
            //TODO
        }
    }

    *response = res;

    return 0;
}

int HttpPost(char *link, 
            ReqHeader custom_header, 
            Response **response,
            char *data_list[],
            int list_len){
    const int link_len = strlen(link);
    int sockfd;
    int flag = 1;
    ulong recv_len = 0, dest_len = 0;
    char linkaddr[link_len];
    char *http_secure = NULL, *host = NULL, *url = NULL;
    char *recv_data = NULL, *post_data = NULL;
    uchar *uncompressed_data = NULL;
    ReqHeader header;
    Response *res;

    strcpy(linkaddr, link);

    flag = SplitLink(linkaddr, &http_secure, &host, &url);
    if (flag < 0) exit(-1);

    for (int i = 0; i < (int)(sizeof(header) / sizeof(char *)); i++)
        header.header_params[i] = custom_header.header_params[i];
    post_data = join(data_list, "&", list_len);
    header.headers.content_type = "application/x-www-form-urlencoded;charset=utf-8";
    header.headers.content_length = int2str(strlen(post_data));
    flag = CreateRequestHeader(&header, url, "POST", host);
    if (flag < 0) exit(-1);

    sockfd = CreateSocketConnect(host, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd < 0) exit(-1);

    flag = SendRequestHeader(sockfd, header, post_data);
    if (flag < 0) exit(-1);

    flag = RecvAll(sockfd, &recv_data, &recv_len);
    if (flag < 0) exit(-1);

    flag = FindContent(recv_data, recv_len, &res);
    if (flag < 0) exit(-1);

    if (res->content_encoding != NULL){
        if (strncmp(res->content_encoding, "gzip", 4) == 0){
            flag = UnCompressGzip(res->content, res->content_len, &uncompressed_data, &dest_len);
            if (flag < 0) exit(-1);
            res->content = (char *)uncompressed_data;
            res->content_len = dest_len;
        }
        else{
            //TODO
        }
    }

    *response = res;

    return 0;
}

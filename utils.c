#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <math.h>
#include "utils.h"

char *HEADERPARAM[] = headerparam;

int PushData(LNode *link, char *data, int copy_len){
    LNode *node, *tail = link;
    node = (LNode *)malloc(sizeof(LNode));
    while (tail->next != NULL){
        tail = tail->next;
    }
    link->len++;
    memcpy(node->data, data, copy_len);
    node->next = tail->next;
    tail->next = node;

    return 0;
}

char *tolwr(char *str){
    char *orign = str;
    for (; *str != '\0'; str++)
        *str = tolower(*str);

    return orign;
}

char *int2str(ulong num){
    int i = num, count = 0;
    char *str = NULL, tmp;

    while ((int)(i/pow(10, count++))%10);

    str = (char *)malloc(sizeof(char)*count);
    memset(str, 0, count);
    str += count;

    count = 0;
    while (1){
        tmp = ((int)(num/pow(10, count++)))%10+0x30;
        memcpy(--str, &tmp, 1);
        if ((int)(num/pow(10, count))%10 == 0)
            break;
    }

    return str;
}

char *split(char *str, const char *delim, char **saveptr){
    if (str == NULL){
        *saveptr = NULL;
        return str;
    }
    if (strlen(str) < strlen(delim)){
        *saveptr = NULL;
        return str;
    }
    assert(delim != NULL);

    int cmplen = strlen(delim);
    char *flag = NULL, *tmp_save = NULL, *ret = NULL;

    // printf("str: %s\nstrlen: %lu\n\n", str, strlen(str));

    flag = (char *)malloc(sizeof(char)*cmplen);

    for (ulong i = 0; i < strlen(str)-cmplen; i++){
        memcpy(flag, str+i, cmplen);
        if (!strncmp(flag, delim, cmplen)){
            tmp_save = (char *)malloc(sizeof(char)*(strlen(str)-i));
            strcpy(tmp_save, str+i+cmplen);
            *saveptr = tmp_save;
            ret = (char *)malloc(sizeof(char)*i);
            memcpy(ret, str, i);

            return ret;
        }
    }

    *saveptr = NULL;
    return str;
}

char *join(char *data_list[], char *stuff, int list_len){
    assert(stuff != NULL);
    if (list_len == 0)
        return NULL;

    ulong str_len = 0;
    int i = 0;
    char *str = NULL;

    for (int i = 0; i < list_len; i++)
        str_len += strlen(data_list[i]);
    str_len += (strlen(stuff)*list_len);
    
    str = (char *)malloc(sizeof(char)*(str_len+1));
    memset(str, 0, str_len+1);
    while (1){
        strcat(str, data_list[i]);
        if (++i < list_len)
            strcat(str, stuff);
        else
            break;
    }
    /* for (int i = 0; i < list_len; i++){
        strcat(str, data_list[i]);
        strcat(str, stuff);
    } */

    return str;
}

int SplitLink(char *link,
              char **http_secure,
              char **host,
              char **url){
    char *tmp = NULL;
    if (link == NULL)
        return -1;
    if ((*http_secure = split(link, "://", &link)) == NULL)
        return -2;
    if ((*host = split(link, "/", url)) == NULL)
        return -3;
    if (*url == NULL)
        *url = "/";
    else{
        tmp = *url;
        *url = (char *)malloc(sizeof(char)*strlen(tmp)+2);
        memset(*url, 0, strlen(tmp)+2);
        strcat(*url, "/");
        strcat(*url, tmp);
    }

    return 0;
}

int CreateRequestHeader(ReqHeader *header, 
                        char *url, 
                        char *req_method, 
                        char *host){
    int header_NULL = 1;

    for (int i = 0; i < (int)(sizeof(*header)/sizeof(char *)); i++){
        header_NULL &= (header->header_params[i] == NULL);
    }
    if (header_NULL){
        header->headers.method_url_type = (char *)malloc(sizeof(char)*(strlen(url) + strlen(req_method) + 13));
        sprintf(header->headers.method_url_type, "%s %s HTTP/1.1\r\n", req_method, url);
        header->headers.accept = "Accept: */*\r\n";
        header->headers.accept_encoding = "Accept-Encoding: gzip, deflate, br\r\n";
        header->headers.accept_language = "Accept-Language: zh-CN,zh\r\n";
        header->headers.connection = "Connection: keep-alive\r\n";
        header->headers.user_agent = "User-Agent: MyRequests\r\n";
        header->headers.host = (char *)malloc(sizeof(char)*(strlen(host) + 9));
        sprintf(header->headers.host, "Host: %s\r\n", host);
    }
    else{
        header->headers.method_url_type = (char *)malloc(sizeof(char)*(strlen(url) + strlen(req_method) + 13));
        sprintf(header->headers.method_url_type, "%s %s HTTP/1.1\r\n", req_method, url);
        for (int i = 1; i < (int)(sizeof(*header)/sizeof(char *)); i++){
            if (header->header_params[i] != NULL){
                char *tmp = header->header_params[i];
                header->header_params[i] = (char *)malloc(sizeof(char)*(strlen(tmp) + strlen(HEADERPARAM[i]) + 3));
                sprintf(header->header_params[i], "%s%s\r\n", HEADERPARAM[i], tmp);
            }
            else{
                switch(i){
                    case 1:
                        header->headers.accept = "Accept: */*\r\n";
                        break;
                    case 3:
                        header->headers.accept_encoding = "Accept-Encoding: gzip, deflate, br\r\n";
                        break;
                    case 4:
                        header->headers.accept_language = "Accept-Language: zh-CN,zh\r\n";
                        break;
                    case 8:
                        header->headers.connection = "Connection: close\r\n";
                        break;
                    case 15:
                        header->headers.host = (char *)malloc(sizeof(char)*(strlen(host) + 9));
                        sprintf(header->headers.host, "Host: %s\r\n", host);
                        break;
                    case 28:
                        header->headers.user_agent = "User_Agent: crequests v0.0.1\r\n";
                        break;
                }
            }
        }
    }

    return 0;
}

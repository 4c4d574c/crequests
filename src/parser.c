#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>
#include "parser.h"

int DeChunked(char *chunked_data,
              ulong chunked_len,
              char **dechunked_data,
              ulong *dechunked_len){
    char chunked_flag[2];
    ulong copyed_len = 0;

    *dechunked_data = (char *)malloc(sizeof(char)*(chunked_len));
    memset(*dechunked_data, 0, chunked_len+1);
    *dechunked_len = 0;
    for (ulong i = 0; i < chunked_len - 1; i++){
        memcpy(chunked_flag, chunked_data+i, 2);
        if (strncmp(chunked_flag, "\r\n", 2) == 0){
            i += 2;
            if (i >= chunked_len - 1)
                break;
            for (int j = 0; j < 5; j++){
                memcpy(chunked_flag, chunked_data+i, 2);
                if (strncmp(chunked_flag, "\r\n", 2) == 0){
                    memcpy((*dechunked_data)+(*dechunked_len), chunked_data+copyed_len, i-j-2-copyed_len);
                    *dechunked_len += (i-j-2-copyed_len);
                    i += 2;
                    copyed_len = i;
                    break;
                }
                i++;
                if (i >= chunked_len - 1)
                    break;
            }
        }
    }
    if (copyed_len < chunked_len){
        memcpy((*dechunked_data)+(*dechunked_len), chunked_data+copyed_len, chunked_len-copyed_len);
        *dechunked_len += (chunked_len-copyed_len);
    }
    if (*dechunked_len == 0){
        *dechunked_data = chunked_data;
        *dechunked_len = chunked_len;
    }

    return 0;
}

int GetValue(char **key, char *value){
    char *tmp_value = NULL;
    uint line_len;

    if (value == NULL){
        tmp_value = (char *)malloc(sizeof(char)*2);
        memset(tmp_value, 0, 2);
        strcpy(tmp_value, "");
    }
    else{
        line_len = (*value==' ')?strlen(value+1):strlen(value);
        tmp_value = (char *)malloc(sizeof(char)*(line_len+1));
        memset(tmp_value, 0, line_len+1);
        strcpy(tmp_value, value+(strlen(value)-line_len));
    }

    *key = tmp_value;

    return 0;
}

int GetCookie(Response *resheader, char *value){
    // char **cookie_part, **part_ptr;
    char *part, *cookie, *tmp_part;
    char *part_key, *part_value;
    // int part_num = 0;
    // resheader->cookie_num = 0;

    cookie = (char *)malloc(sizeof(char)*(strlen(value)+1));
    memset(cookie, 0, strlen(value)+1);
    strcpy(cookie, value);
    resheader->cookies[resheader->cookie_num]->cookie_value = NULL;
    resheader->cookies[resheader->cookie_num]->path = NULL;
    resheader->cookies[resheader->cookie_num]->domain = NULL;
    resheader->cookies[resheader->cookie_num]->max_age = NULL;

    // printf("cookie: %s\n", cookie);
    part = split(cookie, "; ", &cookie);
    if (part != NULL){
        resheader->cookies[resheader->cookie_num]->cookie_value = (char *)malloc(sizeof(char)*strlen(part));
        memcpy(resheader->cookies[resheader->cookie_num]->cookie_value, part, strlen(part));
        // resheader->cookies[resheader->cookie_num]->cookie_value = part;
    }
    while (1){
        // part_num++;
        part = split(cookie, "; ", &cookie);
        if (part != NULL){
            tmp_part = (char *)malloc(sizeof(char)*(strlen(part)+1));
            memset(tmp_part, 0, strlen(part)+1);
            strcpy(tmp_part, part);
            part_key = split(tmp_part, "=", &tmp_part);
            if (part_key != NULL){
                part_value = split(tmp_part, "=", &tmp_part);
                if (strncmp(tolwr(part_key), "path", 4) == 0){
                    resheader->cookies[resheader->cookie_num]->path = (char *)malloc(sizeof(char)*(strlen(part)+1));
                    memset(resheader->cookies[resheader->cookie_num]->path, 0, strlen(part)+1);
                    memcpy(resheader->cookies[resheader->cookie_num]->path, part, strlen(part));
                    // resheader->cookies[resheader->cookie_num]->path = part_value;
                }
                else if (strncmp(tolwr(part_key), "domain", 6) == 0){
                    resheader->cookies[resheader->cookie_num]->domain = (char *)malloc(sizeof(char)*(strlen(part)+1));
                    memset(resheader->cookies[resheader->cookie_num]->domain, 0, strlen(part)+1);
                    memcpy(resheader->cookies[resheader->cookie_num]->domain, part, strlen(part));
                    // resheader->cookies[resheader->cookie_num]->domain = part_value;
                }
                else if (strncmp(tolwr(part_key), "max-age", 7) == 0){
                    resheader->cookies[resheader->cookie_num]->max_age = (char *)malloc(sizeof(char)*(strlen(part)+1));
                    memset(resheader->cookies[resheader->cookie_num]->max_age, 0, strlen(part)+1);
                    memcpy(resheader->cookies[resheader->cookie_num]->max_age, part, strlen(part));
                    // resheader->cookies[resheader->cookie_num]->max_age = part_value;
                }
            }
        }
        else
            break;
    }
    
    return 0;
}

int ParseResHeader(Response *resheader){
    char **header_list, *header_line;
    char **list_ptr = NULL;
    char *key = NULL, *value = NULL;
    char *tmp_header;
    char *line = NULL;
    char http_version[64] = {0}, status_code[64] = {0}, description[64] = {0};
    char line_flag[2] = {0};
    uint line_num = 0, line_len = 0;

    tmp_header = (char *)malloc(sizeof(char)*(strlen(resheader->headers)+1));
    memset(tmp_header, 0, strlen(resheader->headers)+1);
    strcpy(tmp_header, resheader->headers);

    for (ulong i = 0; i < strlen(tmp_header) - 1; i++){
        memcpy(line_flag, tmp_header+i, 2);
        if (strncmp(line_flag, "\r\n", 2) == 0) line_num++;
    }

    if (line_num == 0)
        return -1;

    header_list = (char **)malloc(sizeof(char *)*(line_num+1));
    list_ptr = header_list;

    line = split(tmp_header, "\n", &tmp_header);
    while (line != NULL){
        line_len = (*(line+strlen(line)-1)=='\r')?strlen(line)-1:strlen(line);
        *list_ptr = (char *)malloc(sizeof(char)*(line_len+1));
        memset(*list_ptr, 0, line_len+1);
        memcpy(*list_ptr, line, line_len);
        list_ptr++;
        line = split(tmp_header, "\n", &tmp_header);
    }

    sscanf(*header_list, "%s %s %s", http_version, status_code, description);
    resheader->status_code = (char *)malloc(sizeof(char)*(strlen(status_code)+1));
    memset(resheader->status_code, 0, strlen(status_code)+1);
    strcpy(resheader->status_code, status_code);

    resheader->content_encoding = NULL;
    resheader->content_language = NULL;
    resheader->content_type = NULL;
    resheader->content_encoding = NULL;
    for (uint i = 1; i <= line_num; i++){
        header_line = (char *)malloc(sizeof(char)*(strlen(header_list[i])+1));
        memset(header_line, 0, strlen(header_list[i]+1));
        strcpy(header_line, header_list[i]);
        key = split(header_line, ": ", &header_line);
        if (key == NULL){
            printf("Response header has a NULL key line!\n");
            return -1;
        }
        value = split(header_line, ": ", &header_line);
        if (strncmp(tolwr(key), "content-encoding", 16) == 0){
            GetValue(&(resheader->content_encoding), value);
        }
        else if (strncmp(tolwr(key), "content-language", 16) == 0){
            GetValue(&(resheader->content_language), value);
        }
        else if (strncmp(tolwr(key), "content-type", 12) == 0){
            GetValue(&(resheader->content_type), value);
        }
        else if (strncmp(tolwr(key), "transfer-encoding", 17) == 0){
            GetValue(&(resheader->transfer_encoding), value);
        }
        else if (strncmp(tolwr(key), "set-cookie", 10) == 0){
            GetCookie(resheader, value);
            resheader->cookie_num++;
        }
    }

    return 0;
}

int FindContent(char *recv_data,
                ulong recv_len,
                Response **res){
    char start_flag[4] = {0};
    char *tmp_content = NULL;
    ulong tmp_len = 0;
    ulong start = 0;
    ulong end = 7;
    Response *resheader;
    
    // printf("Response size: %lu\n", sizeof(Response));
    resheader = (Response *)malloc(sizeof(Response));
    for (int i = 0; i < 256; i++)
        resheader->cookies[i] = (set_cookie *)malloc(sizeof(set_cookie));

    if (recv_data == NULL){
        printf("Has no recived data.\n");
        return -1;
    }
    for (ulong i = 0; i < recv_len-3; i++){
        memcpy(start_flag, recv_data+i+1, 4);
        if (strncmp(start_flag, "\r\n\r\n", 4) == 0){
            resheader->headers = (char *)malloc(sizeof(char)*(i+2));
            memset(resheader->headers, 0, i+2);
            memcpy(resheader->headers, recv_data, i+1);
            if (resheader->headers == NULL){
                printf("Recived data has no headers!\n");
                return -1;
            }
            resheader->cookie_num = 0;
            if (ParseResHeader(resheader) < 0)
                return -1;
            i += 5;
            if (i >= recv_len - 3)
                break;
            /*
             * 判断内容是否压缩，以及压缩方式；
             * 1.如果内容压缩方式为gzip，那么去掉"\r\n\r\n"后面内容中
             *   包含的长度字段及字段后的"\r\n"字符串，同时去掉最后的
             *   结束标志字符串"\r\n0\r\n\r\n"。
             * 2.如果内容压缩方式为deflate，暂时不处理，当作无压缩方式。
             * 3.如果内容压缩方式为br，暂时不处理，当作无压缩方式。
             * 4.如果内容无压缩，则将"\r\n\r\n"后面的字符串全部作为内容保存。
             */
            if (resheader->content_encoding != NULL){
                if (strncmp(resheader->content_encoding, "gzip", 4) == 0){
                    while (*(recv_data+i) != 0x0d) i++;
                    i += 2;
                    if (i >= recv_len - 3) break;
                    tmp_len = recv_len - i - (start+end);
                    tmp_content = (char *)malloc(sizeof(char)*(tmp_len+1));
                    memset(tmp_content, 0, tmp_len+1);
                    memcpy(tmp_content, recv_data+i+start, tmp_len);
                }
                else if (strncmp(resheader->content_encoding, "deflate", 7) == 0){
                    // TODO
                    tmp_len = recv_len - i;
                    tmp_content = (char *)malloc(sizeof(char)*(tmp_len+1));
                    memset(tmp_content, 0, tmp_len+1);
                    memcpy(tmp_content, recv_data+i, tmp_len);
                }
                else if (strncmp(resheader->content_encoding, "br", 2) == 0){
                    // TODO
                    tmp_len = recv_len - i;
                    tmp_content = (char *)malloc(sizeof(char)*(tmp_len+1));
                    memset(tmp_content, 0, tmp_len+1);
                    memcpy(tmp_content, recv_data+i, tmp_len);
                }
            }
            else{
                tmp_len = recv_len - i;
                tmp_content = (char *)malloc(sizeof(char)*(tmp_len+1));
                memset(tmp_content, 0, tmp_len+1);
                memcpy(tmp_content, recv_data+i, tmp_len);
            }
            break;
        }
    }

    /*
     * 判断内容是否是分块传输的，如果内容为分块传输，则去掉内容中的分块标志。
     */
    if (tmp_content == NULL) return -1;
    if (resheader->transfer_encoding != NULL){
        DeChunked(tmp_content, tmp_len, &(resheader->content), &(resheader->content_len));
    }
    else{
        resheader->content = (char *)malloc(sizeof(char)*(tmp_len+1));
        memset(resheader->content, 0, tmp_len+1);
        memcpy(resheader->content, tmp_content, tmp_len);
        resheader->content_len = tmp_len;
    }
    *res = resheader;

    return 0;
}

int UnCompressGzip(char *src,
                   ulong src_len,
                   uchar **dest_str,
                   ulong *dest_len){
    uchar *src_str;
    int uncompressResult;
    z_stream d_stream;
    ulong tlen = 0;

    src_str = (uchar *)malloc(sizeof(uchar)*(src_len+1));
    memset(src_str, 0, src_len+1);
    memcpy(src_str, (uchar *)src, src_len);
    // printf("src_len: %lu\nstrlen(src): %lu\n", src_len, strlen(src));

    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 8; j++)
            tlen += ((src_str[src_len-4+i] >> j) & 0x01) * (1 << (j+i*8));
    /*
     * malloc 出错...
     */
    *dest_str = (uchar *)malloc(sizeof(uchar)*tlen);
    *dest_len = tlen;

    if ((*src_str != 0x1f) || (*(src_str+1) != 0x8b)){
        printf("\nuncompressGzip non Gzip\n");
        return -1;
    }

    d_stream.zalloc = Z_NULL;
    d_stream.zfree = Z_NULL;
    d_stream.opaque = Z_NULL;
    d_stream.next_in = Z_NULL;
    d_stream.avail_in = 0;
    uncompressResult = inflateInit2(&d_stream, 47);
    if (uncompressResult != Z_OK){
        printf("\ninflateInit2 error: %d\n", uncompressResult);
        return uncompressResult;
    }

    d_stream.next_in = src_str;
    d_stream.avail_in = src_len;
    d_stream.next_out = *dest_str;
    d_stream.avail_out = *dest_len;
    uncompressResult = inflate(&d_stream, Z_NO_FLUSH);

    switch(uncompressResult){
        case Z_OK:
            break;
        case Z_STREAM_END:
            break;
        case Z_BUF_ERROR:
            printf("Z_BUF_ERROR\n");
            return -1;
        case Z_NEED_DICT:
            printf("Z_NEED_DICT\n");
            return -1;
        case Z_DATA_ERROR:
            printf("Z_DATA_ERROR\n");
            return -1;
        case Z_STREAM_ERROR:
            printf("Z_STREAM_ERROR\n");
            return -1;
        case Z_MEM_ERROR:
            printf("Z_MEM_ERROR\n");
            (void)inflateEnd(&d_stream);
            return -1;
    }

    inflateEnd(&d_stream);

    return 0;
}

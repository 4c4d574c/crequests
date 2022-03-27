#ifndef __PARSER_H__
#define __PARSER_H__

#include "utils.h"

typedef struct {
    char *cookie_value;
    char *path;
    char *domain;
    char *max_age;
}set_cookie;

typedef struct {
    ulong content_len;
    int cookie_num;
    set_cookie *cookies[256];
    char *headers;
    char *status_code;
    char *content_encoding;
    char *content_language;
    char *content_type;
    char *transfer_encoding;
    char *content;
}Response;

int DeChunked(char *, ulong, char **, ulong *);
int ParseResHeader(Response *);
int FindContent(char *, ulong, Response **);
int UnCompressGzip(char *, ulong, uchar **, ulong *);
#endif

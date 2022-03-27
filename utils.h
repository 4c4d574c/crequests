#ifndef __UTILS_H__
#define __UTILS_H__

#define BUFFERSIZE 1024
#define ulong unsigned long
#define uchar unsigned char
#define uint unsigned int
#define headerparam {"", \
                     "Accept: ",\
                     "Accept-Charset: ",\
                     "Accept-Encoding: ",\
                     "Accept-Language: ",\
                     "Accept-Ranges: ",\
                     "Authorization: ",\
                     "Cache-Control: ",\
                     "Connection: ",\
                     "Cookie: ",\
                     "Content-Length: ",\
                     "Content-Type: ",\
                     "Date: ",\
                     "Expect: ",\
                     "From: ",\
                     "Host: ",\
                     "If-Match: ",\
                     "If-Modified-Since: ",\
                     "If-None-Match: ",\
                     "If-Range: ",\
                     "If-Unmodified-Since: ",\
                     "Max-Forwards: ",\
                     "Pragma: ",\
                     "Proxy-Authorization: ",\
                     "Range: ",\
                     "Referer: ",\
                     "TE: ",\
                     "Upgrade: ",\
                     "User-Agent: ",\
                     "Via: ",\
                     "Warning: "};

typedef struct {
    char *method_url_type;
    char *accept;
    char *accept_charset;
    char *accept_encoding;
    char *accept_language;
    char *accept_ranges;
    char *authorization;
    char *cache_control;
    char *connection;
    char *cookie;
    char *content_length;
    char *content_type;
    char *date;
    char *expect;
    char *from;
    char *host;
    char *if_match;
    char *if_modified_since;
    char *if_none_match;
    char *if_range;
    char *if_unmodified_since;
    char *max_forwards;
    char *pragma;
    char *proxy_authorization;
    char *range;
    char *referer;
    char *te;
    char *upgrade;
    char *user_agent;
    char *via;
    char *warning;
}Reqheader;

typedef union {
    Reqheader headers;
    char *header_params[31];
}ReqHeader;

typedef struct LinkNode{
    int len;
    char data[BUFFERSIZE];
    struct LinkNode *next;
}LNode;

int PushData(LNode *, char *, int);
char *tolwr(char *);
char *int2str(ulong);
char *split(char *, const char *, char **);
char *join(char *[], char *, int);
int SplitLink(char *, char **, char **, char **);
int CreateRequestHeader(ReqHeader *, char *, char *, char *);

#endif

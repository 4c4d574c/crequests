#ifndef __HTTPNET_H__
#define __HTTPNET_H__

#include "utils.h"

#define IPV4 4
#define IPV6 6
#define HTTPPORT 80
#define HTTPSPORT 443

int GetAddrByName(char *, char **);
int CreateSocketConnect(char *, int, int);
int SendAll(int, char *);
int RecvAll2(int, char **, ulong *);
int RecvAll(int, char **, ulong *);
int SendRequestHeader(int, ReqHeader, char *);

#endif

#ifndef __REQUESTS_H__
#define __REQUESTS_H__

#include "utils.h"
#include "parser.h"

int HttpGet(char *, ReqHeader, Response **);
int HttpPost(char *, ReqHeader, Response **, char *[], int);

#endif

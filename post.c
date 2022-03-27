#include <stdio.h>
#include <stdlib.h>
#include "requests.h"

int main(){
    ReqHeader header;
    Response *res;
    char *post_data[21];
    FILE *fl;

    for (int i = 0; i < (int)(sizeof(header) / sizeof(char *)); i++)
        header.header_params[i] = NULL;

    post_data[0] = "pageSize=10";
    post_data[1] = "pageNum=1";
    post_data[2] = "iEType=1";
    post_data[3] = "currencyType=rmb";
    post_data[4] = "year=2021";
    post_data[5] = "startMonth=1";
    post_data[6] = "endMonth=1";
    post_data[7] = "monthFlag=1";
    post_data[8] = "unitFlag=true";
    post_data[9] = "codeLength=8";
    post_data[10] = "outerField1=CODE_TS";
    post_data[11] = "outerField2=ORIGIN_COUNTRY";
    post_data[12] = "outerField3=TRADE_MODE";
    post_data[13] = "outerField4=TRADE_CO_PORT";
    post_data[14] = "outerValue1=";
    post_data[15] = "outerValue2=";
    post_data[16] = "outerValue3=";
    post_data[17] = "outerValue4=";
    post_data[18] = "orderType=CODE ASC DEFAULT";
    post_data[19] = "selectTableState=1";
    post_data[20] = "currentStartTime=201910";

    // header.headers.user_agent = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.77 Safari/537.36 Edg/91.0.864.41";

    // HttpPost("http://43.248.49.97/queryData/getQueryDataListByWhere?DwhQEvbR=5d_aEGfXk0a0KWKI9BR5RUutOFLtFCmrf2W32NnfNgTXAOVmjvUM3gVB3WBe4Cl4qil7PJVTlw4shtjypiCj_tz6iAF8z0_A0tc9CLeu2ps7QfT8LX9YQ5x9PDbvLmCxWTt2FblOxgfgIrtELbFRRfV97IBfcVGy5AQ76tJ1ORvVku7g4qCLepELSLG0kYAmahPbFHcjrOjmD_RuMCBWjjoU4swlq4wLC0Mn96HQpPlyPe0bKWUSCyQIAORX8VM_tUs88YatdOiM4df5s3B8aCcgtVKvu8k47JJwsbBkEpoZ", header, &res, post_data, 21);
    HttpPost("http://httpbin.org/post", header, &res, post_data, 21);
    if (res == NULL)
        printf("failed!\n");
    else{
        printf("%s\n\n", res->headers);
        printf("content_len: %lu\n", res->content_len);
        printf("status_code: %s\n", res->status_code);
        printf("content_encoding: %s\n", res->content_encoding);
        printf("content_language: %s\n", res->content_language);
        printf("content_type: %s\n", res->content_type);
        printf("transfer_encoding: %s\n\n", res->transfer_encoding);
        printf("cookies:\n");
        for (int i = 0; i < res->cookie_num; i++){
            printf("cookie_value: %s\n", res->cookies[i]->cookie_value);
            printf("path: %s\n", res->cookies[i]->path);
            printf("domain: %s\n", res->cookies[i]->domain);
            printf("max-age: %s\n\n", res->cookies[i]->max_age);
        }
        fl = fopen("response.html", "wb+");
        fwrite(res->content, sizeof(char), res->content_len, fl);
        fclose(fl);
    }

    return 0;
}

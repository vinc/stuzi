#include <curl/curl.h>

#include "curl.h"

/*
 * Download URL to file with libcurl.
 */
void curl_download(char* url, FILE* fp)
{
    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        if (CURLE_OK != res) {
            // TODO
        }
    }

    curl_global_cleanup();
}

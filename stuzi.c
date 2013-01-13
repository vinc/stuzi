#include <assert.h>
#include <errno.h>
#include <getopt.h>
#include <limits.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <arpa/inet.h>
#include <curl/curl.h>

#define RECORDS_MAX 1 << 17
#define RIR_MAX     5

/* Regional Internet registries */
const char* rirs[RIR_MAX] = {
    "afrinic",
    "apnic",
    "arin",
    "lacnic",
    "ripencc"
};

/* Simplified RIR Statistics Exchange Format */
typedef struct record
{
    unsigned int start; /* First IP address of range */
    unsigned int stop;  /* Last IP address of range */
    char country[3];    /* ISO 3166 2-letter country code */
} record;

/* Records table for lookups */
record records[RECORDS_MAX];

/*
 * Find ISO 3166 2-letter country code
 * associated with IP Address.
 */
const char* lookup(unsigned int addr)
{
    unsigned int i;

    for (i = 0; i < RECORDS_MAX; ++i) {
        record* r = &records[i];
        if (r->start < addr && addr < r->stop) {
            return r->country;
        }
    }
    return "NOT FOUND";
}

/*
 * Load file cache into records table.
 */
unsigned int load(const char* cache)
{
    record* r;
    unsigned int n = 0;
    FILE* fp = fopen(cache, "r");
    const char* format = "%10u;%10u;%2s\n";

    if (fp == NULL) {
        fprintf(stderr, "stuzi: ");
        perror(cache);
        exit(1);
    }

    do {
        r = &records[n++];
    }
    while (fscanf(fp, format, &r->start, &r->stop, r->country) != EOF);

    fclose(fp);
    return n;
}

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

/*
 * Synchronize file cache with current FTP version
 * of RIR statistics files.
 */
unsigned int sync(const char* cache)
{
    FILE* fp;
    unsigned int n = 0;
    unsigned int i;

    for (i = 0; i < RIR_MAX; ++i) {
        const char* rir = rirs[i];
        char line[128] = "";
        char filename[32];
        char url[128];
        const char* domain = "ftp.ripe.net";

        sprintf(filename, "delegated-%s-latest", rir);
        /* sprintf(domain, "ftp.%s.net", rir); */
        sprintf(url, "ftp://%s/pub/stats/%s/%s", domain, rir, filename);
        printf("Fetching '%s' ...\n", url);

        fp = tmpfile();

        curl_download(url, fp);

        rewind(fp);

        while (fgets(line, 128, fp)) {
            unsigned int value = 0;
            char registry[8] = "";
            char cc[8] = "";
            char type[8] = "";
            char start[64] = "";
            const char* format = "%8[^|]|%8[^|]|%8[^|]|%64[^|]|%10u|%*s\n";

            if (*line == '#') {
                continue; /* Skip comment */
            }

            sscanf(line, format, registry, cc, type, start, &value);

            if (strncmp(registry, rir, sizeof(rir) - 1)) {
                continue; /* Skip header */
            } else if (!strncmp(cc, "*", 1)) {
                continue; /* Skip summary */
            }
            
            if (!strncmp(type, "ipv4", 4)) {
                record r;                
                r.start = htonl(inet_addr(start));
                r.stop = r.start + value;
                strncpy(r.country, cc, sizeof(r.country) - 1);
                r.country[sizeof(r.country) - 1] = '\0';
                records[n++] = r;
            }
        }
        fclose(fp);
    }
    fp = fopen(cache, "w");
    if (fp == NULL) {
        fprintf(stderr, "stuzi: ");
        perror(cache);
        exit(1);
    }
    for (i = 0; i < n; ++i) {
        record* r = &records[i];
        fprintf(fp, "%u;%u;%s\n", r->start, r->stop, r->country);
    }
    fclose(fp);

    return n;
}

int main(int argc, char* argv[])
{
    bool opt_sync = false;
    bool opt_verbose = false;
    int i;
    int opt;
    unsigned int n;
    char* cache = "ipv4.records";

    while ((opt = getopt(argc, argv, "c:hsv")) != EOF) {
        switch (opt) {
        case 'c':
            cache = optarg;
            break;
        case 'h':
            // TODO
            break;
        case 's':
            opt_sync = true;
            break;
        case 'v':
            opt_verbose = true;
            break;
        case '?':
        default:
            printf("Try 'stuzi -h' for more information.");
            return 1;
        }
    }
    
    if (opt_sync) {
        sync(cache);
    }

    n = load(cache);
    if (opt_verbose) {
        printf("%d records loaded\n", n);
    }

    for (i = optind; i < argc; ++i) {
        const unsigned int addr = htonl(inet_addr(argv[i]));
        const char* country = lookup(addr);

        if (opt_verbose) {
            printf("%s: ", argv[i]);
        }
        printf("%s\n", country);
    }
    return 0;
}

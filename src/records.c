#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <arpa/inet.h>

#include "curl.h"
#include "records.h"

/* Regional Internet registries */
const char* rirs[RIR_MAX] = {
    "afrinic",
    "apnic",
    "arin",
    "lacnic",
    "ripencc"
};

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
                r.start = htonl(inet_addr(start)); /* Dot notation to int */
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

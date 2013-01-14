#ifndef RECORDS_H
#define RECORDS_H

#define RECORDS_MAX 1 << 17
#define RIR_MAX     5

/* Regional Internet registries */
extern const char* rirs[RIR_MAX];

/* Simplified RIR Statistics Exchange Format */
typedef struct record
{
    unsigned int start; /* First IP address of range */
    unsigned int stop;  /* Last IP address of range */
    char country[3];    /* ISO 3166 2-letter country code */
} record;

/* Records table for lookups */
extern record records[RECORDS_MAX];

/*
 * Find ISO 3166 2-letter country code
 * associated with IP Address.
 */
const char* lookup(unsigned int addr);

/*
 * Load file cache into records table.
 */
unsigned int load(const char* cache);

/*
 * Synchronize file cache with current FTP version
 * of RIR statistics files.
 */
unsigned int sync(const char* cache);

#endif /* !RECORDS_H */

/* Copyright (C) 2012 Vincent Ollivier
 *
 * Stuzi is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Stuzi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef RECORDS_H
#define RECORDS_H

#define RECORDS_MAX 1 << 17
#define RIR_MAX     5

/* Regional Internet registries */
extern const char* rirs[RIR_MAX];

/* Simplified RIR Statistics Exchange Format */
typedef struct record {
    uint32_t start; /* First IP address of range */
    uint32_t stop;  /* Last IP address of range */
    char country[3];    /* ISO 3166 2-letter country code */
} record;

/* Records table for lookups */
extern record records[RECORDS_MAX];

/*
 * Find ISO 3166 2-letter country code
 * associated with IP Address.
 */
const char* lookup(const uint32_t addr);

/*
 * Load file cache into records table.
 */
const int load(const char* cache);

/*
 * Synchronize file cache with current FTP version
 * of RIR statistics files.
 */
const int sync(const char* cache);

#endif /* !RECORDS_H */

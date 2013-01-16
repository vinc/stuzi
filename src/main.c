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

#include <getopt.h>
#include <stdio.h>
#include <stdbool.h>

#include <arpa/inet.h>

#include "records.h"

int main(int argc, char* argv[])
{
    bool opt_sync = false;
    bool opt_verbose = false;
    int i;
    int opt;
    int n;
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

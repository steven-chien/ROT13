/**
* @file ROT13.c
* @brief Implementation of Rotate by 13 places (ROT-13).
* @author Star Poon <oneonestar@gmail.com>
* @version 1.0
* @copyright 2015
*
* @section LICENSE
* Copyright (C) 2015 Star Poon
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <argp.h>

/* Print --version. */
const char *argp_program_version =
"ROT13 1.0.0\n\
Copyright (C) 2015 Star Poon\n\
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>.\n\
This is free software: you are free to change and redistribute it.\n\
There is NO WARRANTY, to the extent permitted by law.\n\
\n\
Written by Star Poon.";

/* Print --help. */
const char *argp_program_bug_address =
  "Star Poon <oneonestar@gmail.com>";

/* A descriptive string about this program. */
static char doc[] =
  "ROT13 -- Rotate by 13 places encryption";

static struct argp_option options[] = {
        {"shift",   's',    "AMOUNT",      0,  "Shift by this AMOUNT. (default 13)" },
        {"reverse", 'r',    0,      0,  "Reverse shift (right shift)." },
        {"reverse", 'o',    "f",      0,  "Reverse shift (right shift)." },
//        {"direct",  'o',    "FILE", 0,  "Output to FILE instead of standard output" },
        { 0 }
};

/* Used by main to communicate with parse_opt. */
struct arguments
{
    long int shift;
    int reverse;
    char *output_file;
};

//static long parseLong(const char *str)
//{
//    errno = 0;
//    char *temp;
//    long val = strtol(str, &temp, 0);
//
//    if (temp == str || *temp != '\0' ||
//        ((val == LONG_MIN || val == LONG_MAX) && errno == ERANGE))
//        fprintf(stderr, "Could not convert '%s' to long and leftover string is: '%s'\n",
//                str, temp);
//    return val;
//}

/* Parse a single option. */
static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
    /* Get the input argument from argp_parse, which we
       know is a pointer to our arguments structure. */
    struct arguments *arguments = state->input;
    switch (key)
    {
        case 's':
            arguments->shift = strtol(arg, 0, 10);
            return EINVAL;
            break;
        case 'r':
            arguments->reverse = 1;
            break;
        case 'o':
            arguments->output_file = arg;
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

static struct argp argp = { options, parse_opt, 0, doc };

char c;
int main (int argc, char **argv)
{
    struct arguments arguments = {
            .shift = 13
    };
    error_t ret = argp_parse(&argp, argc, argv, 0, 0, &arguments);
    if(ret) {
        fprintf(stderr, "%s: %s\n", argv[0], strerror(ret));
        fprintf(stderr, "Try `ROT13 --help' or `ROT13 --usage' for more information.\n");
        return errno;
    }

    printf("Shift=%ld, Reverse=%d\n", arguments.shift, arguments.reverse);
    do {
        // Read a character from stdin
        c = fgetc(stdin);
        // End of file? Nothing left to read...
        if (feof(stdin)) {
            // Exit loop
            break;
        }

        // Is the character lower case?
        if ((c >= 'a') && (c <= 'z')) {
            // Yes, let's make it upper case
            c -= 32;
        }

        // Encrypt
        if (c >='A' && c<='Z') {
            // Shift
            c -= 3;;

            // Range check
            if (c > 'Z') {
                c -= 26;
            }
            else if (c < 'A') {
                c += 26;
            }
        }
        // Print the character to stdout (default)
        printf("%c", c);
    } while (1);
    // Exit cleanly
    return 0;

}


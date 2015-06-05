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

static char args_doc[] = "[FILE...]";

static struct argp_option options[] = {
        {"shift",   's',    "AMOUNT",      0,  "Shift by this AMOUNT (default 13)" },
        {"reverse", 'r',    0,      0,  "Reverse shift (right shift)" },
        {"verbose", 'v',    0,      0,  "Verbose Mode" },
        { 0 }
};

/* Used by main to communicate with parse_opt. */
struct arguments
{
    long int shift;
    int reverse, verbose;
    char **strings;               /* [FILE…] */
};

/**
 * @brief Error handling for strtol().
 */
int parseLong(const char *str, long *ret)
{
    errno = 0;
    char *temp;
    long val = strtol(str, &temp, 0);

    if (temp == str || *temp != '\0' ||
        ((val == LONG_MIN || val == LONG_MAX) && errno == ERANGE))
        return 1;   //Failed to convert to int
    *ret = val;
    return 0;
}

/**
 * @brief Parse a single option.
 */
static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
    /* Get the input argument from argp_parse, which we
       know is a pointer to our arguments structure. */
    long ret, arg_int;
    struct arguments *arguments = state->input;
    switch (key)
    {
        case 's':
            ret = parseLong(arg, &arg_int);
            if(ret) {
                // Print error and exit()
                argp_error(state, "%s", strerror(EINVAL));
            }
            arguments->shift = arg_int;
            break;
        case 'r':
            arguments->reverse = 1;
            break;
        case 'v':
            arguments->verbose = 1;
            break;
        case ARGP_KEY_ARG:
            arguments->strings = &state->argv[state->next-1];
            state->next = state->argc;
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

static struct argp argp = { options, parse_opt, args_doc, doc };
static struct arguments arguments = {
        .shift = 13
};

int convert_file(FILE *inFile, FILE *outFile)
{
    do {
        // Read a character from stdin
        int c = fgetc(inFile);
        // End of file? Nothing left to read...
        if (feof(inFile)) {
            // Exit loop
            break;
        }

        // Encrypt
        if (c >='A' && c<='Z') {
            // Shift
            c = arguments.reverse? c+arguments.shift: c-arguments.shift;

            // Range check
            if (c > 'Z') {
                c -= 26;
            }
            else if (c < 'A') {
                c += 26;
            }
        }

        if (c >='a' && c<='z') {
            // Shift
            c = arguments.reverse? c+arguments.shift: c-arguments.shift;

            // Range check
            if (c > 'z') {
                c -= 26;
            }
            else if (c < 'a') {
                c += 26;
            }
        }
        // Print the character to stdout (default)
        fprintf(outFile, "%c", c);
    } while (1);
    return 0;
}

char c;
int main (int argc, char **argv)
{
    error_t ret = argp_parse(&argp, argc, argv, 0, 0, &arguments);
    if(ret) {
        fprintf(stderr, "%s: Unknown error during parsing argument: %s\n", argv[0], strerror(ret));
        return EXIT_FAILURE;
    }
    if (arguments.verbose) {
        printf("Shift=%ld, Reverse=%d\n", arguments.shift, arguments.reverse);
    }

    if (arguments.strings) {
        // Provided File Name -> Convert files
        for (int j = 0; arguments.strings[j]; j++) {

            FILE *inFile, *outFile;
            inFile = fopen (arguments.strings[j],"r");
            if(!inFile) {
                fprintf(stderr, "%s: Failed to open file '%s': %s\n", argv[0], arguments.strings[j], strerror(errno));
                return EXIT_FAILURE;
            }
            char temp_file_name[] = "fileXXXXXX";

            int temp_fd = mkstemp(temp_file_name);
            outFile = fdopen(temp_fd, "w");
            if(!outFile) {
                fprintf(stderr, "%s: Failed to create temp file: %s\n", argv[0], strerror(errno));
                return EXIT_FAILURE;
            }
            convert_file(inFile, outFile);

            fclose(inFile);
            fclose(outFile);

            // "filename"+".out" as the output
            if (strlen(arguments.strings[j])+4>=FILENAME_MAX) {
                // if "filename"+".out" is too long, just clean up and terminate the program.
                remove(temp_file_name);
                return EXIT_FAILURE;
            }

            // According to GCC standard, we shouldn't use FILENAME_MAX as the size of an array in which to store a
            // file name.  Use dynamic allocation instead.
            // I doubt that but we just follow it here.
            char *final_output = malloc(strlen(arguments.strings[j])+4);
            snprintf(final_output, FILENAME_MAX, "%s.out", arguments.strings[j]);
            remove(final_output);
            rename(temp_file_name, final_output);
            if (arguments.verbose) {
                printf("Converting %s\n", arguments.strings[j]);
                printf("Output as %s\n", final_output);
            }
            free(final_output);
        }
    } else {
        // No file name provided -> Convert stdin
        convert_file(stdin, stdout);
    }

    // Exit cleanly
    return EXIT_SUCCESS;

}


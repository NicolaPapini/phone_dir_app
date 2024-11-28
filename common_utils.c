#include "common_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

void strip_whitespaces(char *str);
void compact_internal_whitespaces(char *str);


void check(int value, char *msg) {
    if (value == SOCKET_ERROR) {
        perror(msg);
        exit(EXIT_FAILURE);
    }
}

void sanitize_name_or_surname(char *str) {
    strip_whitespaces(str);
    compact_internal_whitespaces(str);
}

void strip_whitespaces(char *str) {
    char *start = str;
    char *end;

    while (isspace((unsigned char)*start)) {
        start++;
    }

    if (*start == '\0') {
        str[0] = '\0';
        return;
    }

    end = start + strlen(start) - 1;

    while (end > start && isspace((unsigned char)*end)) {
        end--;
    }

    *(end + 1) = '\0';

    // Move the stripped string back to the original buffer
    if (start != str) {
        memmove(str, start, end - start + 2); // +2 to include null terminator
    }
}

void compact_internal_whitespaces(char *str) {
    char *src = str;
    char *dst = str;
    bool in_whitespace = false;

    while (*src != '\0') {
        if (isspace((unsigned char)*src)) {
            if (!in_whitespace) {
                *dst++ = ' ';
                in_whitespace = true;
            }
        } else {
            *dst++ = *src;
            in_whitespace = false;
        }
        src++;
    }

    *dst = '\0';
}


bool is_alpha(char *str) {
    while (*str != '\0') {
        if (!isalpha((unsigned char)*str) && !isspace((unsigned char)*str)) {
            return false;
        }
        str++;
    }
    return true;
}

bool is_numeric(char *number) {
    for (int i = 0; number[i] != '\0'; i++) {
        if (!isdigit(number[i])) {
            return false;
        }
    }
    return true;
}
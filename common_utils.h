#ifndef COMMON_UTILS_H
#define COMMON_UTILS_H
#include <stdbool.h>

#define SOCKET_ERROR -1

void check(int value, char *msg);
void sanitize_name_or_surname(char *str);
bool is_numeric(char *number);
bool is_alpha(char *str);
#endif //COMMON_UTILS_H

#ifndef SERIALIZATION_H
#define SERIALIZATION_H
#include <stdbool.h>
#include "data_structures/phone_directory.h"

#define MAX_LINE 128
#define PHONE_DIRECTORY_FILE "phone_directory.txt"

bool serialize(PhoneDirectory *phone_dir);
void deserialize(PhoneDirectory *phone_dir);

#endif //SERIALIZATION_H

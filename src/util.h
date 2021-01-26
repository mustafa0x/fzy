#ifndef _UTIL_H
#define _UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

void UTF8To32(const unsigned char *str, uint32_t **out);

void UTF32To8(const uint32_t *str, unsigned char **out);

size_t strsize_utf8(const unsigned char *str);

size_t strlen_utf8(const unsigned char *str);

size_t strlen_utf32(const uint32_t *str);

size_t strsize_utf32(const uint32_t *str);

#endif // _UTIL_H

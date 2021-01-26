#ifndef _ARA_SKIM_H
#define _ARA_SKIM_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "../util.h"

typedef enum HARAKA
{
	FATHA,
	DAMMA,
	KASRA,
	UNKNOWN
} HARAKA;

///Inserts utf32 character into output buffer
void InsertChar(uint32_t c, uint32_t **out, size_t *outLength, size_t strLength);

///Removes harakat and everything other than arabic letters, even hamza on alef is removed.
///Any type of alef is converted to a raw alef.
size_t Arabic_SkimString(const uint32_t *str, uint32_t **out);

///Checks if UTF32 char is an arabic letter
bool IsArabicLetter(uint32_t c);

bool IsNumberChar(uint32_t c);

///Finds length (In letters) of currently indexed word in buffer
///First letter in word position is stored in firstLetterPos pointer (Can be NULL).
size_t FindWordLength(uint32_t *str, size_t length, size_t i, size_t *firstLetterPos);

///Return previous character
uint32_t GetPrevChar(uint32_t *str, size_t length, size_t i);

///Return next character
uint32_t GetNextChar(uint32_t *str, size_t length, size_t i);

///Return previous haraka in the same word (0 otherwise)
uint32_t GetPrevHaraka(uint32_t *str, size_t length, size_t i);

///Return next haraka in the same word (0 otherwise)
uint32_t GetNextHaraka(uint32_t *str, size_t length, size_t i);

///Returns index of prev REAL letter, and also output the letter itself
size_t GetPrevLetter(uint32_t *str, size_t length, size_t i, uint32_t *out);

///Returns index of next REAL letter, and also output the letter itself
size_t GetNextLetter(uint32_t *str, size_t length, size_t i, uint32_t *out);

///Checks if letter is first in word
unsigned char IsFirstLetter(uint32_t *str, size_t length, size_t i);

///Checks if letter is midword
unsigned char IsMidwordLetter(uint32_t *str, size_t length, size_t i);

///Checks if letter is last in word
unsigned char IsLastLetter(uint32_t *str, size_t length, size_t i);

//Checks if harf is a harf Illah or not
unsigned char IsHarfIllah(uint32_t c);

///Letter specific checks
unsigned char IsYeh(uint32_t c);

unsigned char IsNoon(uint32_t c);

unsigned char IsHeh(uint32_t c);

unsigned char IsAlef(uint32_t c);

#endif // _ARA_SKIM_H

#include "skim.h"

///Inserts utf32 character into output buffer
void InsertChar(uint32_t c, uint32_t **out, size_t *outLength, size_t strLength)
{
	if((*outLength)+1 >= strLength)
		*out = (uint32_t *)realloc(*out, sizeof(uint32_t) * ((*outLength)+1+1));
		
	(*out)[(*outLength)++] = c;
}

///Removes harakat and everything other than arabic letters, even hamza on alef is removed.
///Any type of alef is converted to a raw alef.
size_t Arabic_SkimString(const uint32_t *str, uint32_t **out)
{	
	size_t strLength = strlen_utf32(str);
	
	if(!strLength)
	{
		*out = (uint32_t *)malloc(sizeof(uint32_t));
		(*out)[0] = 0;
		
		return 0;
	}
	
	size_t strLengthNew = 0;
	
	*out = (uint32_t *)malloc(sizeof(uint32_t) * (strLength+1));
	
	unsigned char exactAlef = 0;
	
	//Exact letters
	uint32_t alefMadda = 0xD8A2;
	uint32_t alefHamzaAbove = 0xD8A3;
	uint32_t alefHamzaBelow = 0xD8A5;
	
	if(!exactAlef)
	{
		alefMadda = 0xD8A7;
		alefHamzaAbove = 0xD8A7;
		alefHamzaBelow = 0xD8A7;
	}

	//Skim process
	uint32_t prevStored = 0;
	size_t i = 0;
	for(; i < strLength; i++)
	{
		//Get previously stored character in output buffer
		if(strLengthNew)
			prevStored = (*out)[strLengthNew-1];
		
		//All alef are turned into a normal alef (No hamza or madd)
		if(IsAlef(str[i]))
		{
			InsertChar(0xD8A7, out, &strLengthNew, strLength);
				
			continue;
		}

		//Waw hamzeh is changed into a normal standalone hamza
		if(str[i] == 0xD8A4)
		{
			InsertChar(0xD8A1, out, &strLengthNew, strLength);
				
			continue;
		}
		
		//Noon types
		if(str[i] >= 0xDAB9 && str[i] <= 0xD9BD)
		{
			InsertChar(0xD986, out, &strLengthNew, strLength);
				
			continue;
		}
		
		//Yeh types
		if(str[i] == 0xD98A || str[i] == 0xD8A0 /*|| str[i] == 0xD8A6*/ || (str[i] >= 0xD8BD && str[i] <= 0xD8BF) || (str[i] >= 0xDB8C && str[i] <= 0xDB8E) || str[i] == 0xDB90 || str[i] == 0xDB91)
		{
			InsertChar(0xD98A, out, &strLengthNew, strLength);
			
			continue;
		}
		
		//Qaf types
		if(str[i] == 0xD9AF)
		{	
			InsertChar(0xD982, out, &strLengthNew, strLength);
				
			continue;
		}
		
		//Beh types
		if(str[i] == 0xD9AE)
		{
			InsertChar(0xD8A8, out, &strLengthNew, strLength);
				
			continue;
		}
		
		//Heh types
		if(str[i] == 0xDABE || (str[i] >= 0xDB80 && str[i] <= 0xDB82))
		{
			InsertChar(0xD987, out, &strLengthNew, strLength);
				
			continue;
		}

		//Ta2 marboota
		if(str[i] == 0xD8A9)
		{
			InsertChar(0xD987, out, &strLengthNew, strLength);
				
			continue;
		}
		
		//Other arabic alphabets (No tatweels)
		if(str[i] == 0x00 || str[i] == 0x20 || str[i] == '\n' || (str[i] >= 0xD8A0 && str[i] <= 0xD98A && str[i] != 0xD980))
		{
			//We do not want double spaces, we also don't want a stray space at the beginning of a line or end of it
			if(str[i] == 0x20 && (prevStored == 0x20 || prevStored == '\n' || !strLengthNew))
				continue;
			
			//This is to remove a stray space at end of line
			if(str[i] == '\n' && prevStored == 0x20)
				strLengthNew--;

			//Insert the character
			InsertChar(str[i], out, &strLengthNew, strLength);
				
			continue;
		}
		
		//Or Arabic numbers (Not hindi)
		if(str[i] >= 0x30 && str[i] <= 0x39)
		{
			InsertChar(str[i], out, &strLengthNew, strLength);
				
			continue;
		}

		//Other letters not looked in this function (Excluding harakat)
		if(!(str[i] >= 0xD890 && str[i] <= 0xD89A) || (str[i] >= 0xD98B && str[i] <= 0xD99F))
			InsertChar(str[i], out, &strLengthNew, strLength);
	}

	//Null terminator
	(*out)[strLengthNew] = 0;
	
	//No need to keep allocating that extra memory
	if(strLength > strLengthNew)
		*out = (uint32_t *)realloc(*out, sizeof(uint32_t) * (strLengthNew+1));
	
	return strLengthNew;
}

///Checks if UTF32 char is an arabic letter
bool IsArabicLetter(uint32_t c)
{
	if((c >= 0xD8A1 && c <= 0xD8BA) || (c >= 0xD981 && c <= 0xD98A))
		return true;
	
	return false;
}

bool IsNumberChar(uint32_t c)
{
	if(c >= 0x30 && c <= 0x39)
		return true;
	
	return false;
}

///Finds length (In letters) of currently indexed word in buffer
///First letter in word position is stored in firstLetterPos pointer (Can be NULL).
size_t FindWordLength(uint32_t *str, size_t length, size_t i, size_t *firstLetterPos)
{
	size_t len = 0;
	
	//Count backwards
	for(size_t j = i; j >= 0; j--)
	{
		if(str[j] == 0x20 || str[j] == '\n' || str[j] == 0x00)
		{
			if(firstLetterPos != NULL)
				*firstLetterPos = j+1;
			
			break;
		}
		
		len++;
	}
	
	//Count forwards
	for(size_t j = i; j < length; j++)
	{
		if(str[j] == 0x20 || str[j] == '\n' || str[j] == 0x00)
			break;
		
		len++;
	}
	
	if(len)
		return len-1; //Because we counted the same char twice (To prevent a space bug)
	
	return 0;
}

///Return previous character
uint32_t GetPrevChar(uint32_t *str, size_t length, size_t i)
{
	if(!i)
		return 0;
		
	i--;
	
	//We are already at first char, return 0 (No prev letter in word)
	if(str[i] == 0x00 || str[i] == 0x20 || str[i] == '\n')
		return 0;

	return str[i];
}

///Return next character
uint32_t GetNextChar(uint32_t *str, size_t length, size_t i)
{
	if(i == length-1)
		return 0;
		
	i++;
	
	//We are already at first char, return 0 (No prev letter in word)
	if(i < length)
	{
		if(str[i] == 0x00 || str[i] == 0x20 || str[i] == '\n')
			return 0;

		return str[i];
	}
	else
		return 0;
}

///Return previous haraka in the same word (0 otherwise)
uint32_t GetPrevHaraka(uint32_t *str, size_t length, size_t i)
{
	if(!i)
		return 0;
	
	for(i = i-1; i > 0; i--)
	{
		//We are already at first haraka, return 0 (No prev haraka in word)
		if(str[i] == 0x00 || str[i] == 0x20 || str[i] == '\n')
			return 0;
		
		//If haraka comes, return it
		if(str[i] >= 0xD98B && str[i] <= 0xD992)
			return str[i];
		
		//Those are small harakat, return them as big ones
		if(str[i] >= 0xD898 && str[i] <= 0xD89A)
			return str[i] + 0xF6;
	}
	
	return 0;
}

///Return next haraka in the same word (0 otherwise)
uint32_t GetNextHaraka(uint32_t *str, size_t length, size_t i)
{
	if(i == length-1)
		return 0;
	
	for(i = i+1; i < length; i++)
	{
		//If end of word comes before a haraka, return 0 (No next haraka in word)
		if(str[i] == 0x00 || str[i] == 0x20 || str[i] == '\n')
			return 0;
		
		//If haraka comes, return it
		if(str[i] >= 0xD98B && str[i] <= 0xD992)
			return str[i];
		
		//Those are small harakat, return them as big ones
		if(str[i] >= 0xD898 && str[i] <= 0xD89A)
			return str[i] + 0xF6;
		
		//Small high khah is treated as a sukun
		if(str[i] == 0xDBA1)
			return 0xD992;
	}
	
	return 0;
}

///Returns index of prev REAL letter, and also output the letter itself
size_t GetPrevLetter(uint32_t *str, size_t length, size_t i, uint32_t *out)
{
	if(!i)
	{
		if(out != NULL)
			*out = 0;
		return 0;
	}
	
	for(i = i-1; i > 0; i--)
	{
		//We are already at first letter, return 0 (No prev letter in word)
		if(str[i] == 0x00 || str[i] == 0x20 || str[i] == '\n')
		{
			if(out != NULL)
				*out = 0;
			return 0;
		}
		
		//If Arabic alphabet or number comes, return it
		if((((str[i] >= 0xD8A0 && str[i] <= 0xD98A) || IsYeh(str[i]) || IsHeh(str[i]) || IsNoon(str[i]) || IsAlef(str[i])) && str[i] != 0xD980) || (str[i] >= 0x30 && str[i] <= 0x39))
		{
			if(out != NULL)
				*out = str[i];
			return i;
		}
	}
	
	if(out != NULL)
		*out = str[i];
	return i;
}

///Returns index of next REAL letter, and also output the letter itself
size_t GetNextLetter(uint32_t *str, size_t length, size_t i, uint32_t *out)
{
	if(i == length-1)
	{
		if(out != NULL)
			*out = 0;
		return 0;
	}
	
	for(i = i+1; i < length; i++)
	{
		//If end of word comes before a letter, return 0 (No next letter in word)
		if(str[i] == 0x00 || str[i] == 0x20 || str[i] == '\n')
		{
			if(out != NULL)
				*out = 0;
			return 0;
		}
		
		//If Arabic alphabet or number comes, return it
		if((((str[i] >= 0xD8A0 && str[i] <= 0xD98A) || IsYeh(str[i]) || IsHeh(str[i]) || IsNoon(str[i]) || IsAlef(str[i])) && str[i] != 0xD980) || (str[i] >= 0x30 && str[i] <= 0x39))
		{
			if(out != NULL)
				*out = str[i];
			return i;
		}
	}
	
	return i;
}

///Checks if letter is first in word
unsigned char IsFirstLetter(uint32_t *str, size_t length, size_t i)
{
	if(!i)
		return 1;
	
	for(i=i-1; i > 0; i--)
	{
		if(str[i] == 0x00 || str[i] == 0x20 || str[i] == '\n')
			return 1;
		
		//If Arabic alphabet or number comes, it is not last letter
		if((((str[i] >= 0xD8A0 && str[i] <= 0xD98A) || IsYeh(str[i]) || IsHeh(str[i]) || IsNoon(str[i]) || IsAlef(str[i])) && str[i] != 0xD980) || (str[i] >= 0x30 && str[i] <= 0x39))
			return 0;
	}
	
	return 1;
}

///Checks if letter is midword
unsigned char IsMidwordLetter(uint32_t *str, size_t length, size_t i)
{
	if(!IsFirstLetter(str, length, i) && !IsLastLetter(str, length, i))
		return 1;
		
	return 0;
}

///Checks if letter is last in word
unsigned char IsLastLetter(uint32_t *str, size_t length, size_t i)
{
	if(i == length-1)
		return 1;
	
	for(i = i+1; i < length; i++)
	{
		if(str[i] == 0x00 || str[i] == 0x20 || str[i] == '\n')
			return 1;
		
		//If Arabic alphabet or number comes, it is not last letter
		if((((str[i] >= 0xD8A0 && str[i] <= 0xD98A) || IsYeh(str[i]) || IsHeh(str[i]) || IsNoon(str[i]) || IsAlef(str[i])) && str[i] != 0xD980) || (str[i] >= 0x30 && str[i] <= 0x39))
			return 0;
	}
	
	return 1;
}

//Checks if harf is a harf Illah or not
unsigned char IsHarfIllah(uint32_t c)
{
	if(c == 0xD8A7 || c == 0xD988 || c == 0xD98A)
		return 1;
	
	return 0;
}

///Letter specific checks
unsigned char IsYeh(uint32_t c)
{
	if(c == 0xD98A || c == 0xD8A0 /*|| c == 0xD8A6*/ || (c >= 0xD8BD && c <= 0xD8BF) || (c >= 0xDB8C && c <= 0xDB8E) || c == 0xDB90 || c == 0xDB91)
		return 1;
	
	return 0;
}

unsigned char IsNoon(uint32_t c)
{
	if(c >= 0xDAB9 && c <= 0xD9BD)
		return 1;
		
	return 0;
}

unsigned char IsHeh(uint32_t c)
{
	if(c == 0xDABE || (c >= 0xDB80 && c <= 0xDB82))
		return 1;
	
	return 0;
}

unsigned char IsAlef(uint32_t c)
{
	if(c == 0xD8A7 || c == 0xD8A2 || c == 0xD8A3 || c == 0xD8A5 || c == 0xD9B2 || c == 0xD9B3 || c == 0xD9B1)
		return 1;
	
	return 0;
}

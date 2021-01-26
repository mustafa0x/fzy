#include "util.h"

void UTF8To32(const unsigned char *str, uint32_t **out)
{
	size_t strSize = strsize_utf8(str);
	
	if(!strSize)
	{
		*out = (uint32_t *)malloc(sizeof(uint32_t));
		(*out)[0] = 0;
		return;
	}
	
	*out = (uint32_t *)malloc((strlen_utf8(str) + 1) * sizeof(uint32_t));
	
	size_t i = 0;
	size_t c = 0;
	for(; i < strSize; i++)
	{
		//1-byte (ASCII)
		if(!((str[i] & 0xFF) & 0x80))
		{
			if((str[i] & 0xFF) != 0x00) //Do not add null terminator yet
				(*out)[c++] = (str[i] & 0xFF);
			else
				break;
				
			continue;
		}
		
		//Protection
		if(i+1 >= strSize)
			break;
		
		//2-byte
		if((((str[i] & 0xFF) & 0xE0) == 0xC0) && (((str[i+1] & 0xFF) & 0xC0) == 0x80))
		{
			(*out)[c++] = ((str[i] & 0xFF) << 8) | (str[i+1] & 0xFF);
			i++;
			continue;
		}
		
		//Protection
		if(i+2 >= strSize)
			break;
		
		//3-byte
		if((((str[i] & 0xFF) & 0xF0) == 0xE0) && (((str[i+1] & 0xFF) & 0xC0) == 0x80) && (((str[i+2] & 0xFF) & 0xC0) == 0x80))
		{
			(*out)[c++] = ((str[i] & 0xFF) << 16) | ((str[i+1] & 0xFF) << 8) | (str[i+2] & 0xFF);
			
			i++;
			i++;
			continue;
		}
		
		//Protection
		if(i+3 >= strSize)
			break;
		
		//4-byte
		if((((str[i] & 0xFF) & 0xF8) == 0xF0) && (((str[i+1] & 0xFF) & 0xC0) == 0x80) && (((str[i+2] & 0xFF) & 0xC0) == 0x80) && (((str[i+3] & 0xFF) & 0xC0) == 0x80))
		{
			(*out)[c++] = ((str[i] & 0xFF) << 24) | ((str[i+1] & 0xFF) << 16) | ((str[i+2] & 0xFF) << 8) | (str[i+3] & 0xFF);
			
			i++;
			i++;
			i++;
			continue;
		}
		
		//printf("unknown char!\n");
	}
	
	(*out)[c] = 0; //Null terminator
}

void UTF32To8(const uint32_t *str, unsigned char **out)
{
	size_t strSize = strsize_utf32(str);
	
	if(!strSize)
	{
		*out = (unsigned char *)malloc(sizeof(unsigned char));
		(*out)[0] = 0;
		return;
	}
	
	*out = (unsigned char *)malloc(strSize);
	
	size_t strLength = strlen_utf32(str);
	
	size_t i = 0;
	size_t c = 0;
	for(; i < strLength; i++)
	{
		//1-byte (ASCII)
		if(!(str[i] & 0x80))
		{
			if(str[i] != 0x00)
				(*out)[c++] = str[i] & 0xFF;
			
			continue;
		}
		
		//2-byte
		if((((str[i] >> 8) & 0xE0) == 0xC0) && ((str[i] & 0xC0) == 0x80))
		{
			(*out)[c++] = (str[i] >> 8) & 0xFF;
			(*out)[c++] = str[i] & 0xFF;
			
			continue;
		}
		
		//3-byte
		if((((str[i] >> 16) & 0xF0) == 0xE0) && (((str[i] >> 8) & 0xC0) == 0x80) && ((str[i] & 0xC0) == 0x80))
		{
			(*out)[c++] = (str[i] >> 16) & 0xFF;
			(*out)[c++] = (str[i] >> 8) & 0xFF;
			(*out)[c++] = str[i];
			
			
			continue;
		}
		
		//4-byte
		if((((str[i] >> 24) & 0xF8) == 0xF0) && (((str[i] >> 16) & 0xC0) == 0x80) && (((str[i] >> 8) & 0xC0) == 0x80) && ((str[i] & 0xC0) == 0x80))
		{
			(*out)[c++] = (str[i] >> 24) & 0xFF;
			(*out)[c++] = (str[i] >> 16) & 0xFF;
			(*out)[c++] = (str[i] >> 8) & 0xFF;
			(*out)[c++] = str[i];
			
			continue;
		}
	}
	
	(*out)[c] = 0; //Null terminator
}

///Returns string size
size_t strsize_utf8(const unsigned char *str)
{
	if(str == NULL)
		return 0;
		
	size_t i = 0;
	for(;; i++)
		if((str[i] & 0xFF) == 0)
			break;
			
	i++;
		
	return i;
}

///Returns string length
size_t strlen_utf8(const unsigned char *str)
{
	if(str == NULL)
		return 0;
		
	size_t strSize = strsize_utf8(str);
	
	size_t i = 0;
	size_t l = 0;
	
	for(; i < strSize; i++)
	{	
		//1-byte (ASCII)
		if(!((str[i] & 0xFF) & 0x80))
		{
			if((str[i] & 0xFF) == 0x00)
				break;
				
			l++;
				
			continue;
		}
		
		//Protection
		if(i+1 >= strSize)
			break;
		
		//2-byte
		if((((str[i] & 0xFF) & 0xE0) == 0xC0) && (((str[i+1] & 0xFF) & 0xC0) == 0x80))
		{
			l++;
			
			i++;
			continue;
		}
		
		//Protection
		if(i+2 >= strSize)
			break;
		
		//3-byte
		if((((str[i] & 0xFF) & 0xF0) == 0xE0) && (((str[i+1] & 0xFF) & 0xC0) == 0x80) && (((str[i+2] & 0xFF) & 0xC0) == 0x80))
		{
			l++;
			
			i++;
			i++;
			continue;
		}
		
		//Protection
		if(i+3 >= strSize)
			break;
		
		//4-byte
		if((((str[i] & 0xFF) & 0xF8) == 0xF0) && (((str[i+1] & 0xFF) & 0xC0) == 0x80) && (((str[i+2] & 0xFF) & 0xC0) == 0x80) && (((str[i+3] & 0xFF) & 0xC0) == 0x80))
		{
			l++;

			i++;
			i++;
			i++;
			continue;
		}
	}
	
	return l;
}

///Returns string length
size_t strlen_utf32(const uint32_t *str)
{
	if(str == NULL)
		return 0;
		
	size_t i = 0;
	for(;; i++)
		if(str[i] == 0x00)
			break;
			
	return i;
}

///Returns string REAL size in bytes
size_t strsize_utf32(const uint32_t *str)
{
	if(str == NULL)
		return 0;
		
	size_t i = 0;
	size_t sz = 0;
	
	for(;; i++)
	{
		//1-byte (ASCII)
		if(!(str[i] & 0x80))
		{
			if(str[i] == 0)
				break;
				
			sz++;
			continue;
		}

		//2-byte
		if((((str[i] >> 8) & 0xE0) == 0xC0) && ((str[i] & 0xC0) == 0x80))
		{
			sz += 2;
			continue;
		}
		
		//3-byte
		if((((str[i] >> 16) & 0xF0) == 0xE0) && (((str[i] >> 8) & 0xC0) == 0x80) && ((str[i] & 0xC0) == 0x80))
		{
			sz += 3;
			continue;
		}
		
		//4-byte
		if((((str[i] >> 24) & 0xF8) == 0xF0) && (((str[i] >> 16) & 0xC0) == 0x80) && (((str[i] >> 8) & 0xC0) == 0x80) && ((str[i] & 0xC0) == 0x80))
		{
			sz += 4;
			continue;
		}
		
		sz++; //In case char was not a unicode char
	}
	sz++;
	
	return sz;
}

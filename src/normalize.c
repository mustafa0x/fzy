#include "normalize.h"

size_t normalize(const uint32_t *src, uint32_t **dst)
{
	//Arabic
	uint32_t *normalized = NULL;
	size_t length = Arabic_SkimString(src, &normalized);

	//Latin

	//Make all letters capital
	for(size_t i = 0; i < length; i++)
	{
		if(normalized[i] >= 0x61 && normalized[i] <= 0x7A)
			normalized[i] -= 0x20;
	}

	*dst = normalized;
	return length;
}

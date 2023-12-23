#pragma once
#include <string.h>

bool cmpncend(const char* str1, const char* str2)
{
	size_t len1 = strlen(str1);
	size_t len2 = strlen(str2);

	if (len1 < len2)
		return false;

	for (size_t i = 0; i < len2; ++i)
	{
		if (str1[len1 - len2 + i] != str2[i])
			return false;
	}

	return true;
}
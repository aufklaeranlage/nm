#include <elf.h>

static inline void swap_char(char *a, char *b)
{
	char c;
	c = *a;
	*a = *b;
	*b = c;
}

uint16_t	lsbuint16(char *data)
{
	uint16_t ret = *(uint16_t *)data;
	char *arr = (char *)&ret;
	swap_char(arr + 0, arr + 1);
	return ret;
}

int16_t		lsbint16(char *data)
{
	int16_t ret = *(int16_t *)data;
	char *arr = (char *)&ret;
	swap_char(arr + 0, arr + 1);
	return ret;
}

uint32_t	lsbuint32(char *data)
{
	uint32_t ret = *(uint32_t *)data;
	char *arr = (char *)&ret;
	swap_char(arr + 0, arr + 3);
	swap_char(arr + 1, arr + 2);
	return ret;
}

int32_t		lsbint32(char *data)
{
	int32_t ret = *(int32_t *)data;
	char *arr = (char *)&ret;
	swap_char(arr + 0, arr + 3);
	swap_char(arr + 1, arr + 2);
	return ret;
}

uint64_t	lsbuint64(char *data)
{
	uint64_t ret = *(uint64_t *)data;
	char *arr = (char *)&ret;
	swap_char(arr + 0, arr + 7);
	swap_char(arr + 1, arr + 6);
	swap_char(arr + 2, arr + 5);
	swap_char(arr + 3, arr + 4);
	return ret;
}

int64_t		lsbint64(char *data)
{
	int64_t ret = *(int64_t *)data;
	char *arr = (char *)&ret;
	swap_char(arr + 0, arr + 7);
	swap_char(arr + 1, arr + 6);
	swap_char(arr + 2, arr + 5);
	swap_char(arr + 3, arr + 4);
	return ret;
}


uint16_t	msbuint16(char *data)
{
	return *(uint16_t *)data;
}

int16_t		msbint16(char *data)
{
	return *(int16_t *)data;
}

uint32_t	msbuint32(char *data)
{
	return *(uint32_t *)data;
}

int32_t		msbint32(char *data)
{
	return *(int32_t *)data;
}

uint64_t	msbuint64(char *data)
{
	return *(uint64_t *)data;
}

int64_t		msbint64(char *data)
{
	return *(int64_t *)data;
}

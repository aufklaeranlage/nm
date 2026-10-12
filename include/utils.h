#ifndef UTILS_H
# define UTILS_H

# include <elf.h>

uint16_t	lsbuint16(char *data);
int16_t		lsbint16(char *data);
uint32_t	lsbuint32(char *data);
int32_t		lsbint32(char *data);
uint64_t	lsbuint64(char *data);
int64_t		lsbint64(char *data);

uint16_t	msbuint16(char *data);
int16_t		msbint16(char *data);
uint32_t	msbuint32(char *data);
int32_t		msbint32(char *data);
uint64_t	msbuint64(char *data);
int64_t		msbint64(char *data);

#endif

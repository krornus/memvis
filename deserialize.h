#include <stdio.h>

#ifndef SEEK_SET
#define SEEK_SET 0
#endif

#ifndef SEEK_SET
#define SEEK_CUR 1
#endif

#ifndef SEEK_END
#define SEEK_END 2
#endif

#ifndef BIG_ENDIAN
#define BIG_ENDIAN 4321
#endif

#ifndef LITTLE_ENDIAN
#define LITTLE_ENDIAN 1234
#endif

#define create_element(order, parent, member) { \
  .address = &(parent).member,\
  .size = sizeof((parent).member),\
  .endianness = order\
}

typedef struct { 
  unsigned long len;
  unsigned long offset;
  int order;
  char *bytes;
} Serializable;

typedef struct {
  void *address;
  int size;
  int endianness;
} Element;


void deserialize(Serializable *ser, Element *values, int length);
void srewind(Serializable *ser);
void sseek(Serializable *ser, long amount, int whence);
int smap(char *fn, Serializable *ser);
int sunmap(Serializable *ser);
size_t fsize(FILE *fp);

#include "deserialize.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

size_t data_length;

void deserialize(Serializable *ser, Element *values, int length)
{
    int host_order,guest_order,pos,order_tester, host_endianness;

    order_tester = 1;
    host_endianness = (((char *)&order_tester)[0] ? LITTLE_ENDIAN : BIG_ENDIAN);

    for(int x = 0; x < length; x++)
    {
        int host_incrementer, host_offset, guest_incrementer, guest_offset;

        if(host_endianness == LITTLE_ENDIAN)
        {
            host_offset = 0;
            host_incrementer = 1;
        }
        else
        {
            host_offset = values->size-1;
            host_incrementer = -1;
        }

        if(values->endianness == LITTLE_ENDIAN)
        {
            guest_offset = 0;
            guest_incrementer = 1;
        }
        else
        {
            guest_offset = values->size-1;
            guest_incrementer = -1;
        }

        for(int ctr = 0; ctr < values->size; ctr++)
        {
            /* target address + (host_endianness) = bytestream position + (guest_endianness) */
            *((char *)values->address + host_offset) = *(ser->bytes + ser->offset + guest_offset);

            host_offset += host_incrementer;
            guest_offset += guest_incrementer;
        }
        ser->offset += values->size;
        values++;
    }
}


void srewind(Serializable *ser)
{
    ser->offset = 0;
}

void sseek(Serializable *ser, long amount, int whence)
{
    switch(whence)
    {
        case SEEK_SET:
            ser->offset = amount;
            break;
        case SEEK_CUR:
            if (ser->offset + amount < ser->len)
                ser->offset += amount;
            break;
        case SEEK_END:
            if (ser->len - 1 - amount > -1)
                ser->offset -= ser->len - 1 - amount;
            break;
    }
}

int smap(char* fn, Serializable *ser)
{
    int fd; 
    struct stat sb;

    fd = open(fn, O_RDONLY);

    if(fd == -1)
    {
        perror("open()");
        return -1;
    }

    if(fstat(fd, &sb) == -1)
    {
        perror("fstat()");
        return -1;
    }

    ser->len = (int)sb.st_size;

    if(!S_ISREG(sb.st_mode)) 
    {
        fprintf(stderr, "invalid file: %s\n", fn);
        return -1;
    }
    
    ser->bytes = mmap(0, ser->len, PROT_READ, MAP_PRIVATE, fd, 0);
    
    if(MAP_FAILED == ser->bytes)
    {
        perror("mmap()");
        return 1;
    }

    if(close(fd) == -1)
    {
        perror("close()");
        return -1;
    }

    ser->offset = 0;
    
    /* default to little endian */
    ser->order = LITTLE_ENDIAN;

    return 0;
}

int sunmap(Serializable *ser)
{
    return munmap(ser->bytes, ser->len);
}

size_t fsize(FILE *fp)
{
    size_t size, init_seek;

    init_seek = ftell(fp);
    fseek(fp, 0, SEEK_END);
    size = ftell(fp);

    fseek(fp, init_seek, SEEK_SET);

    return size;
}

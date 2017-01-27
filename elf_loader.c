#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "hash.h"
#include "deserialize.h"
#include "elf_loader.h"


int main(int argc, char **argv)
{
    FILE *fp;
    size_t size;
    Serializable *ser;
    Elf32 *elf32;

    if(argc != 2)
    {
        printf("Usage: ./elf-loader <file>\n");
        return -1;
    }

    ser = (Serializable *) malloc(sizeof(Serializable));

    if(0 != smap(argv[1], ser))
    {
        fprintf(stderr, "failed to map %s\n", argv[1]);
        return -1;
    }

    ser->order = (ser->bytes[EI_DATA] == ELFDATA2MSB ? BIG_ENDIAN : LITTLE_ENDIAN);

    elf32 = (Elf32 *) malloc(sizeof(Elf32));
    
    if(load_32bit(ser, elf32) != 0)
    {
        return -1;
    }

    /* Retrieve global offset table */
    Elf32_Shdr *sect = get_section(elf32->hash, ".text");
    if(NULL==sect)
    {
        fprintf(stderr, "could not load section\n");
        exit(-1);
    }

    for(int i = sect->sh_offset; i < sect->sh_offset + sect->sh_size; i++)
    {
        if(i % 10 == 0 && i > 0)
        {
            printf("\n");
        }

        printf("%01x", (unsigned char)ser->bytes[i]);
    }

    sunmap(ser);
    destroy_32bit(elf32);
    
    free(ser);
    free(elf32);
}

int load_32bit(Serializable *prg, Elf32 *elf)
{
    Elf32_Ehdr eheader;
    Elf32_Phdr pheader;
    Elf32_Shdr strtab;

    /* Must set dict to null */
    elf->hash = NULL;

    /* Get ELF header */
    if(load_32bit_eheader(prg, &eheader) != 0)
    {
        return -1;
    }  

    /* Get program header */
    sseek(prg, eheader.e_phoff, SEEK_SET);
    if(load_32bit_pheader(prg, &pheader) != 0)
    {
        return -1;          
    }

    /* Get section headers */
    sseek(prg, eheader.e_shoff, SEEK_SET);

    elf->sheaders = (Elf32_Shdr *)malloc(sizeof(Elf32_Shdr) * eheader.e_shnum);

    for(int i = 0; i < eheader.e_shnum; i++)
    {
        load_32bit_sheader(prg, elf->sheaders + i); 
    }

    /* Load string table */
    strtab = elf->sheaders[eheader.e_shstrndx];

    /* Loop through sections */
    for(int i = 0; i < eheader.e_shnum; i++)
    {
        char *name;
        name = get_section_name(prg, strtab, elf->sheaders[i].sh_name);
        hset(&elf->hash, name, elf->sheaders+i);
    }


    return 0;
}

void destroy_32bit(Elf32 *elf)
{
    free(elf->sheaders);
}

char *get_section_name(Serializable *prg, Elf32_Shdr strtab, unsigned long long offset)
{
	return prg->bytes + strtab.sh_offset + offset;
}

Elf32_Shdr *get_section(struct Hentry *hash, char *name)
{
    Elf32_Shdr *sect;
    sect = (Elf32_Shdr *)hget(hash, name);

    return sect;
}

int load_32bit_eheader(Serializable *prg, Elf32_Ehdr *eheader)
{
	/* Assume data is valid */
	if(strncmp(ELF_MAGIC, prg->bytes, 4) != 0)
	{ 
		printf("[-] Error, bad magic number.\n");
		return -1;
	}

	if(prg->bytes[EI_CLASS] != ELFCLASS32)
	{
		printf("[-] Error, binary is not 32 bit\n");
		return -1;
	}  

	Element header_elements[] = {
		create_element(prg->order, *eheader, e_ident),
		create_element(prg->order, *eheader, e_type),
		create_element(prg->order, *eheader, e_machine),
		create_element(prg->order, *eheader, e_version),
		create_element(prg->order, *eheader, e_entry),
		create_element(prg->order, *eheader, e_phoff),
		create_element(prg->order, *eheader, e_shoff),
		create_element(prg->order, *eheader, e_flags),
		create_element(prg->order, *eheader, e_ehsize),
		create_element(prg->order, *eheader, e_phentsize),
		create_element(prg->order, *eheader, e_phnum),
		create_element(prg->order, *eheader, e_shentsize),
		create_element(prg->order, *eheader, e_shnum),
		create_element(prg->order, *eheader, e_shstrndx)
	};

	deserialize(prg, header_elements, N_EHEADER);
	return 0;
}


int load_32bit_pheader(Serializable *prg, Elf32_Phdr *pheader)
{
	if(prg->bytes[EI_CLASS] != ELFCLASS32)
	{
		printf("[-] Error, binary is not 32 bit\n");
		return -1;
	}

	Element header_elements[] = {
		create_element(prg->order, *pheader, p_type),
		create_element(prg->order, *pheader, p_offset),
		create_element(prg->order, *pheader, p_vaddr),
		create_element(prg->order, *pheader, p_paddr),
		create_element(prg->order, *pheader, p_filesz),
		create_element(prg->order, *pheader, p_memsz),
		create_element(prg->order, *pheader, p_flags),
		create_element(prg->order, *pheader, p_align)
	};

	deserialize(prg, header_elements, sizeof(header_elements)/sizeof(Element));
	return 0;
}


int load_32bit_sheader(Serializable *prg, Elf32_Shdr *sheader)
{
	if(prg->bytes[EI_CLASS] != ELFCLASS32)
	{
		printf("[-] Error, binary is not 32 bit\n");
		return -1;
	}

	Element header_elements[] = {
		create_element(prg->order, *sheader, sh_name),
		create_element(prg->order, *sheader, sh_type),
		create_element(prg->order, *sheader, sh_flags),
		create_element(prg->order, *sheader, sh_addr),
		create_element(prg->order, *sheader, sh_offset),
		create_element(prg->order, *sheader, sh_size),
		create_element(prg->order, *sheader, sh_link),
		create_element(prg->order, *sheader, sh_info),
		create_element(prg->order, *sheader, sh_addralign),
		create_element(prg->order, *sheader, sh_entsize)
	};

	deserialize(prg, header_elements, sizeof(header_elements)/sizeof(Element));
	return 0;
}



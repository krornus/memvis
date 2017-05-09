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


    /******** .TEXT SECTION *********/
    Elf32_Shdr *text = get_section(elf32->hash, ".text");

    if(NULL==text)
    {
        fprintf(stderr, "could not load section\n");
        exit(-1);
    }

    for(int i = text->sh_offset; i < text->sh_offset + text->sh_size; i++)
    {
        if(i % 10 == 0 && i > 0)
        {
            printf("\n");
        }

        printf("%01x", (unsigned char)ser->bytes[i]);
    }
    /****** END .TEXT SECTION *******/
    
    /****** .BSS SECTION ***********/
    Elf32_Shdr *bss = get_section(elf32->hash, ".bss");

    if(NULL==bss)
    {
        fprintf(stderr, "could not load .bss seciont");
        exit(-1);
    }
    printf("printing bss:\n");
    for(int i = bss->sh_offset; i < bss + bss->sh_size; i++)
    {
        printf("%X",i);
        break;
    }


    /******** SYMTAB *********/
    Elf32_Shdr *symtab_header = get_section(elf32->hash, ".symtab");

    if(NULL==text)
    {
        fprintf(stderr, "could not load section\n");
        exit(-1);
    }


    /****** END SYMTAB .*******/ 
    
    sunmap(ser);
    destroy_32bit(elf32);
    
    free(ser);
    free(elf32);
}

int load_32bit(Serializable *prg, Elf32 *elf)
{
    int nsym;
    Elf32_Ehdr eheader;
    Elf32_Phdr pheader;
    Elf32_Shdr strtab_header, *symtab_header;
    Elf32_Sym sym;

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
    strtab_header = elf->sheaders[eheader.e_shstrndx];

    /* Loop through sections */
    for(int i = 0; i < eheader.e_shnum; i++)
    {
        char *name;
        name = get_name(prg, strtab_header, elf->sheaders[i].sh_name);
        printf("Loaded section '%s'\n", name);
        hset(&elf->hash, name, elf->sheaders+i);
    }

    /* Load symbol table header */
    symtab_header = get_section(elf->hash, ".symtab");

    if(symtab_header==NULL)
    {
        fprintf(stderr, "No symbol table in binary\n");
        return -1;
    }
    else
    {
        /* Seek to symtab offset */
        sseek(prg, symtab_header->sh_offset, SEEK_SET);
        
        /* TODO: Better way to do this? */
        load_32bit_sym(prg, &sym);

        /* Get the number of symbols */
        nsym = symtab_header->sh_size / (prg->offset - symtab_header->sh_offset);
        elf->symbols = (Elf32_Sym *) malloc(sizeof(Elf32_Sym)*nsym);

        printf("There are %lu symbols\n", nsym);

        sseek(prg, symtab_header->sh_offset, SEEK_SET);

        for(int i = 0; i < nsym; i++)
        {
            load_32bit_sym(prg, elf->symbols + i);
            if(elf->symbols[i].st_name!=0)
                printf("Loaded symbol '%s'\n", get_name(prg, strtab_header, elf->symbols[i].st_name));
        }
    }

    return 0;
}

void destroy_32bit(Elf32 *elf)
{
    free(elf->sheaders);
}

char *get_name(Serializable *prg, Elf32_Shdr strtab, unsigned long long offset)
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
		CREATE_ELEMENT(prg->order, *eheader, e_ident),
		CREATE_ELEMENT(prg->order, *eheader, e_type),
		CREATE_ELEMENT(prg->order, *eheader, e_machine),
		CREATE_ELEMENT(prg->order, *eheader, e_version),
		CREATE_ELEMENT(prg->order, *eheader, e_entry),
		CREATE_ELEMENT(prg->order, *eheader, e_phoff),
		CREATE_ELEMENT(prg->order, *eheader, e_shoff),
		CREATE_ELEMENT(prg->order, *eheader, e_flags),
		CREATE_ELEMENT(prg->order, *eheader, e_ehsize),
		CREATE_ELEMENT(prg->order, *eheader, e_phentsize),
		CREATE_ELEMENT(prg->order, *eheader, e_phnum),
		CREATE_ELEMENT(prg->order, *eheader, e_shentsize),
		CREATE_ELEMENT(prg->order, *eheader, e_shnum),
		CREATE_ELEMENT(prg->order, *eheader, e_shstrndx)
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
		CREATE_ELEMENT(prg->order, *pheader, p_type),
		CREATE_ELEMENT(prg->order, *pheader, p_offset),
		CREATE_ELEMENT(prg->order, *pheader, p_vaddr),
		CREATE_ELEMENT(prg->order, *pheader, p_paddr),
		CREATE_ELEMENT(prg->order, *pheader, p_filesz),
		CREATE_ELEMENT(prg->order, *pheader, p_memsz),
		CREATE_ELEMENT(prg->order, *pheader, p_flags),
		CREATE_ELEMENT(prg->order, *pheader, p_align)
	};

	deserialize(prg, header_elements, sizeof(header_elements)/sizeof(Element));
	return 0;
}


int load_32bit_sym(Serializable *prg, Elf32_Sym *symbol)
{
	Element header_elements[] = {
		CREATE_ELEMENT(prg->order, *symbol, st_name),
		CREATE_ELEMENT(prg->order, *symbol, st_value),
		CREATE_ELEMENT(prg->order, *symbol, st_size),
		CREATE_ELEMENT(prg->order, *symbol, st_info),
		CREATE_ELEMENT(prg->order, *symbol, st_other),
		CREATE_ELEMENT(prg->order, *symbol, st_shndx)
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
		CREATE_ELEMENT(prg->order, *sheader, sh_name),
		CREATE_ELEMENT(prg->order, *sheader, sh_type),
		CREATE_ELEMENT(prg->order, *sheader, sh_flags),
		CREATE_ELEMENT(prg->order, *sheader, sh_addr),
		CREATE_ELEMENT(prg->order, *sheader, sh_offset),
		CREATE_ELEMENT(prg->order, *sheader, sh_size),
		CREATE_ELEMENT(prg->order, *sheader, sh_link),
		CREATE_ELEMENT(prg->order, *sheader, sh_info),
		CREATE_ELEMENT(prg->order, *sheader, sh_addralign),
		CREATE_ELEMENT(prg->order, *sheader, sh_entsize)
	};

	deserialize(prg, header_elements, sizeof(header_elements)/sizeof(Element));
	return 0;
}

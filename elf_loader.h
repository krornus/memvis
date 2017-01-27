#include "uthash.h"

#define ELF_MAGIC "\x7f\x45\x4c\x46"
/* Number of elements in ELF header */
#define N_EHEADER 14

/* size of e_ident in elf header */
#define EI_NIDENT 16

/* File class */
#define EI_CLASS 4

/* Possible values for class */
#define ELFCLASSNONE 0
#define ELFCLASS32 1
#define ELFCLASS64 2

/* Data encoding */
#define EI_DATA 5
#define ELFDATANONE 0
#define ELFDATA2LSB 1
#define ELFDATA2MSB 2

/* File version */
#define EI_VERSION 6
#define EV_NONE 0
#define EV_CURRENT 1

/* String table */
#define SHT_STRTAB 3

/* ELF Data Types */
typedef unsigned int Elf32_Addr;
typedef unsigned short Elf32_Half;
typedef unsigned int Elf32_Off;
typedef unsigned int Elf32_Sword;
typedef unsigned int Elf32_Word;


typedef unsigned long Elf64_Addr;
typedef unsigned short Elf64_Half;
typedef unsigned long Elf64_Off;
typedef unsigned int Elf64_Sword;
typedef unsigned int Elf64_Word;

/*Text section struct*/
typedef struct {
	void* offset;
	Elf32_Word length;
} Elf32_s_text;

/* ELF Header structs */
typedef struct {
	unsigned char e_ident[EI_NIDENT];
	Elf32_Half e_type;
	Elf32_Half e_machine;
	Elf32_Word e_version;
	Elf32_Addr e_entry;
	Elf32_Off e_phoff;
	Elf32_Off e_shoff;
	Elf32_Word e_flags;
	Elf32_Half e_ehsize;
	Elf32_Half e_phentsize;
	Elf32_Half e_phnum;
	Elf32_Half e_shentsize;
	Elf32_Half e_shnum;
	Elf32_Half e_shstrndx;
} Elf32_Ehdr;

typedef struct {
	Elf64_Half e_ident[EI_NIDENT];
	Elf64_Half e_type;
	Elf64_Word e_machine;
	Elf64_Addr e_version;
	Elf64_Off  e_entry;
	Elf64_Off  e_phoff;
	Elf64_Word e_shoff;
	Elf64_Half e_flags;
	Elf64_Half e_ehsize;
	Elf64_Half e_phentsize;
	Elf64_Half e_phnum;
	Elf64_Half e_shentsize;
	Elf64_Half e_shnum;
	Elf64_Half e_shstrndx;
} Elf64_Ehdr;


typedef struct {                    
	Elf32_Word sh_name;
	Elf32_Word sh_type;
	Elf32_Word sh_flags;
	Elf32_Addr sh_addr;
	Elf32_Off  sh_offset;
	Elf32_Word sh_size;
	Elf32_Word sh_link;
	Elf32_Word sh_info;
	Elf32_Word sh_addralign;
	Elf32_Word sh_entsize;
} Elf32_Shdr;

typedef struct {                    
	Elf64_Word sh_name;
	Elf64_Word sh_type;
	Elf64_Word sh_flags;
	Elf64_Addr sh_addr;
	Elf64_Off  sh_offset;
	Elf64_Word sh_size;
	Elf64_Word sh_link;
	Elf64_Word sh_info;
	Elf64_Word sh_addralign;
	Elf64_Word sh_entsize;
} Elf64_Shdr;


typedef struct {
	Elf32_Word st_name;
	Elf32_Addr st_value;
	Elf32_Word st_size;
	unsigned char st_info;
	unsigned char st_other;
	Elf32_Half st_shndx;
} Elf32_Sym;

typedef struct {
	Elf64_Word st_name;
	Elf64_Addr st_value;
	Elf64_Word st_size;
	unsigned char st_info;
	unsigned char st_other;
	Elf64_Half st_shndx;
} Elf64_Sym;

typedef struct {
	size_t size;
	unsigned char bytes[];
} Program; 


typedef struct {
	Elf32_Addr r_offset;
	Elf32_Word r_info;
} Elf32_Rel;

typedef struct {
	Elf64_Addr r_offset;
	Elf64_Word r_info;
} Elf64_Rel;

typedef struct {
	Elf32_Addr r_offset;
	Elf32_Word r_info;
	Elf32_Sword r_addend;
} Elf32_Rela;

typedef struct {
	Elf64_Addr r_offset;
	Elf64_Word r_info;
	Elf64_Sword r_addend;
} Elf64_Rela;

typedef struct {
	Elf32_Word p_type;
	Elf32_Off  p_offset;
	Elf32_Addr p_vaddr;
	Elf32_Addr p_paddr;
	Elf32_Word p_filesz;
	Elf32_Word p_memsz;
	Elf32_Word p_flags;
	Elf32_Word p_align;
} Elf32_Phdr;

typedef struct {
	Elf64_Word p_type;
	Elf64_Off p_offset;
	Elf64_Addr p_vaddr;
	Elf64_Addr p_paddr;
	Elf64_Word p_filesz;
	Elf64_Word p_memsz;
	Elf64_Word p_flags;
	Elf64_Word p_align;
} Elf64_Phdr;


typedef struct {
  Elf32_Ehdr header;
  Elf32_Shdr *sheaders;
  Elf32_Sym *symbols;
  struct Hentry *hash;
} Elf32;

int parse_elf_header(Serializable *prg);
int load_32bit(Serializable *prg, Elf32 *elf);
void destroy_32bit(Elf32 *elf);
int load_32bit_eheader(Serializable *prg, Elf32_Ehdr *eheader);
int load_32bit_pheader(Serializable *prg, Elf32_Phdr *pheader);
int load_32bit_sheader(Serializable *prg, Elf32_Shdr *sheader);
int load_32bit_sym(Serializable *prg, Elf32_Sym *symbol);
char *get_name(Serializable *prg, Elf32_Shdr strtab, unsigned long long offset);
Elf32_Shdr *get_section(struct Hentry *hash, char *key);

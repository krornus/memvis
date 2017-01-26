# memvis
Tool for interfacing with ELF files

### Deserializer Usage

```c
    Serializable *ser; /* Deserializer context */
    Elf32_Ehdr *eheader; /* Struct to hold data */
    size_t size; /* The size of the raw data you wish to deserialize */
    
    ser = (Serializable *) malloc(sizeof(Serializable));
    eheader = (Elf32_Ehdr) malloc(sizeof(Elf32_Ehdr);
    
    /* mmap the file */
    if(0 != smap(argv[1], ser))
    {
        fprintf(stderr, "failed to map %s\n", argv[1]);
        return -1;
    }
    
    /* set the endianness (defaults to little) */
    ser->order = LITTLE_ENDIAN;
    
    /* Prepare the struct */
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
```

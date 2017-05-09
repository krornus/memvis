# Authors:
Spencer Powell - elfldr.c
Carlos Guerra - database functionality
Wayne Havey - Python/Yara

# UCCS 2017 - Undergraduate Independent Study
Field: Cyber Security
Advisor: Dr. Chow

## memvis ##
Tool for interfacing with ELF files

## The idea ##
Use the elfldr c program to interface with and extract information from a binary file (32bit currently), that information gets placed into a database where it can be accessed easily by python and used in conjunction with custom yara signatures to detect vulnerabilities within a binary without needing access to the source code.

*********USAGE**********
#### Deserializer Usage ####
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

usage: ./elfldr --file|-f <filename> --section|-s <section_name> 

#### Python/YARA USAGE ####

Python directory contains:
    - Disassemble.py
        description: capstone module implemented to print assembly and other section info from binary
        usage: python2.7 disassemble.py <filename>
        info: this file can be modified to print info from every section see documentation on python capstone

    - rules (directory)
        description: Repository of yara rules. Most are for malware detection, some CVE detection. 
        usage: use this repo in conjunction with run_rules.sh script. ./run_rules.sh <file>
        info: any new rules written for vulnerability detection should be placed in here or the run_rules.sh script should be modfied to point to the new rules repo.
        
    - run_rules.sh
        description: script to run yara rules on a specified file.
        usage: ./run_rules.sh <file>
        info: This script points to the rules directory and runs every yara rule contained within on the specified file.
              See yara command line usage for more information on tailoring this script.

    - rules.yar
        description: Beginnings of a yara rule file tailored to elf analysis.
        usage: see yara usage command line usage.
        info: Yara has a builin api for accessing information for elf files.  

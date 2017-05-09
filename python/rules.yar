import "elf"

rule single_section
{
    condition:
        elf.number_of_sections == 1
}

rule elf_64
{
    condition:
        elf.machine == elf.EM_X86_64
}

import sys
from elftools.elf.elffile import ELFFile
from capstone import *

def process_file(filename):
    with open(filename, 'rb') as f:
        elffile = ELFFile(f)
        code = elffile.get_section_by_name('.text')
        rodata = elffile.get_section_by_name('.rodata')
        plt = elffile.get_section_by_name('.plt')
        data = elffile.get_section_by_name('.data')
        bss = elffile.get_section_by_name('.bss')

        opcodes = code.data()
        addr = code['sh_addr']
        #print "Entry point: {0}".format(hex(elffile.header['e_entry']))
        md = Cs(CS_ARCH_X86, CS_MODE_64)
        for i in md.disasm(opcodes, addr):
            print "0x%x:\t%s\t%s\t" %(i.address, i.mnemonic, i.op_str)
        print "\n\nrodata:\n"
        print rodata.data()
        print "\n\nplt\n"
        print plt.data()
        print "\n\ndata\n"
        print data.data()
        print "\n\nbss\n"
        print bss.data()
if __name__ == '__main__':
    if len(sys.argv) == 2:
        process_file(sys.argv[1])

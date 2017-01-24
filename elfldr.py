import struct

class ELF(object):

  MAGIC_NUMBER = '\x7FELF'
  ENDIANNESS = {
    '\x01':"little endian",
    '\x02':"big endian"
  }
  WORD_SIZE = {
    '\x01':32,
    '\x02':64
  }
  EXEC_TYPE = {
    '\x01\x00':"relocatable",
    '\x02\x00':"executable",
    '\x03\x00':"shared",
    '\x04\x00':"core",
    '\xff\x00':"processor-specific",
    '\xff\xff':"processor-specific"
  }
  INSTRUCTION_SET = {
    '\x00':"unspecified",
    '\x02':"Sparc",
    '\x03':"x86",
    '\x08':"MIPS",
    '\x14':"PowerPC",
    '\x28':"ARM",
    '\x2A':"SuperH",
    '\x32':"IA-64",
    '\x3E':"x84-64",
    '\xB7':"AArch64"
  }

  DATA_TYPES = {
    Elf32_Addr:    4,
    Elf32_Half:    2,
    Elf32_Off:     4,
    Elf32_Sword:   4,
    Elf32_Word:    4,
    unsigned_char: 1
  }


  def __init__(self, fn):
    self.bytes = self.load(fn)
    self.parse_header(fn)


  def parse_header(self, fn):

    if self.bytes[:4] != ELF.MAGIC_NUMBER:
      print "Invalid file signature:"
      print "\tExpected: '%s', got '%s'".format(hex(ELF.MAGIC_NUMBER), hex(self.bytes[:4]))

    if self.bytes[4] in ELF.WORD_SIZE:
      self.word_size = ELF.WORD_SIZE[self.bytes[4]]
    else:
      self.word_size = 0

    if self.bytes[5] == '\x01':
      self.endianness = "little endian" 
      self._endianness = 1
      self.struct_endianness = "<"

    elif self.bytes[5] == '\x02':
      self.endianness = "big endian"
      self._endianness = 2
      self.struct_endianness = ">"

    else:
      self.endianness = "unknown"
      self._endianness = 0

    self.version = ord(self.bytes[6])
    self.osabi = ord(self.bytes[7])

    if self.bytes[16:18] in ELF.EXEC_TYPE:
      self.exec_type = ELF.EXEC_TYPE[self.bytes[16:18]]
    else:
      self.exec_type = "unknown"


    if self.bytes[18] in ELF.INSTRUCTION_SET:
      self.instruction_set = ELF.INSTRUCTION_SET[self.bytes[18]]
    else:
      self.instruction_set = "unknown"
    
    self.elf_version = self.bytes[20:24].encode("hex")

    if self.word_size == 32:
      self.parse_elf_header_32()
    elif self.word_size == 64:
      self.parse_elf_header_64()
    else:
      print "unable to parse all of header, unknown wordsize"
      exit(1)

    self.program_header_size = self.program_header_entry_size * self.program_header_entry_count
    self.program_header = self.bytes[self.program_header_table:self.program_header_table + self.program_header_size] 
     


  def parse_elf_header_32(self):
    word = self.struct_endianness + "I"
    integer = self.struct_endianness + "I"
    short = self.struct_endianness + "H"


    self.program_entry = struct.unpack(word, self.bytes[24:28])[0]
    self.program_header_table = struct.unpack(word, self.bytes[28:32])[0]
    self.section_header_table = struct.unpack(integer, self.bytes[32:36])[0]
    self.flags = struct.unpack(integer, self.bytes[36:40])[0]
    self.header_size = struct.unpack(short, self.bytes[40:42])[0]
    self.program_header_entry_size = struct.unpack(short, self.bytes[42:44])[0]
    self.program_header_entry_count = struct.unpack(short, self.bytes[44:46])[0]
    self.section_header_entry_size = struct.unpack(short, self.bytes[46:48])[0]
    self.section_header_entry_count = struct.unpack(short, self.bytes[48:50])[0]
    self.section_header_name_index  = struct.unpack(short, self.bytes[50:52])[0]
    

  def parse_elf_header_64(self):
    # quad word
    word = self.struct_endianness + "q"
    integer = self.struct_endianness + "I"
    short = self.struct_endianness + "H"

    self.program_entry = struct.unpack(word, self.bytes[24:32])[0]
    self.program_header_table = struct.unpack(word, self.bytes[32:40])[0]
    self.section_header_table = struct.unpack(word, self.bytes[40:48])[0]
    self.flags = struct.unpack(integer, self.bytes[48:52])[0]
    self.header_size = struct.unpack(short, self.bytes[52:54])[0]
    self.program_header_entry_size = struct.unpack(short, self.bytes[54:56])[0]
    self.program_header_entry_count = struct.unpack(short, self.bytes[56:58])[0]
    self.section_header_entry_size = struct.unpack(short, self.bytes[58:60])[0]
    self.section_header_entry_count = struct.unpack(short, self.bytes[60:62])[0]
    self.section_header_name_index  = struct.unpack(short, self.bytes[62:64])[0]

    
  def load(self,fn):
    with open(fn, 'rb') as f:
      return f.read()


e = ELF("/bin/ls")
#e = ELF("forgot")
print hex(e.program_header_table)
print (e.program_header).encode("hex")

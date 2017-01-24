CC=gcc
CFLAGS=-I. -g
DEPS=deserialize.h
OBJ=elf_loader.o deserialize.o 

ODIR=obj

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

elfldr: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(ODIR).*.o *~ cote $(INCDIR)/*~

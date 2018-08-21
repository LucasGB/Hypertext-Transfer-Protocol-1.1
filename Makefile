# Compiler
CC = gcc

# Flags
CFLAGS = -I$(LDIR)

# C files directory
CDIR = src

# Object files directory
ODIR = src/obj

# Library files directory
LDIR = lib

#LIBS=-lm

# Dependency files
_DEPS = url.h connect.h
DEPS = $(patsubst %,$(LDIR)/%,$(_DEPS))

_OBJ = http.o url.o connect.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: $(CDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

http: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ 
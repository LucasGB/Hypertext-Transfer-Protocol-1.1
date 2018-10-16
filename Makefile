# Compiler
CC = gcc

# Flags
CFLAGS = -I$(LDIR) -lpthread

# C files directory
CDIR = src

# Object files directory
ODIR = src/obj

# Library files directory
LDIR = lib

#LIBS=-lm

# Dependency files
_DEPS = parser.h tlpi_hdr.h error_functions.h get_num.h connect.h errors.h b64.h
DEPS = $(patsubst %,$(LDIR)/%,$(_DEPS))

_OBJ = http.o parser.o connect.o errors.o decode.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: $(CDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

http: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~
# Compiler
CC = gcc

# Flags
CFLAGS = -I$(LDIR) -lpthread -lm

# C files directory
CDIR = src

# Object files directory
ODIR = src/obj

# Library files directory
LDIR = lib

# CGI executables directory
CGIDIR = root/cgi

#LIBS=-lm

# Dependency files
_DEPS = parser.h error_functions.h connect.h errors.h b64.h utils.h embedded_c_compiler.h cgi_module.h
DEPS = $(patsubst %,$(LDIR)/%,$(_DEPS))

_OBJ = http.o parser.o  errors.o decode.o utils.o embedded_c_compiler.o cgi_module.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

_CGI = age_horoscope_calculator.o
CGI = $(patsubst %,$(CGIDIR)/%,$(_CGI))

$(ODIR)/%.o: $(CDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(CGIDIR)/%.o:
	$(CC) -c $(CDIR)/age_horoscope_calculator.c -o $(CGIDIR)/age_horoscope_calculator.o -lm

http: $(OBJ) #$(CGI)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~
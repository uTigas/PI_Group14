OBJDIR=./obj
IDIR=../include
LDIR=../lib
IDIR2=../eigen/Eigen
IDIR3=../cryptopp850
CC=g++ -std=c++17 -Wall
CFLAGS=-I $(IDIR) -I $(IDIR2) -I $(IDIR3) -Wall -g -w
CRYPTOFLAGS=-L/usr/lib/ -lcryptopp -lpthread
# Na linha CFLAGS=-I $(IDIR) -I $(IDIR2) -I $(IDIR3) -Wall -g -w
# Adicionar -O3 e remover o -Wall -g para fazer teste de velocidade

all: qkd_emulator

# we define 'DEPS' and '_OBJ' variables to avoid repeation (section 2.4 at https://www.gnu.org/software/make/manual/make.html#Phony-Targets)
DEPS = load_ascii_20200819.h save_ascii_20200819.h binary_source_20200819.h netxpto_20200819.h add_20200819.h ms_windows_console_output_common_20200819.h

# 'patsubst' is a function that modifies the file name (section 8.2 at https://www.gnu.org/software/make/manual/make.html#Phony-Targets)
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = qkd_emulator.o load_ascii_20200819.o save_ascii_20200819.o binary_source_20200819.o netxpto_20200819.o add_20200819.o ms_windows_console_output_common_20200819.o

OBJ = $(patsubst %,$(OBJDIR)/%,$(_OBJ))

# '$<' and '$@' and '$^' are Automatic Variables that help to run a pattern rule (section 10.5.3 at https://www.gnu.org/software/make/manual/make.html#Phony-Targets). see section 4.5.4 also.
# '$<' refers to the name of the first prerequisite.
# '$@' refers to the file name of the target of the rule.
# '$^' refers to the names of all the prerequisites, with spaces between them.

$(OBJDIR)/%.o: $(LDIR)/%.cpp $(_DEPS) 
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/qkd_emulator.o: qkd_emulator.cpp 
	$(CC) $(CFLAGS) -c $< -o $@

qkd_emulator: $(OBJ) 
	$(CC) $(CFLAGS) -o $@ $^ $(CRYPTOFLAGS)

#create a directory named 'obj', if required, before creating the objects (section 4.3 at https://www.gnu.org/software/make/manual/make.html#Bugs)
$(OBJ): | $(OBJDIR)
$(OBJDIR):
	mkdir $(OBJDIR)

#run clean command, even if there is a file named clean in the directory (section 4.6 at https://www.gnu.org/software/make/manual/make.html#Phony-Targets)
.PHONY: clean

clean:
	rm -rf $(OBJDIR) qkd_emulator



 

OBJDIR=./obj
IDIR=../include
LDIR=../lib
IDIR2=../eigen/Eigen
CC=g++ -std=c++17 -Wall
CFLAGS=-I $(IDIR) -I $(IDIR2) -Wall -g -Wall

all: recon_emul
DEPS = convert_b64.h key_generator.h ms_windows_console_output_common_20200819.h netxpto_20200819.h save_ascii_2024.h

DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = recon_emul.o convert_b64.o key_generator.o ms_windows_console_output_common_20200819.o netxpto_20200819.o save_ascii_2024.o

OBJ = $(patsubst %,$(OBJDIR)/%,$(_OBJ))

$(OBJDIR)/%.o: $(LDIR)/%.cpp $(_DEPS) 
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/recon_emul.o: recon_emul.cpp cv_qokd_ldpc_multi_machine_sdf.h
	$(CC) $(CFLAGS) -c $< -o $@

recon_emul: $(OBJ) 
	$(CC) $(CFLAGS) -o $@ $^

$(OBJ): | $(OBJDIR)
$(OBJDIR):
	mkdir $(OBJDIR)

.PHONY: clean

clean:
	rm -rf $(OBJDIR) recon_emul

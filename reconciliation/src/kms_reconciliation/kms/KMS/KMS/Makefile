OBJDIR=./obj
IDIR=../include
LDIR=../lib
IDIR2=../eigen/Eigen
CC=g++ -std=c++17 -Wall
CFLAGS=-I $(IDIR) -I $(IDIR2) -Wall -g -Wall

all: key_provider
DEPS = etsi_qkd_004.h ETSI004_kps.h ip_tunnel_ms_windows_20200819.h json.hpp load_ascii_2024.h message_handler.h ms_windows_console_output_common_20200819.h netxpto_20200819.h save_ascii_2024.h

DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = key_provider.o etsi_qkd_004.o ETSI004_kps.o ip_tunnel_ms_windows_20200819.o load_ascii_2024.o message_handler.o ms_windows_console_output_common_20200819.o netxpto_20200819.o save_ascii_2024.o

OBJ = $(patsubst %,$(OBJDIR)/%,$(_OBJ))

$(OBJDIR)/%.o: $(LDIR)/%.cpp $(_DEPS) 
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/key_provider.o: keyProviderServer.cpp cv_qokd_ldpc_multi_machine_sdf.h
	$(CC) $(CFLAGS) -c $< -o $@

key_provider: $(OBJ) 
	$(CC) $(CFLAGS) -o $@ $^

$(OBJ): | $(OBJDIR)
$(OBJDIR):
	mkdir $(OBJDIR)

.PHONY: clean

clean:
	rm -rf $(OBJDIR) key_provider

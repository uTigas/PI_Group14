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

all: emulator

# we define 'DEPS' and '_OBJ' variables to avoid repeation (section 2.4 at https://www.gnu.org/software/make/manual/make.html#Phony-Targets)
DEPS = ms_windows_network_aux_common_20200819.h fork_20200819.h dv_qkd_message_processor_common_20200819.h very_big_counter_20200819.h dv_qkd_ldpc_rx_parameter_estimation_20200819.h dv_qkd_ldpc_tx_parameter_estimation_20200819.h dv_qkd_privacy_amplification_20200819.h load_ascii_20200819.h save_ascii_20200819.h bit_error_rate_20200819.h cv_qokd_ldpc_tx_20200819.h cv_qokd_ldpc_rx_20200819.h ms_windows_console_output_common_20200819.h dv_qkd_polarization_physical_layer_20200819.h binary_source_20200819.h ip_tunnel_ms_windows_20200819.h netxpto_20200819.h sink_20200819.h dv_qkd_ldpc_rx_message_processor_receiver_20200819.h dv_qkd_ldpc_rx_message_processor_transmitter_20200819.h dv_qkd_ldpc_tx_message_processor_receiver_20200819.h dv_qkd_ldpc_tx_message_processor_transmitter_20200819.h  dv_qkd_rx_bases_reconciliation_20200819.h dv_qkd_tx_bases_reconciliation_20200819.h dv_qkd_rx_incoming_data_processor_20200819.h cv_qokd_ldpc_tx_sindrome_reconciliation_20200819.h cv_qokd_ldpc_rx_sindrome_reconciliation_20200819.h message_handler.h add_20200819.h

# 'patsubst' is a function that modifies the file name (section 8.2 at https://www.gnu.org/software/make/manual/make.html#Phony-Targets)
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = emulator.o ms_windows_network_aux_common_20200819.o fork_20200819.o dv_qkd_message_processor_common_20200819.o very_big_counter_20200819.o dv_qkd_ldpc_rx_parameter_estimation_20200819.o dv_qkd_ldpc_tx_parameter_estimation_20200819.o dv_qkd_privacy_amplification_20200819.o load_ascii_20200819.o save_ascii_20200819.o bit_error_rate_20200819.o cv_qokd_ldpc_tx_20200819.o cv_qokd_ldpc_rx_20200819.o ms_windows_console_output_common_20200819.o dv_qkd_polarization_physical_layer_20200819.o binary_source_20200819.o ip_tunnel_ms_windows_20200819.o netxpto_20200819.o sink_20200819.o dv_qkd_ldpc_rx_message_processor_receiver_20200819.o dv_qkd_ldpc_rx_message_processor_transmitter_20200819.o dv_qkd_ldpc_tx_message_processor_receiver_20200819.o dv_qkd_ldpc_tx_message_processor_transmitter_20200819.o  dv_qkd_rx_bases_reconciliation_20200819.o dv_qkd_tx_bases_reconciliation_20200819.o dv_qkd_rx_incoming_data_processor_20200819.o cv_qokd_ldpc_tx_sindrome_reconciliation_20200819.o cv_qokd_ldpc_rx_sindrome_reconciliation_20200819.o message_handler.o add_20200819.o

OBJ = $(patsubst %,$(OBJDIR)/%,$(_OBJ))

# '$<' and '$@' and '$^' are Automatic Variables that help to run a pattern rule (section 10.5.3 at https://www.gnu.org/software/make/manual/make.html#Phony-Targets). see section 4.5.4 also.
# '$<' refers to the name of the first prerequisite.
# '$@' refers to the file name of the target of the rule.
# '$^' refers to the names of all the prerequisites, with spaces between them.

$(OBJDIR)/%.o: $(LDIR)/%.cpp $(_DEPS) 
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/emulator.o: emulator.cpp 
	$(CC) $(CFLAGS) -c $< -o $@

emulator: $(OBJ) 
	$(CC) $(CFLAGS) -o $@ $^ $(CRYPTOFLAGS)

#create a directory named 'obj', if required, before creating the objects (section 4.3 at https://www.gnu.org/software/make/manual/make.html#Bugs)
$(OBJ): | $(OBJDIR)
$(OBJDIR):
	mkdir $(OBJDIR)

#run clean command, even if there is a file named clean in the directory (section 4.6 at https://www.gnu.org/software/make/manual/make.html#Phony-Targets)
.PHONY: clean

clean:
	rm -rf $(OBJDIR) emulator



 

# Define variables
MAKEFILE_TX="Makefile_tx"
MAKEFILE_RX="Makefile_rx" 
RX_EXECUTABLE="cv_qokd_ldpc_rx_linux"
TX_EXECUTABLE="cv_qokd_ldpc_tx_linux"

# Delete old executables before compiling new ones
[ -f "$TX_EXECUTABLE" ] && rm "$TX_EXECUTABLE"
[ -f "$RX_EXECUTABLE" ] && rm "$RX_EXECUTABLE"

# Compile Tx 
make -f $MAKEFILE_TX $TX_EXECUTABLE
# Compile Rx
make -f $MAKEFILE_RX $RX_EXECUTABLE

if [ -f "$TX_EXECUTABLE" -a -f "$RX_EXECUTABLE" ]; then
    # Execute TX in terminal
    gnome-terminal -- ./"$TX_EXECUTABLE"
    
    sleep 1

    # Execute RX in terminal
    gnome-terminal -- ./"$RX_EXECUTABLE"
else 
    echo "Error during compilation. Check the make output for more details.\n"
fi


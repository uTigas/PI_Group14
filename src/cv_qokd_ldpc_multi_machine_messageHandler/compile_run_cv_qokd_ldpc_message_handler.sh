# Define variables
MAKEFILE_TX="Makefile_tx"
MAKEFILE_RX="Makefile_rx" 
RX_EXECUTABLE="cv_qokd_ldpc_rx_linux"
TX_EXECUTABLE="cv_qokd_ldpc_tx_linux"
CONFIG_FILE_TX="input_parameters_tx.txt"
CONFIG_FILE_RX="input_parameters_rx.txt"

# Delete old executables before compiling new ones
[ -f "$TX_EXECUTABLE" ] && rm "$TX_EXECUTABLE"
[ -f "$RX_EXECUTABLE" ] && rm "$RX_EXECUTABLE"

# Ask to user define if program uses Oblivious Keys (0) or Symmetric Keys (1)
echo "What key type do you want to use?"
echo "0 - Oblivious Keys"
echo "1 - Symmetric Keys"
read user_input

# Validate the user input value
if [ "$user_input" == "0" ] || [ "$user_input" == "1" ]; then
    # Change the keyType in input_parameters config files
    sed -i "4s/keyType=.*/keyType=$user_input/" "$CONFIG_FILE_TX"
    sed -i "4s/keyType=.*/keyType=$user_input/" "$CONFIG_FILE_RX"

    # Wait 1.5 sec
        sleep 1.5

    # Compile TX and RX
    make -f $MAKEFILE_TX $TX_EXECUTABLE
    make -f $MAKEFILE_RX $RX_EXECUTABLE

    # Verify if the file was well compiled
    if [ -f "$TX_EXECUTABLE" ]; then
        # Execute TX in terminal
        gnome-terminal -- ./"$TX_EXECUTABLE"

        # wait 1.5 sec
            sleep 1.5

        # Verify if the file was well compiled
        if [ -f "$RX_EXECUTABLE" ]; then
            # Execute RX in terminal
            gnome-terminal -- ./"$RX_EXECUTABLE";
        else
            echo "Error during $RX_EXECUTABLE compiling. Check the make output for more details."
        fi
    else
        echo "Error during $TX_EXECUTABLE compiling. Check the make output for more details."
    fi
else
    echo "Invalid value. Use 0 for Oblivious Keys or 1 for Symmetric Keys."
fi
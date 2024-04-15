# Get the PID of the parent terminal
parent_terminal_pid=$(ps -o ppid= -p $$)

# Define variables
MAKEFILE_TX="Makefile_tx"
MAKEFILE_RX="Makefile_rx" 
RX_EXECUTABLE="cv_qokd_ldpc_rx_linux"
TX_EXECUTABLE="cv_qokd_ldpc_tx_linux"
CONFIG_FILE_TX="input_parameters_tx.txt"
CONFIG_FILE_RX="input_parameters_rx.txt"
MATRIX1="sparseParityCheckMatrix_n1944_k972_nonZeros6885.dat"
MATRIX2="sparseParityCheckMatrix_n10000_k200_nonZeros32800.dat"
VALUE1A="1944"
VALUE1B="972"
VALUE2A="10000"
VALUE2B="200"

# Delete old executables before compiling new ones
[ -f "$TX_EXECUTABLE" ] && rm "$TX_EXECUTABLE"
[ -f "$RX_EXECUTABLE" ] && rm "$RX_EXECUTABLE"
[ -f "server_tx" ] && rm "server_tx"
[ -f "server_rx" ] && rm "server_rx"
[ -f "client_tx" ] && rm "client_tx"
[ -f "client_rx" ] && rm "client_rx"
# [ -f "tx_raw_new0.dat" ] && rm "tx_raw_new0.dat"
# [ -f "tx_raw_new1.dat" ] && rm "tx_raw_new1.dat"
# [ -f "tx_raw_new2.dat" ] && rm "tx_raw_new2.dat"
# [ -f "tx_raw_new3.dat" ] && rm "tx_raw_new3.dat"
# [ -f "tx_raw_new4.dat" ] && rm "tx_raw_new4.dat"
# [ -f "rx_raw_new0.dat" ] && rm "rx_raw_new0.dat"
# [ -f "rx_raw_new1.dat" ] && rm "rx_raw_new1.dat"
# [ -f "rx_raw_new2.dat" ] && rm "rx_raw_new2.dat"
# [ -f "rx_raw_new3.dat" ] && rm "rx_raw_new3.dat"
# [ -f "rx_raw_new4.dat" ] && rm "rx_raw_new4.dat"

# Ask to user define if program uses Oblivious Keys (0) or Symmetric Keys (1)
echo "What key type do you want to use?"
echo "0 - Oblivious Keys"
echo "1 - Symmetric Keys"
echo
echo "Introduce the number of your choice (0 or 1):"
read user_input_keyType

# Validate the user input 1 value
if [ "$user_input_keyType" != "0" ] && [ "$user_input_keyType" != "1" ]; then
    echo "Invalid value. Use 0 for Oblivious Keys or 1 for Symmetric Keys."
else
    # Ask to user what LDPC matrix he wants to use
    echo
    echo
    echo "What LDPC Matrix do you want to use for correction method:"
    echo "1 - 1944x972 Matrix (sparseParityCheckMatrix_n1944_k972_nonZeros6885)"
    echo "2 - 10000x200 Matrix (sparseParityCheckMatrix_n10000_k200_nonZeros32800)"
    echo
    echo "Introduce the number of your choice (1 or 2):"
    read matrix_choice
    echo

    if [ "$matrix_choice" == "1" ]; then
        matrix=$MATRIX1
        value_a=$VALUE1A
        value_b=$VALUE1B
    elif [ "$matrix_choice" == "2" ]; then
        matrix=$MATRIX2
        value_a=$VALUE2A
        value_b=$VALUE2B
    else
        echo "Invalid choice. Using default LDPC Matrix (1944x972)."
        echo
        matrix=$MATRIX1
        value_a=$VALUE1A
        value_b=$VALUE1B
    fi

    # Change the keyType in input_parameters config files
    sed -i "4s/keyType=.*/keyType=$user_input_keyType/" "$CONFIG_FILE_TX"
    sed -i "4s/keyType=.*/keyType=$user_input_keyType/" "$CONFIG_FILE_RX"

    # Change the LDPC Matrix in input_parameters config files
    sed -i "30s/parityCheckMatrixFileName=.*/parityCheckMatrixFileName=$matrix/" "$CONFIG_FILE_TX"
    sed -i "40s/parityCheckMatrixFileName=.*/parityCheckMatrixFileName=$matrix/" "$CONFIG_FILE_RX"

    
    # Compile TX and Rx server and client files
    g++ server_tx.cpp -o server_tx
    g++ server_rx.cpp -o server_rx
    g++ client_tx.cpp -o client_tx
    g++ client_rx.cpp -o client_rx
    
    # Wait 0.5 sec
    sleep 0.5

    # Verify if the file was well compiled
    if [ -f "server_tx" -a -f "server_rx" -a -f "client_tx" -a -f "client_rx" ]; then

        gnome-terminal -- ./server_tx
        sleep 0.25
        gnome-terminal -- ./server_rx
        sleep 0.25
        gnome-terminal -- ./client_tx
        sleep 0.25
        gnome-terminal -- ./client_rx
        sleep 0.25
        
        # Wait 5 sec for the servers to start and transfer initial Raw Data
        sleep 5

        # Compile Tx
        make -f $MAKEFILE_TX $TX_EXECUTABLE
        
        # Wait 1.5 sec
        sleep 1.5

        if [ -f "$TX_EXECUTABLE" ]; then

            # Execute TX in terminal
            gnome-terminal -- ./"$TX_EXECUTABLE"

            # Compile Tx
            make -f $MAKEFILE_RX $RX_EXECUTABLE

            # Wait 0.5 sec
            sleep 1.5

            if [ -f "$RX_EXECUTABLE" ]; then
                
                # Execute RX in terminal
                gnome-terminal -- ./"$RX_EXECUTABLE"

            else
                echo "Error during $RX_EXECUTABLE compiling. Check the make output for more details."
                echo
            fi
        else
            echo "Error during $TX_EXECUTABLE compiling. Check the make output for more details."
            echo
        fi
    else
        echo "Error during Server/Client compiling. Check the files for more details."
        echo
    fi
fi

# Kill the parent terminal
kill "$parent_terminal_pid"
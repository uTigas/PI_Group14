EMULATOR_EXEC="emulator/qkd_emulator"
RX_EXEC="cv_qokd_ldpc_rx_linux"
TX_EXEC="cv_qokd_ldpc_tx_linux"

gnome-terminal -- ./"$EMULATOR_EXEC"

sleep 5.5

gnome-terminal -- ./server_tx
sleep 0.25
gnome-terminal -- ./server_rx
sleep 0.25
gnome-terminal -- ./client_tx
sleep 0.25
gnome-terminal -- ./client_rx
sleep 0.25

gnome-terminal -- ./"$RX_EXEC"
gnome-terminal -- ./"$TX_EXEC"
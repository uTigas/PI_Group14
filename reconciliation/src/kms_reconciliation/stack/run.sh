EMULATOR_EXEC="emulator/qkd_emulator"
RX_EXEC="cv_qokd_ldpc_rx_linux"
TX_EXEC="cv_qokd_ldpc_tx_linux"
KPS_EXEC="key_provider"
KMS_EXEC="kms"
APPA_EXEC="appA"

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

gnome-terminal -- ./"$TX_EXEC"
gnome-terminal -- bash -c ./"$RX_EXEC; exec bash"

sleep 120

gnome-terminal -- ./"$KPS_EXEC"
gnome-terminal -- ./"$KMS_EXEC"
gnome-terminal -- bash -c "./$APPA_EXEC; exec bash"


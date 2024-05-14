RX_EXEC="cv_qokd_ldpc_rx_linux"
TX_EXEC="cv_qokd_ldpc_tx_linux"
KPS_EXEC="key_provider"
KMS_EXEC="kms"
APPA_EXEC="appA"

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

sleep 120

gnome-terminal -- ./"$KPS_EXEC"
gnome-terminal -- ./"$KMS_EXEC"
gnome-terminal -- bash -c "./$APPA_EXEC; exec bash"


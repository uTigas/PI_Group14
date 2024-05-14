RX_EXEC="cv_qokd_ldpc_rx_linux"
TX_EXEC="cv_qokd_ldpc_tx_linux"
KPS_EXEC="key_provider"
KMS_EXEC="kms"
APPA_EXEC="appA"

gnome-terminal -- ./"$RX_EXEC"
gnome-terminal -- ./"$TX_EXEC"
sleep 1.5
gnome-terminal -- ./"$KPS_EXEC"
gnome-terminal -- ./"$KMS_EXEC"
gnome-terminal -- bash -c "./$APPA_EXEC; exec bash"


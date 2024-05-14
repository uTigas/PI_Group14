KPS_EXEC="key_provider"
KMS_EXEC="kms"
APPA_EXEC="appA"

gnome-terminal -- ./"$KMS_EXEC"
gnome-terminal -- ./"$KPS_EXEC"
gnome-terminal -- bash -c "./$APPA_EXEC; exec bash"
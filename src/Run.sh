# Define variables
DIRECTORY_MultiMachine_MessageHandler="sdf/cv_qokd_ldpc/cv_qokd_ldpc_multi_machine_messageHandler" 
EXECUTABLE_MultiMachine_MessageHandler="compile_run_cv_qokd_ldpc_message_handler.sh"
parent_terminal_pid=$(ps -o ppid= -p $$)

# Run the script
gnome-terminal -- bash -c "cd $DIRECTORY_MultiMachine_MessageHandler && chmod +x $EXECUTABLE_MultiMachine_MessageHandler && ./$EXECUTABLE_MultiMachine_MessageHandler; exec bash"

kill "$parent_terminal_pid"
# Define variables
DIRECTORY_MultiMachine_MessageHandler="cv_qokd_ldpc_multi_machine_messageHandler"
EXECUTABLE_MultiMachine_MessageHandler="compile_run_cv_qokd_ldpc_message_handler.sh"
parent_terminal_pid=$(ps -o ppid= -p $$)


if [ -n "$GNOME_TERMINAL_SCREEN" ]; then
    TERMINAL=gnome-terminal
elif [ -n "$KONSOLE_DBUS_SERVICE" ]; then
    TERMINAL=konsole
elif [ -n "$XTERM_VERSION" ]; then
    TERMINAL=xterm
else
    echo "Set $TERMINAL variable to your prefered terminal emulator, any problems feel free to edit Run.sh"
fi

# Set your terminal
TERMINAL=gnome-terminal

# Run the script
$TERMINAL -- bash -c "cd $DIRECTORY_MultiMachine_MessageHandler && chmod +x $EXECUTABLE_MultiMachine_MessageHandler && ./$EXECUTABLE_MultiMachine_MessageHandler; exec bash"

kill "$parent_terminal_pid"

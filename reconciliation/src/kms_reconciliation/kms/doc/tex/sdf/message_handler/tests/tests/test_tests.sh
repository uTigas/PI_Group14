#!/bin/bash

# Function to handle Ctrl+C signal
function ctrl_c() {
    echo "Program terminated by user."
    exit 1
}

# Trap the Ctrl+C signal
trap ctrl_c INT

# Initialize counter
counter=0

# Continuous execution loop
while :
do
    # Increment counter
    ((counter++))

    # Run the program
    ./../bin/tests/message_handler_unit_tests

    # Check the termination status
    status=$?

    # Exit the loop if the termination status is not 0
    if [ $status -ne 0 ]; then
        echo "Program terminated with status $status."
        exit $status
    fi

    # Output the number of executions
    echo "Execution: $counter"

    # Wait for a short duration before running again
    sleep 1
done

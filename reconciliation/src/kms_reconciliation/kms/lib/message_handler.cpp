// Import of local libraries
#include "message_handler.h"

/* #########################################################################
   ########################### Priority Buffer #############################
   ######################################################################### */

void PriorityBuffer::insert_sorted(const t_handler_message newMsg)
{

    if (full())
        return;

    // Find the position in the list to insert the new message
    auto msg = begin();
    for (; msg != end() && msg->getPriority() < newMsg.getPriority(); ++msg)
        ;

    // Insert the new message at the appropriate position
    insert(msg, newMsg);
}

bool PriorityBuffer::lowerPriority()
{
    bool decreased = false;
    // Scroll through every message and decrease the priority if possible
    for (auto msg = begin(); msg != end(); ++msg)
    {
        if (msg->getPriority() == 0)
        {
            // The priority can't get any lower
            continue;
        }
        else if (msg->getPriority() == 5)
        {
            // There won't be any more message to decrease priority (max priority reached)
            break;
        }
        else
        {
            // Decrease the priority
            msg->setPriority(msg->getPriority() - 1);
            decreased = true;
        }
    }
    return decreased;
}

bool PriorityBuffer::removeLowPriorityMessages()
{
    overflow = false;
    for (auto msg = begin(); msg != end();)
    {
        if (msg->getPriority() == 0)
        {
            overflow = true;
            // Erase the current message and update the iterator
            msg = erase(msg);
            // Use the logging function in the future
            std::cerr << "Message discarded: " << msg->getRawMessage()
                      << std::endl;
        }
        else
        {
            // When a new priority appears, there won't be any more messages
            // with priority = 0
            break;
        }
    }
    return overflow;
}

/* #########################################################################
   ################### Destination Translation Table  ######################
   ######################################################################### */

t_integer DestinationTranslationTable::translate(t_string destinationBlock)
{
    // If the translation table is empty, send to port o
    if (destinationTable.empty())
        return 0;

    // Get the exit port corresponding to the given destinationBlock
    auto it = destinationTable.find(destinationBlock);
    if (it == destinationTable.end())
        return -1;

    // Return the exit port corresponding to the given destinationBlock
    return it->second;
}

void DestinationTranslationTable::add(t_string destinationBlock,
                                      t_integer exitPort)
{
    // add the exit port to the signalTable
    destinationTable[destinationBlock] = exitPort;
}

void DestinationTranslationTable::remove(t_string destinationBlock)
{
    // remove the exit port from the signalTable
    destinationTable.erase(destinationBlock);
}

/* #########################################################################
   ################### Input Translation Table  ############################
   ######################################################################### */

InputSignalInfo InputTranslationTable::translate(t_integer entryPort)
{
    // The translation table can't be empty
    if (inputTable.empty())
        throw std::runtime_error(
            "The Input Translation Table should be defined, when handling "
            "various signal types");

    // Get the Signal info corresponding to the given entryPOrt
    auto it = inputTable.find(entryPort);
    if (it == inputTable.end())
        throw std::runtime_error(
            "Information received from an unknown port, please configure it "
            "properly");

    // Return the signal info corresponding to the given entryPort
    return it->second;
}

void InputTranslationTable::add(t_integer entryPort,
                                InputSignalInfo signalInfo)
{
    // add the entry port to the signalTable
    inputTable.insert({entryPort, signalInfo});
    // inputTable[entryPort] = signalInfo;
}

void InputTranslationTable::remove(t_integer entryPort)
{
    // remove the exit port from the signal Table
    inputTable.erase(entryPort);
}

void InputTranslationTable::edit(t_integer entryPort, InputSignalInfo signalInfo)
{
    // edit the entryPort entry in the signal table
    inputTable[entryPort] = signalInfo;
}

/* #########################################################################
   ########################### Message Handler #############################
   ######################################################################### */

void MessageHandler::initialize(void)
{
    /* The following code is used in determining the functioning mode
     * and asserting that the system configuration is correct
    */
    bool equalInputSignals = false;
    bool equalOutputSignals = false;

    t_string tempSignal;
    for (int signalIndex = 0; signalIndex < getNumberOfInputSignals();
         signalIndex++)
    {
        if (tempSignal.empty())
            tempSignal = inputSignals[signalIndex]->getType();
        else if (tempSignal == inputSignals[signalIndex]->getType())
            equalInputSignals = true;
        else
        {
            equalInputSignals = false;
            break;
        }
    }

    tempSignal = "";
    for (int signalIndex = 0; signalIndex < getNumberOfOutputSignals();
         signalIndex++)
    {
        if (tempSignal.empty())
            tempSignal = outputSignals[signalIndex]->getType();
        else if (tempSignal == outputSignals[signalIndex]->getType())
            equalOutputSignals = true;
        else
        {
            equalOutputSignals = false;
            break;
        }
    }

    // Auto-detect if functioning in Rx or Tx mode
    if (functioningMode == DETERMINE_FUNCTIONING)
    {

        if (!equalInputSignals && (equalOutputSignals && outputSignals[0]->getValueType() == signal_value_type::t_handler_message))
            functioningMode = FUNCTIONING_AS_TX;
        else if ((equalInputSignals && inputSignals[0]->getValueType() == signal_value_type::t_handler_message) && !equalOutputSignals)
            functioningMode = FUNCTIONING_AS_RX;
        else if ((equalInputSignals && inputSignals[0]->getValueType() == signal_value_type::t_handler_message) && (equalOutputSignals && outputSignals[0]->getValueType() == signal_value_type::t_handler_message))
            functioningMode = INDISTINGUISHABLE;
        else
            throw std::invalid_argument(
                "The system couldn't detect the functioning mode (Rx or Tx) "
                "based on your provided inputSignals Please use the "
                "constructor to explicitly declare the functioning mode or "
                "correct the used signals");
    }

    // Setup needed for Tx mode, operating with various types of input signals
    if (functioningMode == FUNCTIONING_AS_TX && !equalInputSignals)
    {
        if (inputSignalTranslation->size() == 0 && inputSignals.size() > 1)
        {
            throw std::invalid_argument(
                "Operating with various types of input signals in Tx mode "
                "mandates the "
                "use of a Translation Table for the input signals, so that the "
                "Message Handler can determine the name of the Block that is "
                "operating with him. Please use the proper constructor to "
                "initialize this class");
        }
        else if (inputSignalTranslation->size() != getNumberOfInputSignals())
        {
            throw std::invalid_argument(
                "When Translation table is used in the scenario of various "
                "types of input signals, the size of the translation table "
                "must be equal to the number of input signals");
        }
    }

    // Initialize buffers
    // One buffer for each output signal
    for (int i = 0; i < getNumberOfOutputSignals(); i++)
    {
        buffers->push_front(PriorityBuffer{bufferSize});
    }
}

bool MessageHandler::runBlock(void)
{
    bool alive = false;

    // If any message was decremented in any buffer, place alive to true
    for (auto it = buffers->begin(); it != buffers->end(); ++it)
    {
        if (it->lowerPriority())
            alive = true;
    }

    // Check and remove messages from buffers and place in output signals
    send_to_output_signals();

    // Removes messages from Input Signals and places them in
    //  the Internal Buffer, according to their destination block
    receive_from_input_signals();

    // Before leaving repeat the execution, in case a new message arrived
    receive_from_input_signals();

    return alive;
}

//! Used in Tx Mode
t_handler_message MessageHandler::convert_to_message(
    uint32_t InputSignalIndex)
{
    // Discover the core block corresponding to the input signal
    InputSignalInfo signalInfo =
        inputSignalTranslation->translate(InputSignalIndex);

    t_string msgType = inputSignals[InputSignalIndex]->getType();
    std::string messageText = "";

    // Get the data to a vector
    if (inputSignals[InputSignalIndex]->getValueType() == signal_value_type::t_binary)
    {
        std::vector<t_binary> vectorData{};
        t_binary rawData;
        t_integer ready = inputSignals[InputSignalIndex]->ready();
        for (auto k = 0; (k < ready) && (k < MAX_MSG_SIZE); ++k)
        {
            inputSignals[InputSignalIndex]->bufferGet(&rawData);
            vectorData.push_back((t_binary)rawData);
        }

        // Join the data into a string
        for (int w : vectorData)
        {
            messageText.append(std::to_string(w));
        }
    }
    else if (inputSignals[InputSignalIndex]->getValueType() == signal_value_type::t_integer)
    {
        std::vector<t_integer> vectorData{};
        t_integer rawData;
        t_integer ready = inputSignals[InputSignalIndex]->ready();
        for (auto k = 0; (k < ready) && (k < MAX_MSG_SIZE); ++k)
        {
            inputSignals[InputSignalIndex]->bufferGet(&rawData);
            vectorData.push_back((t_integer)rawData);
        }

        // Join the data into a string
        for (int w : vectorData)
        {
            messageText.append(std::to_string(w));
        }
    }
    else if (inputSignals[InputSignalIndex]->getValueType() == signal_value_type::t_message)
    {
        t_message hashMessage;
        inputSignals[InputSignalIndex]->bufferGet(&hashMessage);
        messageText.append(hashMessage.messageRaw);
    }
    // More signal conversion types can be added here
    else
    {
        throw std::runtime_error("The provided input signal doesn't have a specified conversion for message");
    }

    // Create the message
    t_handler_message msg;
    msg.setHeader(signalInfo.destinationBlock, signalInfo.sourceBlock,
                  signalInfo.priority, signalInfo.SeqId++, msgType, 0,
                  std::to_string(messageText.size()));
    msg.setMessageData(messageText);

    // Update sequential ID of the saved data in the InputSignalInfo class
    inputSignalTranslation->edit(InputSignalIndex, signalInfo);
    return msg;
}

//! Used in Rx Mode
void MessageHandler::disconvert_from_message(
    t_handler_message &msg, uint32_t outputSignalIndex)
{

    t_string msgType = msg.getMessageType();

    if (msgType == "Binary")
    {
        for (char ch : msg.getMessageData())
        {
            if (ch < '0' || ch > '9')
            {
                throw std::runtime_error("Invalid character: " + std::to_string(ch) + ", for disconversion of message type: " + msg.getMessageType() + ", with all characters needing to be a number");
            }
            uint8_t num = static_cast<uint8_t>(ch - '0');
            // Insert value in outputSignal
            outputSignals[outputSignalIndex]->bufferPut(num);
        }
    }// The end of the type is cutoff because of message type limit of 35 bytes
    else if (msgType == "TimeDiscreteAmplitudeDiscreteIntege" || msgType == "TimeContinuousAmplitudeDiscreteInte")
    {
        t_string data = msg.getMessageData();
        for (char c : data)
        {
            if (!std::isdigit(c))
            {
                throw std::runtime_error("The provided data: " + data + " can only contain numbers, for message type: " + msg.getMessageType());
            }
        }
        t_integer num = stoi(data);
        // Insert value in outputSignal
        outputSignals[outputSignalIndex]->bufferPut(num);
    }
    else if (msgType == "Message"){
        t_string data = msg.getMessageData();
        t_message m;
        m.messageRaw = data;
        outputSignals[outputSignalIndex]->bufferPut(m);
    }
    // More signal translations can be added here
    else
    {
        throw std::runtime_error("The message type: " + msg.getMessageType() + "doesn't have a specified conversion to signal");
    }
}

void MessageHandler::receive_from_input_signals()
{
    // For all input signals
    for (int signalIndex = 0; signalIndex < getNumberOfInputSignals();
         signalIndex++)
    {
        // Get number of messages available, in the signal, to remove
        t_integer messagesStored = inputSignals[signalIndex]->ready();

        // Repeat execution for every stored message
        while (messagesStored > 0)
        {
            // Remove message from the input signal
            t_handler_message msg;

            // check input signal type and if different than message, call
            //  the function to convert it
            if (inputSignals[signalIndex]->getValueType() ==
                signal_value_type::t_handler_message)
            {
                inputSignals[signalIndex]->bufferGet(&msg);
            }
            else
            {
                msg = convert_to_message(signalIndex);
            }

            // Discover the destination output signal, based on the
            //  destination block
            auto outputBufferId =
                translationTable->translate(msg.getDestinationBlock());

            // If the output signal is not found, error
            if (outputBufferId == -1)
                throw std::runtime_error("A message with Destination Block:" +
                                         msg.getDestinationBlock() +
                                         ", which doesn't exist");

            // Select the buffer associated to the output signals
            auto outputBuffer = buffers->begin();
            std::advance(outputBuffer, outputBufferId);

            // Check if the output signal has sufficient space
            if (outputBuffer->freeSpace() < messagesStored)
                // Remove messages with priority = 0
                outputBuffer->removeLowPriorityMessages();

            // In case the buffer remains full, stop here
            if (outputBuffer->full())
                break;

            // place message in buffer and specify the source port sent to
            msg.setSourcePort(outputBufferId);
            outputBuffer->insert_sorted(msg);

            // Message was removed, update available messages
            messagesStored = inputSignals[signalIndex]->ready();
            messagesStored--;
        }
    }
}

void MessageHandler::send_to_output_signals()
{
    // For all output signals
    for (int signalIndex = 0; signalIndex < getNumberOfOutputSignals();
         signalIndex++)
    {
        // Get the number of available space, in the signal
        t_integer availableSpace = outputSignals[signalIndex]->space();

        // Select the buffer associated to the output signals
        auto signalInternalBuffer = buffers->begin();
        std::advance(signalInternalBuffer, signalIndex);

        // Remove the messages from the buffer and place them in the output signals
        while (!signalInternalBuffer->empty() && availableSpace > 0)
        {
            t_handler_message msg = signalInternalBuffer->back();
            // Check if the output signal is different that message and call
            //  the function to translate it to the desired type
            if (outputSignals[signalIndex]->getValueType() == signal_value_type::t_handler_message)
                outputSignals[signalIndex]->bufferPut(msg);
            else
                disconvert_from_message(msg, signalIndex);
            signalInternalBuffer->pop_back();
            availableSpace--;
        }
    }
}
// Note: Comments in this file are mainly written according to the C++ Qt Style
#pragma once

// Import of standard or provided libraries
#include <queue>

// Local Imports
#include "netxpto_20200819.h"

/*!
    Class used to hold messages waiting to be delivered according to their
    destination, in the Message Handler.

    It's implementation is based on a Linked List
*/
class PriorityBuffer : public std::list<t_handler_message>
{
public:
    PriorityBuffer(uint32_t maxSize) { maxQueueSize = maxSize; }

    void insert_sorted(const t_handler_message newMsg);

    bool full() const { return size() == maxQueueSize; }

    bool lowerPriority();

    bool removeLowPriorityMessages();

    bool getOverflowFlag() const { return overflow; }
    uint32_t getMaxSize() const { return maxQueueSize; }
    uint32_t freeSpace() const { return getMaxSize() - size(); }

private:
    uint32_t maxQueueSize;
    bool overflow = false;
};

/* ------------------------------------------------------------------------- */

/*!
    This class is used by the Message Handler to determine where to send a
    message, given it's destination block
*/
class DestinationTranslationTable
{
public:
    DestinationTranslationTable(){};

    DestinationTranslationTable(std::map<t_string, t_integer> table)
    {
        destinationTable = table;
    };

    t_integer translate(t_string destinationBlock);

    void add(t_string destinationBlock, t_integer exitPort);

    void remove(t_string destinationBlock);

    t_integer size() const { return destinationTable.size(); }

private:
    std::map<t_string, t_integer> destinationTable;
};

/* ------------------------------------------------------------------------- */
/*!
    This class is used in conjunction with the InputTranslationTable class, to
    hold the information about the connected signals, when operating with signals with
    type different than HandlerMessage.
*/
struct InputSignalInfo
{
    t_string destinationBlock;
    t_string sourceBlock;
    uint8_t priority = 5; // default value
    uint8_t SeqId = 0;    // default value
};

/*!
    This class is used by the Message Handler (in Tx mode) to aid in constructing
    a message, when it's connected to signals of type different than t_handler_message
    (which don't provide the same metadata as this type of messages do)
*/
class InputTranslationTable
{
public:
    InputTranslationTable(){};

    InputTranslationTable(std::map<t_integer, InputSignalInfo> table)
    {
        inputTable = table;
    };

    InputSignalInfo translate(t_integer entryPort);

    void add(t_integer entryPort, InputSignalInfo signalInfo);

    void remove(t_integer entryPort);

    void edit(t_integer entryPort, InputSignalInfo signalInfo);

    t_integer size() const { return inputTable.size(); }

private:
    std::map<t_integer, InputSignalInfo> inputTable;
};

/* ------------------------------------------------------------------------- */

#define FUNCTIONING_AS_TX 1
#define FUNCTIONING_AS_RX 2
#define DETERMINE_FUNCTIONING 3
#define INDISTINGUISHABLE 4

/*!
    This is the class that operates and controls the Message Handler.

    It's implementation is based on the Block class.

    It operates synchronously and executes the operations listed in runBlock
    function, each iteration.
*/
class MessageHandler : public Block
{
public:
    /*!
        Initialization, without translation tables (Only supports one output signal).
        Automatic detection of operating mode
    */
    MessageHandler(std::initializer_list<Signal *> InputSig,
                   std::initializer_list<Signal *> OutputSig)
        : Block(InputSig, OutputSig),
          translationTable(std::make_shared<DestinationTranslationTable>()),
          buffers(std::make_shared<std::list<PriorityBuffer>>()),
          inputSignalTranslation(std::make_shared<InputTranslationTable>()){};

    /*!
        Initialization only with mandatory equal Input Signals.
        Automatic detection of operating mode
    */
    MessageHandler(std::initializer_list<Signal *> InputSig,
                   std::initializer_list<Signal *> OutputSig,
                   DestinationTranslationTable tTable)
        : Block(InputSig, OutputSig),
          translationTable(
              std::make_shared<DestinationTranslationTable>(tTable)),
          buffers(std::make_shared<std::list<PriorityBuffer>>()),
          inputSignalTranslation(std::make_shared<InputTranslationTable>()){};

    /*!
        Initialization only with mandatory equal Input Signals.
        Explicit declaration of operating mode
    */
    MessageHandler(std::initializer_list<Signal *> InputSig,
                   std::initializer_list<Signal *> OutputSig,
                   DestinationTranslationTable tTable, uint8_t mode)
        : Block(InputSig, OutputSig),
          translationTable(
              std::make_shared<DestinationTranslationTable>(tTable)),
          buffers(std::make_shared<std::list<PriorityBuffer>>()),
          inputSignalTranslation(std::make_shared<InputTranslationTable>()),
          functioningMode(mode){};

    /*!
        Initialization with equal Input Signals and unequal Output Signals.
        Explicit declaration of operating mode (generally Rx mode)
    */
    MessageHandler(std::initializer_list<Signal *> InputSig,
                   std::initializer_list<Signal *> OutputSig, uint8_t mode,
                   InputTranslationTable inputDescription)
        : Block(InputSig, OutputSig),
          translationTable(std::make_shared<DestinationTranslationTable>()),
          buffers(std::make_shared<std::list<PriorityBuffer>>()),
          inputSignalTranslation(
              std::make_shared<InputTranslationTable>(inputDescription)),
          functioningMode(mode){};

    /*!
        Initialization with equal or unequal input and output signals.
        (Used in Tx mode)
        Explicit declaration of operating mode
    */
    MessageHandler(std::initializer_list<Signal *> InputSig,
                   std::initializer_list<Signal *> OutputSig,
                   DestinationTranslationTable tTable, uint8_t mode,
                   InputTranslationTable inputDescription)
        : Block(InputSig, OutputSig),
          translationTable(
              std::make_shared<DestinationTranslationTable>(tTable)),
          buffers(std::make_shared<std::list<PriorityBuffer>>()),
          inputSignalTranslation(
              std::make_shared<InputTranslationTable>(inputDescription)),
          functioningMode(mode){};

    void initialize(void);

    bool runBlock(void);

    void setBufferSize(uint32_t size) { bufferSize = size; };

    uint32_t getBufferSize() { return bufferSize; };

    uint8_t getFunctioningMode() { return functioningMode; };

    std::shared_ptr<std::list<PriorityBuffer>> getBuffers() const
    {
        return buffers;
    };

    std::shared_ptr<DestinationTranslationTable> getTranslationTable() const
    {
        return translationTable;
    };
    
    /*! Get information from input signals not operating with t_handler_message type
     *  and convert to it
     */
    t_handler_message convert_to_message(uint32_t InputSignalIndex);

    /*! Use a t_handler_message and transform it to the specific type present in the
     *  messageType field
     */
    void disconvert_from_message(t_handler_message &msg, uint32_t outputSignalIndex);

    void receive_from_input_signals();

    void send_to_output_signals();

    uint32_t bufferSize = 100;
    std::shared_ptr<std::list<PriorityBuffer>> buffers;
    std::shared_ptr<DestinationTranslationTable> translationTable;
    std::shared_ptr<InputTranslationTable> inputSignalTranslation;
    uint8_t functioningMode = DETERMINE_FUNCTIONING;
    
};
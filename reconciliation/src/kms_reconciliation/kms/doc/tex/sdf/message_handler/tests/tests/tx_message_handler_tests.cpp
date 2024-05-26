// Import of standard or provided libraries
#include <gtest/gtest.h>

// Import of local libraries
#include "message_handler.h"

/*!
 * Tests for the MessageHandler class, working in Tx mode
*/

MessageHandler* TxSetUpDummy() {
    Signal::t_write_mode sWriteMode{Signal::t_write_mode::Ascii};
    Signal::t_header_type hType{Signal::t_header_type::noHeader};

    // Attention to this code: The HandlerMessage signal will be written in the stack,
    // so this signal won't be accessible after this function scope
    HandlerMessage coreBlock1{"coreBlock1.sgn", 10, hType, sWriteMode};
    HandlerMessage coreBlock2{"coreBlock2.sgn", 10, hType, sWriteMode};

    HandlerMessage exitPort1{"exitPort1.sgn", 10, hType, sWriteMode};
    HandlerMessage exitPort2{"exitPort2.sgn", 10, hType, sWriteMode};

    return new MessageHandler({&coreBlock1, &coreBlock2},
                              {&exitPort1, &exitPort2});
};

TEST(MessageHandler, TxTestClassCreation) {
    MessageHandler* msgHandler = TxSetUpDummy();

    // Check if the number of core blocks is correct
    EXPECT_EQ(msgHandler->getNumberOfInputSignals(), 2);

    // Check if the number of exit ports is correct
    EXPECT_EQ(msgHandler->getNumberOfOutputSignals(), 2);
};

TEST(MessageHandler, TxTestInitialize) {

    Signal::t_write_mode sWriteMode{Signal::t_write_mode::Ascii};
    Signal::t_header_type hType{Signal::t_header_type::noHeader};

    HandlerMessage coreBlock1{"coreBlock1.sgn", 10, hType, sWriteMode};
    HandlerMessage coreBlock2{"coreBlock2.sgn", 10, hType, sWriteMode};

    HandlerMessage exitPort1{"exitPort1.sgn", 10, hType, sWriteMode};
    HandlerMessage exitPort2{"exitPort2.sgn", 10, hType, sWriteMode};

    auto* msgHandler = new MessageHandler({&coreBlock1, &coreBlock2},
                                          {&exitPort1, &exitPort2});

    // Run initialize function
    msgHandler->initialize();

    // Check if number of buffers is correct
    EXPECT_EQ(msgHandler->getBuffers()->size(), 2);

    // Get default initialization size
    uint32_t bufferSize = msgHandler->getBufferSize();

    // Check if the buffers are initialized and correct size
    for (const auto& buffer : *msgHandler->getBuffers())
        EXPECT_EQ(buffer.getMaxSize(), bufferSize);
};

TEST(MessageHandler, TxTestRunBlockReceive) {
    /* Code that inserts messages in signals from core block, for the Tx Message 
     * Handler to read and insert them in it's buffers. This test will only 
     * output to one exit port, so no need to have a Translation Table 
     */

    Signal::t_write_mode sWriteMode{Signal::t_write_mode::Ascii};
    Signal::t_header_type hType{Signal::t_header_type::noHeader};

    HandlerMessage coreBlock1{"coreBlock1.sgn", 10, hType, sWriteMode};
    HandlerMessage coreBlock2{"coreBlock2.sgn", 10, hType, sWriteMode};

    HandlerMessage exitPort1{"exitPort1.sgn", 10, hType, sWriteMode};
    HandlerMessage exitPort2{"exitPort2.sgn", 10, hType, sWriteMode};

    auto* msgHandler = new MessageHandler({&coreBlock1, &coreBlock2},
                                          {&exitPort1, &exitPort2});

    // Run initialize function
    msgHandler->initialize();

    // Assess functioning mode
    EXPECT_EQ(msgHandler->getFunctioningMode(), INDISTINGUISHABLE);

    // Insert data in Core Block signals
    // Default parameters
    t_string destinationBlock = "dstBlock", sourceBlock = "srcBlock",
             messageType = "1212";
    uint32_t Id = 1;

    // Dummy value of sourcePort, since it should be replaced
    uint8_t sourcePort = 9, priority = 5;
    t_string messageData = "Hello to the World!";

    for (auto signal : msgHandler->inputSignals) {
        // Check signal values
        EXPECT_EQ(signal->ready(), 0);
        EXPECT_EQ(signal->space(), 10);

        // Create message
        t_handler_message msg;
        msg.setHeader(destinationBlock, sourceBlock, priority, Id++,
                      messageType, sourcePort,
                      std::to_string(messageData.length()));
        msg.setMessageData(messageData);

        // Insert message in signal
        signal->bufferPut(msg);
    }

    // Check if the buffers are empty
    for (const auto& buffer : *msgHandler->getBuffers())
        EXPECT_EQ(buffer.size(), 0);

    // Run the block
    msgHandler->runBlock();

    // Check if the core blocks are empty
    for (auto signal : msgHandler->inputSignals)
        EXPECT_EQ(signal->ready(), 0);

    // Check if the buffer from exit port 0 has the messages
    EXPECT_EQ(msgHandler->getBuffers()->front().size(), 2);

    Id = 2;
    // Check if the messages are correct (mainly source port = 0)
    for (auto msg : msgHandler->getBuffers()->front()) {
        EXPECT_EQ(msg.getSourcePort(), 0);
        EXPECT_EQ(msg.getMessageData(), messageData);
        EXPECT_EQ(msg.getSequentialId(), Id--);
    }

    // The messages should all be located in buffer 0
    EXPECT_EQ(msgHandler->getBuffers()->back().size(), 0);

    // Make sure that the created message handler will be deleted
    delete msgHandler;
};

TEST(MessageHandler, TxTestRunBlockSend) {
    /* Code that inserts messages in the buffers, for the Tx Message 
     * Handler to read and insert them in the respective exit port signals.
     * No need to have a translation table, since each buffer is assigned to 
     * one exit port 
     */

    Signal::t_write_mode sWriteMode{Signal::t_write_mode::Ascii};
    Signal::t_header_type hType{Signal::t_header_type::noHeader};

    HandlerMessage exitPort1{"exitPort1.sgn", 10, hType, sWriteMode};
    HandlerMessage exitPort2{"exitPort2.sgn", 10, hType, sWriteMode};

    auto* msgHandler = new MessageHandler({}, {&exitPort1, &exitPort2});

    // Run initialize function
    msgHandler->initialize();

    // Assess functioning mode
    EXPECT_EQ(msgHandler->getFunctioningMode(), FUNCTIONING_AS_TX);

    // Insert data in buffers
    // Default parameters
    t_string destinationBlock = "dstBlock", sourceBlock = "srcBlock",
             messageType = "1212";
    uint32_t Id = 1;

    uint8_t sourcePort = 0, priority = 5;
    t_string messageData = "Hello to the World!";

    for (auto& buffer : *msgHandler->getBuffers()) {

        // Create message
        t_handler_message msg;
        msg.setHeader(destinationBlock, sourceBlock, priority, Id++,
                      messageType, sourcePort++,
                      std::to_string(messageData.length()));
        msg.setMessageData(messageData);

        // Insert message in buffer
        buffer.insert_sorted(msg);
    }

    // Check if the buffers have one message
    for (const auto& buffer : *msgHandler->getBuffers())
        EXPECT_EQ(buffer.size(), 1);

    // Check if the exit ports are empty
    for (auto signal : msgHandler->outputSignals)
        EXPECT_EQ(signal->ready(), 0);

    // Run the block
    msgHandler->runBlock();

    // Check if the buffers are empty
    for (const auto& buffer : *msgHandler->getBuffers())
        EXPECT_EQ(buffer.size(), 0);

    // Check if the exit ports have one message each
    sourcePort = 0;
    for (auto signal : msgHandler->outputSignals) {
        EXPECT_EQ(signal->ready(), 1);
        // Check if the messages are correct
        t_handler_message msg;
        signal->bufferGet(&msg);
        std::cout << "msg: " << msg.getRawMessage() << std::endl;
        EXPECT_EQ(msg.getSourcePort(), sourcePort++);
        EXPECT_EQ(msg.getMessageData(), messageData);
    }

    // Make sure that the created message handler will be deleted
    delete msgHandler;
};

TEST(MessageHandler, TxTestRunBlockSendReceiveWTranslationTable) {
    /* Code that tests the Tx Message Handler's ability to receive and send data,
     * including using a Destination Translation Table, to know the exit port and
     * associated buffer to send the message to. 
     */
    // Create signals
    Signal::t_write_mode sWriteMode{Signal::t_write_mode::Ascii};
    Signal::t_header_type hType{Signal::t_header_type::noHeader};

    HandlerMessage coreBlock1{"coreBlock1.sgn", 10, hType, sWriteMode};
    HandlerMessage coreBlock2{"coreBlock2.sgn", 10, hType, sWriteMode};

    HandlerMessage exitPort1{"exitPort1.sgn", 10, hType, sWriteMode};
    HandlerMessage exitPort2{"exitPort2.sgn", 10, hType, sWriteMode};

    // Create translation table
    DestinationTranslationTable translationTable;
    translationTable.add("parameter_estimation", 0);   // exitPort1
    translationTable.add("privacy_amplification", 0);  // exitPort1
    translationTable.add("error_correction", 1);       // exitPort2
    translationTable.add("frame_synchronization", 1);  // exitPort2

    auto* msgHandler =
        new MessageHandler({&coreBlock1, &coreBlock2}, {&exitPort1, &exitPort2},
                           translationTable, FUNCTIONING_AS_TX);

    // Run initialize function
    msgHandler->initialize();

    // Assess functioning mode
    EXPECT_EQ(msgHandler->getFunctioningMode(), FUNCTIONING_AS_TX);

    t_string messageData = "This is clearly a dummy message!";

    // Insert Messages in Core Block 1
    for (uint32_t Id = 1; Id <= 10; Id++) {

        t_string destinationBlock;
        if (Id > 5 == 0)
            destinationBlock = "parameter_estimation";
        else
            destinationBlock = "error_correction";

        t_string sourceBlock = "coreBlock1", messageType = "9999";
        uint8_t sourcePort = 9;
        uint8_t priority = Id % 5;  // Rotate priority

        t_handler_message msg;
        msg.setHeader(destinationBlock, sourceBlock, priority, Id, messageType,
                      sourcePort, std::to_string(messageData.length()));
        msg.setMessageData(messageData);

        coreBlock1.bufferPut(msg);
    }

    // Check that Core Block 1 has 10 messages
    EXPECT_EQ(coreBlock1.ready(), 10);
    EXPECT_EQ(coreBlock1.space(), 0);

    // Insert Messages in Core Block 2
    for (uint32_t Id = 1; Id <= 10; Id++) {

        t_string destinationBlock;
        if (Id > 5 == 0)
            destinationBlock = "privacy_amplification";
        else
            destinationBlock = "frame_synchronization";

        t_string sourceBlock = "coreBlock2", messageType = "9999";
        uint8_t sourcePort = 9;
        uint8_t priority = Id % 5;  // Circularly rotate priority

        t_handler_message msg;
        msg.setHeader(destinationBlock, sourceBlock, priority, Id, messageType,
                      sourcePort, std::to_string(messageData.length()));
        msg.setMessageData(messageData);

        coreBlock2.bufferPut(msg);
    }

    // Check that Core Block 2 has 10 messages
    EXPECT_EQ(coreBlock2.ready(), 10);
    EXPECT_EQ(coreBlock2.space(), 0);

    // Run the block
    msgHandler->runBlock();

    // Check that the core blocks are empty
    EXPECT_EQ(coreBlock1.ready(), 0);
    EXPECT_EQ(coreBlock2.ready(), 0);

    // Check that the buffers have the correct messages
    for (const auto& buffer : *msgHandler->getBuffers()) {
        for (auto& msg : buffer) {
            // Check destination block message parameter
            EXPECT_EQ(
                msg.getDestinationBlock() != "parameter_estimation" &&
                    msg.getDestinationBlock() != "privacy_amplification" &&
                    msg.getDestinationBlock() != "frame_synchronization" &&
                    msg.getDestinationBlock() != "error_correction",
                false);
        }
    }

    // Run the second time, to place the messages in the exit ports
    msgHandler->runBlock();

    // Check that the exit ports have 10 messages each
    EXPECT_EQ(exitPort1.ready(), 10);
    EXPECT_EQ(exitPort2.ready(), 10);

    // Check that the exit ports have the correct messages
    for (auto& signal : msgHandler->outputSignals) {
        while (signal->ready() > 0) {
            t_handler_message msg;
            signal->bufferGet(&msg);

            // Check destination block message parameter
            EXPECT_EQ(
                msg.getDestinationBlock() != "parameter_estimation" &&
                    msg.getDestinationBlock() != "privacy_amplification" &&
                    msg.getDestinationBlock() != "frame_synchronization" &&
                    msg.getDestinationBlock() != "error_correction",
                false);
        }
    }

    // Make sure that the created message handler will be deleted
    delete msgHandler;
};

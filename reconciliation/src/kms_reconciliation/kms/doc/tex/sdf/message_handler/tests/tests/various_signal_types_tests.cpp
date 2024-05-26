// Import of standard or provided libraries
#include <gtest/gtest.h>

// Import of local libraries
#include "message_handler.h"

/*!
 * This is a test to the Message Handler, to guarantee it's detection of 
 * operating modes and operation with signals of type different than 
 * t_handler_message
*/

TEST(MessageHandler, TestCreateTxMode) {
    Signal::t_write_mode sWriteMode{Signal::t_write_mode::Ascii};
    Signal::t_header_type hType{Signal::t_header_type::noHeader};

    Binary coreBlock1_tx{"coreBlock1.sgn", 10, hType, sWriteMode};
    TimeDiscreteAmplitudeDiscreteInteger coreBlock2_tx{"coreBlock2.sgn", 10,
                                                       hType, sWriteMode};

    HandlerMessage exitPort1{"exitPort1.sgn", 10, hType, sWriteMode};
    HandlerMessage exitPort2{"exitPort2.sgn", 10, hType, sWriteMode};

    // Create translation table for output signals
    DestinationTranslationTable outputBlockTranslation;
    outputBlockTranslation.add("coreBlock1_tx", 0);  // exitPort1
    outputBlockTranslation.add("coreBlock2_tx", 1);  // exitPort2

    // Create translation table for input signals
    InputTranslationTable inputBlockTranslation;
    inputBlockTranslation.add(0, {"coreBlock1_tx", "coreBlock1_tx"});
    inputBlockTranslation.add(1, {"coreBlock2_tx", "coreBlock2_tx"});

    auto* msgHandler = new MessageHandler(
        {&coreBlock1_tx, &coreBlock2_tx}, {&exitPort1, &exitPort2},
        outputBlockTranslation, FUNCTIONING_AS_TX, inputBlockTranslation);

    // Run initialize function
    msgHandler->initialize();

    // Assess functioning mode
    EXPECT_EQ(msgHandler->getFunctioningMode(), FUNCTIONING_AS_TX);
};

TEST(MessageHandler, TestCreateRxMode) {

    Signal::t_write_mode sWriteMode{Signal::t_write_mode::Ascii};
    Signal::t_header_type hType{Signal::t_header_type::noHeader};

    HandlerMessage coreBlock1{"coreBlock1.sgn", 10, hType, sWriteMode};
    HandlerMessage coreBlock2{"coreBlock2.sgn", 10, hType, sWriteMode};

    Binary exitPort1{"exitPort1.sgn", 10, hType, sWriteMode};
    HandlerMessage exitPort2{"exitPort2.sgn", 10, hType, sWriteMode};

    auto* msgHandler = new MessageHandler({&coreBlock1, &coreBlock2},
                                          {&exitPort1, &exitPort2});

    // Run initialize function
    msgHandler->initialize();

    // Assess functioning mode
    EXPECT_EQ(msgHandler->getFunctioningMode(), FUNCTIONING_AS_RX);
};

TEST(MessageHandler, TestBadCreationMode) {

    Signal::t_write_mode sWriteMode{Signal::t_write_mode::Ascii};
    Signal::t_header_type hType{Signal::t_header_type::noHeader};

    Binary coreBlock1{"coreBlock1.sgn", 10, hType, sWriteMode};
    HandlerMessage coreBlock2{"coreBlock2.sgn", 10, hType, sWriteMode};

    HandlerMessage exitPort1{"exitPort1.sgn", 10, hType, sWriteMode};
    Binary exitPort2{"exitPort2.sgn", 10, hType, sWriteMode};

    auto* msgHandler = new MessageHandler({&coreBlock1, &coreBlock2},
                                          {&exitPort1, &exitPort2});

    // Run initialize function
    /* invalid argument because of different signal types on automatic signal mode
     * detection, leading to the inability to determine mode (Rx or Tx) */
    EXPECT_THROW(msgHandler->initialize(), std::invalid_argument);

    delete msgHandler;

    HandlerMessage exitPort_subs_2{"exitPort2.sgn", 10, hType, sWriteMode};

    msgHandler = new MessageHandler({&coreBlock1, &coreBlock2},
                                    {&exitPort1, &exitPort_subs_2});

    // Run initialize function
    /* invalid arguments, because input signal translation table not supplied
     * when using signals of various types */
    EXPECT_THROW(msgHandler->initialize(), std::invalid_argument);
};

TEST(MessageHandler, TestFunctioningMode) {
    /* Combined Setup*/
    Signal::t_write_mode sWriteMode{Signal::t_write_mode::Ascii};
    Signal::t_header_type hType{Signal::t_header_type::noHeader};

    HandlerMessage exitPort1{"exitPort1.sgn", 10, hType, sWriteMode};
    HandlerMessage exitPort2{"exitPort2.sgn", 10, hType, sWriteMode};

    /* Setup for Tx */
    unsigned long bufferSize = 256;
    Binary coreBlock1_tx{"coreBlock1_tx.sgn", bufferSize};
    TimeDiscreteAmplitudeDiscreteInteger coreBlock2_tx{"coreBlock2_tx.sgn", 1,
                                                       hType, sWriteMode};
    coreBlock1_tx.setSaveSignal(false);
    coreBlock1_tx.setNoHeader(true);
    coreBlock2_tx.setSaveSignal(false);

    // Create translation table for output signals
    DestinationTranslationTable outputBlockTranslationTx;
    outputBlockTranslationTx.add("coreBlock1_rx", 0);  // exitPort1
    outputBlockTranslationTx.add("coreBlock2_rx", 1);  // exitPort2

    // Create translation table for input signals
    InputTranslationTable inputBlockTranslation;
    inputBlockTranslation.add(0, {"coreBlock1_rx", "coreBlock1_tx"});
    inputBlockTranslation.add(1, {"coreBlock2_rx", "coreBlock2_tx"});

    auto* txMsgHandler = new MessageHandler(
        {&coreBlock1_tx, &coreBlock2_tx}, {&exitPort1, &exitPort2},
        outputBlockTranslationTx, FUNCTIONING_AS_TX, inputBlockTranslation);

    // Run initialize function
    txMsgHandler->initialize();

    std::vector<t_binary> * coreBlock1_tx_data = new std::vector<t_binary>();

    // Insert data in coreBlock1_tx signal
    t_binary val{1};
    for (auto k = 0; k < 50; k++) {
        val = !val; // cycle values
        coreBlock1_tx.bufferPut(val);
        coreBlock1_tx_data->push_back(val);
    }

    // Check coreBlock1_tx signal data
    EXPECT_EQ(coreBlock1_tx.ready(), 50);
    EXPECT_EQ(coreBlock1_tx.space(), 206);

    std::vector<t_integer> * coreBlock2_tx_data = new std::vector<t_integer>();

    // Insert data in coreBlock2_tx signal
    t_integer ready = coreBlock2_tx.space();
    for (auto k = 0; k < ready; k++) {
        t_integer val = netxpto::randi(INT_MAX);
        coreBlock2_tx.bufferPut(val);
        coreBlock2_tx_data->push_back(val);
    }

    // Check coreBlock2_tx signal data
    EXPECT_EQ(coreBlock2_tx.ready(), 1);
    EXPECT_EQ(coreBlock2_tx.space(), 0);

    // Execute block (2 runs for the messages to be placed in the output signals)
    txMsgHandler->runBlock();
    txMsgHandler->runBlock();

    // Check that data was consumed
    EXPECT_EQ(coreBlock1_tx.space(), 256);
    EXPECT_EQ(coreBlock2_tx.space(), 1);

    // Check message values
    for (int i = 0; i < txMsgHandler->getNumberOfOutputSignals(); i++) {
        t_handler_message msg;
        txMsgHandler->outputSignals[i]->bufferGet(&msg);

        // Uncomment to see message contents
        //std::cout << "Signal: " << i << ", data: " << msg << std::endl;

        if (i == 0) {
            EXPECT_EQ(msg.getDestinationBlock(), "coreBlock1_rx");
            EXPECT_EQ(msg.getSourceBlock(), "coreBlock1_tx");
            EXPECT_EQ(msg.getMessageType(), "Binary");
            EXPECT_EQ(msg.getSourcePort(), 0);
        } else if (i == 1) {
            EXPECT_EQ(msg.getDestinationBlock(), "coreBlock2_rx");
            EXPECT_EQ(msg.getSourceBlock(), "coreBlock2_tx");
            EXPECT_EQ(msg.getMessageType(), "TimeDiscreteAmplitudeDiscreteIntege"); // the r is cutoff
            EXPECT_EQ(msg.getSourcePort(), 1);
        } else {
            throw std::runtime_error(
                "The test is only prepared for two Output Signals");
        }

        // Re-insert message
        txMsgHandler->outputSignals[i]->bufferPut(msg);
    }

    /* Setup for Rx */
    Binary coreBlock1_rx{"coreBlock1_rx.sgn", bufferSize};
    TimeDiscreteAmplitudeDiscreteInteger coreBlock2_rx{"coreBlock2_rx.sgn", 1,
                                                       hType, sWriteMode};
    coreBlock1_rx.setSaveSignal(false);
    coreBlock1_rx.setNoHeader(true);
    coreBlock2_rx.setSaveSignal(false);

    // Create translation table for output signals
    DestinationTranslationTable outputBlockTranslationRx;
    outputBlockTranslationRx.add("coreBlock1_rx", 0);  // exitPort1
    outputBlockTranslationRx.add("coreBlock2_rx", 1);  // exitPort2

    auto* rxMsgHandler = new MessageHandler(
        {&exitPort1, &exitPort2}, {&coreBlock1_rx, &coreBlock2_rx},
        outputBlockTranslationRx, FUNCTIONING_AS_RX);

    // Check that the messages are in input signals of Rx
    EXPECT_EQ(exitPort1.ready(), 1);
    EXPECT_EQ(exitPort2.ready(), 1);

    // Run initialize function
    rxMsgHandler->initialize();

    // Execute block (2 runs for the messages to be placed in the output signals)
    rxMsgHandler->runBlock();
    rxMsgHandler->runBlock();

    // Check that all messages were consumed
    EXPECT_EQ(exitPort1.ready(), 0);
    EXPECT_EQ(exitPort2.ready(), 0);

     // Check that the number of data in the signals is the same
    EXPECT_EQ(coreBlock1_rx.ready(), coreBlock1_tx_data->size());
    EXPECT_EQ(coreBlock2_rx.ready(), coreBlock2_tx_data->size());

    
    // Check values in coreBlock1_rx signal match the placed ones
    for (auto k = 0; k < coreBlock1_rx.ready(); k++) {
        t_binary val{0};
        val = (t_integer)coreBlock1_rx.bufferGet();
        EXPECT_EQ(val, coreBlock1_tx_data->at(k));
    }

    // Check values in coreBlock2_rx signal match the placed ones
    for (auto k = 0; k < coreBlock2_rx.ready(); k++) {
        t_real val;
        val = (t_integer)coreBlock2_rx.bufferGet();
        EXPECT_EQ(val, coreBlock2_tx_data->at(k));
    }
};
// Import of standard or provided libraries
#include <gtest/gtest.h>

// Import of local libraries
#include "ip_tunnel_ms_windows_20200819.h"

/*!
 * Tests for the IPTunnel class, using the new (t_handler_messages) and 
 * old (t_message) message format. This was done to guarantee that new changes 
 * didn't affect old functionalities. This class works both for Windows and Linux
*/

#define IP_TX "127.0.0.1"
#define IP_RX "127.0.0.1"
// Change the port numbers, if some conflicts arise
#define PORT_TX 54000 
#define PORT_RX 54001

// Header + Body
const uint32_t HEADER_SIZE = DST_BLOCK_SIZE + SRC_BLOCK_SIZE + PRIORITY_SIZE +
                             ID_SIZE + MSG_TYPE_SIZE + SRC_PORT_SIZE +
                             MSG_DATA_LENGTH_SIZE;
const uint32_t MAX_COMPLETE_MSG_SIZE = HEADER_SIZE + MAX_MSG_SIZE;

TEST(IPTunnel, TestOldMessages) {
    Signal::t_write_mode sWriteMode{Signal::t_write_mode::Ascii};
    Signal::t_header_type hType{Signal::t_header_type::noHeader};

    Message entryPort{"entryPort.sgn", 10, hType, sWriteMode};

    Message exitPort{"exitPort.sgn", 10, hType, sWriteMode};

    // Client - Transmitter
    IPTunnel IPTunnel_Tx{ {&entryPort}, {} };
    IPTunnel_Tx.setLocalMachineIpAddress(IP_TX);
    //IPTunnel_Tx.setLocalMachinePort(PORT_TX);
    IPTunnel_Tx.setRemoteMachineIpAddress(IP_RX);
    IPTunnel_Tx.setRemoteMachinePort(PORT_RX);
    IPTunnel_Tx.setVerboseMode(true);

    // Server - Receiver
    IPTunnel IPTunnel_Rx{ {}, {&exitPort} };
    IPTunnel_Rx.setLocalMachineIpAddress(IP_RX);
    IPTunnel_Rx.setLocalMachinePort(PORT_RX);
    IPTunnel_Rx.setRemoteMachineIpAddress(IP_TX);
    //IPTunnel_Tx.setRemoteMachinePort(PORT_TX);
    IPTunnel_Rx.setVerboseMode(true);

    // Initialize IP tunnel Rx
    IPTunnel_Rx.initialize();

    // Initialize IP tunnel Tx
    IPTunnel_Tx.initialize();

    // Insert Messages in IP Tunnel Tx
    t_string dummyData = "Hello World!";
    t_integer nMessages = 9;
    for(int i = 0; i < nMessages; i++) {
        t_message msg;
        msg.setMessageType(std::to_string(i));
        msg.setMessageDataLength(std::to_string(dummyData.length()));
        msg.setMessageData(dummyData);
        IPTunnel_Tx.inputSignals[0]->bufferPut(msg);
    }

    // Assertion for values in Input Signal of Tx
    EXPECT_EQ(entryPort.ready(), nMessages);

    // Run Tx and Rx Blocks
    for(int i = 0; i < nMessages+1; i++) {
        IPTunnel_Tx.runBlock();
        IPTunnel_Rx.runBlock();
    }

    // Assertion for values in Output Signal of Rx
    EXPECT_EQ(exitPort.ready(), nMessages);

    // Check messages arrived at Output Signals of Rx
    for(int i = 0; i < nMessages; i++) {
        t_message msg;
        IPTunnel_Rx.outputSignals[0]->bufferGet(&msg); // Instead of exitPort.bufferPut(msg), because of compilation issues
        EXPECT_EQ(msg.getMessageType(), std::to_string(i));
        EXPECT_EQ(std::stoi(msg.getMessageDataLength()), static_cast<int>(dummyData.length()));
        EXPECT_EQ(msg.getMessageData(), dummyData);
    }

    // Terminate IP Tunnels
    IPTunnel_Tx.terminate();
    IPTunnel_Rx.terminate();
}

TEST(IPTunnel, TestNewMessages) {
    Signal::t_write_mode sWriteMode{Signal::t_write_mode::Ascii};
    Signal::t_header_type hType{Signal::t_header_type::noHeader};

    HandlerMessage entryPort{"entryPort.sgn", 10, hType, sWriteMode};

    HandlerMessage exitPort{"exitPort.sgn", 10, hType, sWriteMode};

    // Client - Transmitter
    IPTunnel IPTunnel_Tx{ {&entryPort}, {} };
    IPTunnel_Tx.setLocalMachineIpAddress(IP_TX);
    //IPTunnel_Tx.setLocalMachinePort(PORT_TX);
    IPTunnel_Tx.setRemoteMachineIpAddress(IP_RX);
    IPTunnel_Tx.setRemoteMachinePort(PORT_RX);
    IPTunnel_Tx.setVerboseMode(true);

    // Server - Receiver
    IPTunnel IPTunnel_Rx{ {}, {&exitPort} };
    IPTunnel_Rx.setLocalMachineIpAddress(IP_RX);
    IPTunnel_Rx.setLocalMachinePort(PORT_RX);
    IPTunnel_Rx.setRemoteMachineIpAddress(IP_TX);
    //IPTunnel_Tx.setRemoteMachinePort(PORT_TX);
    IPTunnel_Rx.setVerboseMode(true);

    // Initialize IP tunnel Rx
    IPTunnel_Rx.initialize();

    // Initialize IP tunnel Tx
    IPTunnel_Tx.initialize();

    // Insert Messages in IP Tunnel Tx
    t_string dummyData = "Hello World!";
    t_string dstBlock = "dstBlock";
    t_string srcBlock = "srcBlock";
    t_integer priority = 5;
    uint8_t port = 0;
    t_integer nMessages = 9;
    for(int i = 0; i < nMessages; i++) {
        t_handler_message msg;
        msg.setHeader(dstBlock, srcBlock, priority, i, std::to_string(i), port, std::to_string(dummyData.length()));
        msg.setMessageData(dummyData);
        IPTunnel_Tx.inputSignals[0]->bufferPut(msg);
    }

    // Assertion for values in Input Signal of Tx
    EXPECT_EQ(entryPort.ready(), nMessages);

    // Run Tx and Rx Blocks
    for(int i = 0; i < nMessages+1; i++) {
        std::cout << "Starting test" << std::endl;
        IPTunnel_Tx.runBlock();
        IPTunnel_Rx.runBlock();
    }

    // Assertion for values in Output Signal of Rx
    EXPECT_EQ(exitPort.ready(), nMessages);
    
    // Check messages arrived at Output Signals of Rx
    for(int i = 0; i < nMessages; i++) {
        t_handler_message msg;
        IPTunnel_Rx.outputSignals[0]->bufferGet(&msg); // Instead of exitPort.bufferPut(msg), because of compilation issues
        EXPECT_EQ(msg.getDestinationBlock(), dstBlock);
        EXPECT_EQ(msg.getSourceBlock(), srcBlock);
        EXPECT_EQ(msg.getPriority(), priority);
        EXPECT_EQ(msg.getSequentialId(), i);
        EXPECT_EQ(msg.getMessageType(), std::to_string(i));
        EXPECT_EQ(msg.getSourcePort(), port);
        EXPECT_EQ(std::stoi(msg.getMessageDataLength()), static_cast<int>(dummyData.length()));
        EXPECT_EQ(msg.getMessageData(), dummyData);
    }
    

    // Terminate IP Tunnels
    IPTunnel_Tx.terminate();
    IPTunnel_Rx.terminate();
}
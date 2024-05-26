// Import of standard or provided libraries
#include <gtest/gtest.h>
#include <memory>

// Import of local libraries
#include "left_superblock.h"
#include "right_superblock.h"
#include "ip_tunnel_ms_windows_20200819.h"

#define IP_ADDRESS "127.0.0.1"
#define PORT_TX 54006
#define PORT_RX 54007
#define VERBOSE_MODE false

TEST(MessageHandler, TestCompleteSystem) {
    /* Simple test for full system execution, no lost messages should appear, 
     * since we are stopping the generation of new messages by the core 
     * blocks, in the last 10 iterations (it should take roughly 6 iteration 
     * for a message to be fully processed, which means, generated by one 
     * Core Block, processed by the other and a response be received)
     */

    // Create signals to connect left to right SuperBlocks
    Signal::t_write_mode sWriteMode{Signal::t_write_mode::Ascii};
    Signal::t_header_type hType{Signal::t_header_type::noHeader};

    // The signals are declared from top to bottom, according to the diagram
    HandlerMessage leftToRight{"left_to_right.sgn", 10, hType, sWriteMode};
    HandlerMessage rightToLeft{"right_to_left.sgn", 10, hType, sWriteMode};

    // Instantiate left SuperBlock
    auto* leftSuperBlock = new LeftSuperBlock({&rightToLeft}, {&leftToRight});
    leftSuperBlock->initialize();

    // Instantiate right SuperBlock
    auto* rightSuperBlock = new RightSuperBlock({&leftToRight}, {&rightToLeft});
    rightSuperBlock->initialize();

    // Simulation Variables
    uint32_t iterations = 100;
    uint32_t numberOfMessagesToGenerate = 1;  // in each block

    leftSuperBlock->setNumberOfMessagesToGenerate(numberOfMessagesToGenerate);
    rightSuperBlock->setNumberOfMessagesToGenerate(numberOfMessagesToGenerate);

    // Run the simulation
    for (uint32_t i = 0; i < iterations; i++) {
        leftSuperBlock->runBlock();
        rightSuperBlock->runBlock();

        // Reaching the remaining 10% of iterations, send signal to terminate the simulation
        if (i >= iterations * 0.90) {
            leftSuperBlock->sendSignalToTerminate();
            rightSuperBlock->sendSignalToTerminate();
        }
    }

    // Terminate the Simulation
    leftSuperBlock->terminate();
    rightSuperBlock->terminate();
}

TEST(MessageHandler, TestCompleteSystemWIpTunnel) {
    /* Simple test for full system execution, using IP tunnel.
     * It's natural to have some lost messages, since the IP Tunnel increases 
     * headroom and since the priority is being rotated of [0,5], some 
     * messages will see their priority being decreased and subsequently 
     * getting eliminated when reaching 0
    */

    // Create signals to connect left to right SuperBlocks
    Signal::t_write_mode sWriteMode{Signal::t_write_mode::Ascii};
    Signal::t_header_type hType{Signal::t_header_type::noHeader};

    // The signals are declared from top to bottom, according to the diagram
    HandlerMessage topLeft{"top_left.sgn", 10, hType, sWriteMode};
    HandlerMessage topRight{"top_right.sgn", 10, hType, sWriteMode};
    HandlerMessage bottomLeft{"bottom_left.sgn", 10, hType, sWriteMode};
    HandlerMessage bottomRight{"bottom_right.sgn", 10, hType, sWriteMode};

    // Client - Transmitter
    IPTunnel IPTunnelTopLeft{ {&topLeft}, {} };
    IPTunnelTopLeft.setLocalMachineIpAddress(IP_ADDRESS);
    //IPTunnelTopLeft.setLocalMachinePort(PORT_TX);
    IPTunnelTopLeft.setRemoteMachineIpAddress(IP_ADDRESS);
    IPTunnelTopLeft.setRemoteMachinePort(PORT_RX);
    IPTunnelTopLeft.setVerboseMode(VERBOSE_MODE);

    // Server - Receiver
    IPTunnel IPTunnelTopRight{ {}, {&topRight} };
    IPTunnelTopRight.setLocalMachineIpAddress(IP_ADDRESS);
    IPTunnelTopRight.setLocalMachinePort(PORT_RX);
    IPTunnelTopRight.setRemoteMachineIpAddress(IP_ADDRESS);
    //IPTunnelTopRight.setRemoteMachinePort(PORT_TX);
    IPTunnelTopRight.setVerboseMode(VERBOSE_MODE);

    // Client - Transmitter
    IPTunnel IPTunnelBottomRight{ {&bottomRight}, {} };
    IPTunnelBottomRight.setLocalMachineIpAddress(IP_ADDRESS);
    //IPTunnelBottomRight.setLocalMachinePort(PORT_TX);
    IPTunnelBottomRight.setRemoteMachineIpAddress(IP_ADDRESS);
    IPTunnelBottomRight.setRemoteMachinePort(PORT_TX);
    IPTunnelBottomRight.setVerboseMode(VERBOSE_MODE);

    // Server - Receiver
    IPTunnel IPTunnelBottomLeft{ {}, {&bottomLeft} };
    IPTunnelBottomLeft.setLocalMachineIpAddress(IP_ADDRESS);
    IPTunnelBottomLeft.setLocalMachinePort(PORT_TX);
    IPTunnelBottomLeft.setRemoteMachineIpAddress(IP_ADDRESS);
    //IPTunnelBottomLeft.setRemoteMachinePort(PORT_TX);
    IPTunnelBottomLeft.setVerboseMode(VERBOSE_MODE);

    // Initialize IP tunnels
    IPTunnelTopRight.initialize();
    IPTunnelBottomLeft.initialize();
    IPTunnelTopLeft.initialize();
    IPTunnelBottomRight.initialize();

    // Instantiate left SuperBlock
    auto* leftSuperBlock = new LeftSuperBlock({&bottomLeft}, {&topLeft});
    leftSuperBlock->initialize();

    // Instantiate right SuperBlock
    auto* rightSuperBlock = new RightSuperBlock({&topRight}, {&bottomRight});
    rightSuperBlock->initialize();

    // Simulation Variables
    uint32_t iterations = 100;
    uint32_t numberOfMessagesToGenerate = 1;  // in each block

    leftSuperBlock->setNumberOfMessagesToGenerate(numberOfMessagesToGenerate);
    rightSuperBlock->setNumberOfMessagesToGenerate(numberOfMessagesToGenerate);

    // Run the simulation
    for (uint32_t i = 0; i < iterations; i++) {
        leftSuperBlock->runBlock();
        rightSuperBlock->runBlock();
        IPTunnelTopRight.runBlock();
        IPTunnelBottomLeft.runBlock();
        IPTunnelTopRight.runBlock();
        IPTunnelBottomRight.runBlock();

        // Reaching the remaining 10% of iterations, send signal to terminate the simulation
        if (i >= iterations * 0.90) {
            leftSuperBlock->sendSignalToTerminate();
            rightSuperBlock->sendSignalToTerminate();
        }
    }

    // Terminate the Simulation
    leftSuperBlock->terminate();
    rightSuperBlock->terminate();
    IPTunnelTopRight.terminate();
    IPTunnelBottomLeft.terminate();
    IPTunnelTopRight.terminate();
    IPTunnelBottomRight.terminate();
}

TEST(MessageHandler, TestCompleteSystemNoTerminateSignal) {
    /* Simple test for full system execution with not signal termination.
     * Don't worry if some messages appear to be lost, that only indicates
     * that the system has been saturated and takes some time for those messages
     * to be processed. That can happen on this test, since we don't order for the
     * core blocks to stop processing before terminating the system.
    */

    // Create signals to connect left to right SuperBlocks
    Signal::t_write_mode sWriteMode{Signal::t_write_mode::Ascii};
    Signal::t_header_type hType{Signal::t_header_type::noHeader};

    // The signals are declared from top to bottom, according to the diagram
    HandlerMessage leftToRight{"left_to_right.sgn", 10, hType, sWriteMode};
    HandlerMessage rightToLeft{"right_to_left.sgn", 10, hType, sWriteMode};

    // Instantiate left SuperBlock
    auto* leftSuperBlock = new LeftSuperBlock({&rightToLeft}, {&leftToRight});
    leftSuperBlock->initialize();

    // Instantiate right SuperBlock
    auto* rightSuperBlock = new RightSuperBlock({&leftToRight}, {&rightToLeft});
    rightSuperBlock->initialize();

    // Simulation Variables
    uint32_t iterations = 100;
    uint32_t numberOfMessagesToGenerate = 1;  // in each block

    leftSuperBlock->setNumberOfMessagesToGenerate(numberOfMessagesToGenerate);
    rightSuperBlock->setNumberOfMessagesToGenerate(numberOfMessagesToGenerate);

    // Run the simulation
    for (uint32_t i = 0; i < iterations; i++) {
        leftSuperBlock->runBlock();
        rightSuperBlock->runBlock();
    }

    // Terminate the Simulation
    leftSuperBlock->terminate();
    rightSuperBlock->terminate();
}

/*
TEST(MessageHandler, TestCompleteSystemStressTest) {
    /* This is a stress test of the system. It's currently commented because
     * it generates a lot of unprocessed/lost messages. This is a unrealistic 
     * scenario since a total of 1400 messages will be generated by each core 
     * block, so a saturation point will be reached and a lot of messages will 
     * be discarded
    */

    /*

    // Create signals to connect left to right SuperBlocks
    Signal::t_write_mode sWriteMode{Signal::t_write_mode::Ascii};
    Signal::t_header_type hType{Signal::t_header_type::noHeader};

    // The signals are declared from top to bottom, according to the diagram
    HandlerMessage leftToRight{"left_to_right.sgn", 10, hType, sWriteMode};
    HandlerMessage rightToLeft{"right_to_left.sgn", 10, hType, sWriteMode};

    // Instantiate left SuperBlock
    auto* leftSuperBlock = new LeftSuperBlock({&rightToLeft}, {&leftToRight});
    leftSuperBlock->initialize();

    // Instantiate right SuperBlock
    auto* rightSuperBlock = new RightSuperBlock({&leftToRight}, {&rightToLeft});
    rightSuperBlock->initialize();

    // Simulation Variables
    uint32_t iterations = 300;
    uint32_t numberOfMessagesToGenerate = 7;  // in each block

    leftSuperBlock->setNumberOfMessagesToGenerate(numberOfMessagesToGenerate);
    rightSuperBlock->setNumberOfMessagesToGenerate(numberOfMessagesToGenerate);

    // Run the simulation
    for (uint32_t i = 0; i < iterations; i++) {
        leftSuperBlock->runBlock();
        rightSuperBlock->runBlock();

        // Reaching the remaining 5% of iterations, send signal to terminate the simulation
        if (i >= iterations * 0.95) {
            leftSuperBlock->sendSignalToTerminate();
            rightSuperBlock->sendSignalToTerminate();
        }
    }

    // Terminate the Simulation
    leftSuperBlock->terminate();
    rightSuperBlock->terminate();
}*/

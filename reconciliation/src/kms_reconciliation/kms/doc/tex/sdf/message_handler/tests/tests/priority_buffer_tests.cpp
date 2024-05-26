// Import of standard or provided libraries
#include <gtest/gtest.h>

// Import of local libraries
#include "message_handler.h"

/*!
 * Tests for the PriorityBuffer class that is implemented in the 
 * MessageHandler, to store messages by order of priority and arrival
*/

TEST(PriorityBuffer, TestClassCreation) {
    PriorityBuffer buffer(10);

    // Check buffer size
    EXPECT_EQ(buffer.getMaxSize(), 10);

    // Check overflow value
    EXPECT_EQ(buffer.getOverflowFlag(), false);

    // Check empty
    EXPECT_EQ(buffer.empty(), true);

    // Check not full
    EXPECT_EQ(buffer.full(), false);
};

TEST(PriorityBuffer, TestInsertSorted) {
    PriorityBuffer buffer(3);

    // Default parameters
    t_string destinationBlock = "dstBlock", sourceBlock = "srcBlock",
             messageType = "4321";
    uint8_t sourcePort = 1;
    uint32_t Id = 12;

    // Message 1
    t_string messageData1 = "message1";
    t_handler_message msg1;
    msg1.setHeader(destinationBlock, sourceBlock, 1, Id, messageType,
                   sourcePort, std::to_string(messageData1.length()));
    msg1.setMessageData(messageData1);

    // Message 2
    t_string messageData2 = "message2";
    t_handler_message msg2;
    msg2.setHeader(destinationBlock, sourceBlock, 2, Id, messageType,
                   sourcePort, std::to_string(messageData2.length()));
    msg2.setMessageData(messageData2);

    // Message 3
    t_string messageData3 = "message3";
    t_handler_message msg3;
    msg3.setHeader(destinationBlock, sourceBlock, 3, Id, messageType,
                   sourcePort, std::to_string(messageData3.length()));
    msg3.setMessageData(messageData3);

    // Message 4
    t_string messageData4 = "message4";
    t_handler_message msg4;
    msg4.setHeader(destinationBlock, sourceBlock, 4, Id, messageType,
                   sourcePort, std::to_string(messageData4.length()));
    msg4.setMessageData(messageData4);

    buffer.insert_sorted(msg1);
    buffer.insert_sorted(msg3);
    buffer.insert_sorted(msg2);
    buffer.insert_sorted(msg4);

    // Check buffer size and content
    EXPECT_EQ(buffer.getMaxSize(), 3);
    EXPECT_EQ(buffer.size(), 3);
    EXPECT_EQ(buffer.empty(), false);
    EXPECT_EQ(buffer.full(), true);
    t_handler_message msg = buffer.back();
    EXPECT_EQ(msg.getMessageData(), messageData3);
};

TEST(PriorityBuffer, TestStressInsertSorted) {
    PriorityBuffer buffer(12);

    // Insert Messages in Priority Buffer
    for (uint32_t Id = 1; Id <= 12; Id++) {

        t_string messageData = "This is clearly a dummy message!";
        t_string sourceBlock = "coreBlock1", messageType = "9999",
                 destinationBlock = "destBlock";
        uint8_t sourcePort = 9;     // Dummy Port
        uint8_t priority = Id % 6;  // Rotate priority

        t_handler_message msg;
        msg.setHeader(destinationBlock, sourceBlock, priority, Id, messageType,
                      sourcePort, std::to_string(messageData.length()));
        msg.setMessageData(messageData);

        buffer.insert_sorted(msg);
    }

    std::vector<uint8_t> prioritiesOrder = {5, 5, 4, 4, 3, 3, 2, 2, 1, 1, 0, 0};

    for (uint32_t i = 0; i < 12; i++) {
        t_handler_message msg = buffer.back();
        EXPECT_EQ(msg.getPriority(), prioritiesOrder.front());
        // Remove checked priority
        prioritiesOrder.erase(prioritiesOrder.begin());
        // Remove message from buffer
        buffer.pop_back();
    }

    // check if all priorities are checked
    EXPECT_EQ(prioritiesOrder.size(), 0);
};

TEST(PriorityBuffer, TestLowerPriority) {
    PriorityBuffer buffer(3);

    // Default parameters
    t_string destinationBlock = "dstBlock", sourceBlock = "srcBlock",
             messageType = "4321";
    uint8_t sourcePort = 1;
    uint32_t Id = 12;

    // Message 1
    t_string messageData1 = "message1";
    t_handler_message msg1;
    msg1.setHeader(destinationBlock, sourceBlock, 0, Id, messageType,
                   sourcePort, std::to_string(messageData1.length()));
    msg1.setMessageData(messageData1);

    // Message 2
    t_string messageData2 = "message2";
    t_handler_message msg2;
    msg2.setHeader(destinationBlock, sourceBlock, 3, Id, messageType,
                   sourcePort, std::to_string(messageData2.length()));
    msg2.setMessageData(messageData2);

    // Message 3
    t_string messageData3 = "message3";
    t_handler_message msg3;
    msg3.setHeader(destinationBlock, sourceBlock, 5, Id, messageType,
                   sourcePort, std::to_string(messageData3.length()));
    msg3.setMessageData(messageData3);

    buffer.insert_sorted(msg1);
    buffer.insert_sorted(msg2);
    buffer.insert_sorted(msg3);

    // Lower message priority
    bool decreased = buffer.lowerPriority();

    // Check message priority has been lowered
    EXPECT_EQ(decreased, true);
    EXPECT_EQ(buffer.back().getPriority(), 5);
    buffer.pop_back();
    EXPECT_EQ(buffer.back().getPriority(), 2);
    buffer.pop_back();
    EXPECT_EQ(buffer.back().getPriority(), 0);
    buffer.pop_back();

    // Call again to see if a message with priority = 5, triggers decreased variable
    buffer.insert_sorted(msg3);
    decreased = buffer.lowerPriority();
    EXPECT_EQ(decreased, false);
}

TEST(PriorityBuffer, TestRemoveLowPriorityMessages) {
    PriorityBuffer buffer(4);

    // Default parameters
    t_string destinationBlock = "dstBlock", sourceBlock = "srcBlock",
             messageType = "4321";
    uint8_t sourcePort = 1;
    uint32_t Id = 12;

    // Message 1
    t_string messageData1 = "message1";
    t_handler_message msg1;
    msg1.setHeader(destinationBlock, sourceBlock, 0, Id, messageType,
                   sourcePort, std::to_string(messageData1.length()));
    msg1.setMessageData(messageData1);

    // Message 2
    t_string messageData2 = "message2";
    t_handler_message msg2;
    msg2.setHeader(destinationBlock, sourceBlock, 3, Id, messageType,
                   sourcePort, std::to_string(messageData2.length()));
    msg2.setMessageData(messageData2);

    // Message 3
    t_string messageData3 = "message3";
    t_handler_message msg3;
    msg3.setHeader(destinationBlock, sourceBlock, 5, Id, messageType,
                   sourcePort, std::to_string(messageData3.length()));
    msg3.setMessageData(messageData3);

    buffer.insert_sorted(msg1);
    buffer.insert_sorted(msg2);
    buffer.insert_sorted(msg3);

    // Redirect stdout to a temporary file
    FILE* tmp = freopen("tmp.txt", "w", stderr);

    // Remove low priority messages
    bool overflow = buffer.removeLowPriorityMessages();

    // Close the temporary file and restore stdout
    fclose(tmp);
    freopen("/dev/tty", "w", stdout);

    // Read the contents of the temporary file
    std::ifstream file("tmp.txt");
    std::string output((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
    file.close();

    std::cout << "-> " << output << std::endl;

    // Check if the output contains the expected string
    EXPECT_NE(output.find("Message discarded"), std::string::npos);

    // Delete the temporary file
    std::remove("tmp.txt");

    // Check low priority messages have been removed
    EXPECT_EQ(buffer.empty(), false);
    EXPECT_EQ(buffer.full(), false);
    EXPECT_EQ(overflow, true);
    EXPECT_EQ(buffer.size(), 2);
    EXPECT_EQ(buffer.back().getMessageData(), "message3");
    buffer.pop_back();
    EXPECT_EQ(buffer.back().getMessageData(), "message2");
    buffer.pop_back();

    // Test if overflow flag is triggered if when there is no message with
    // priority = 0
    buffer.insert_sorted(msg3);
    overflow = buffer.removeLowPriorityMessages();
    EXPECT_EQ(overflow, false);
    EXPECT_EQ(buffer.size(), 1);

    // Message 4
    t_string messageData4 = "message4";
    t_handler_message msg4;
    msg4.setHeader(destinationBlock, sourceBlock, 0, Id, messageType,
                   sourcePort, std::to_string(messageData4.length()));
    msg4.setMessageData(messageData4);

    // Test if two low priority messages are removed
    buffer.insert_sorted(msg1);
    buffer.insert_sorted(msg2);
    buffer.insert_sorted(msg4);

    EXPECT_EQ(buffer.size(), 4);
    overflow = buffer.removeLowPriorityMessages();

    // The buffer should only have two messages
    EXPECT_EQ(overflow, true);
    EXPECT_EQ(buffer.size(), 2);
}

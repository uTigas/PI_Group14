// Import of standard or provided libraries
#include <gtest/gtest.h>

// Import of local libraries
#include "message_handler.h"

/*!
 * Tests for the t_handler_message class, asserting the correct functioning
 * of the class, and the preservation of information
*/

// Header + Body
const uint32_t HEADER_SIZE = DST_BLOCK_SIZE + SRC_BLOCK_SIZE + PRIORITY_SIZE +
                             ID_SIZE + MSG_TYPE_SIZE + SRC_PORT_SIZE +
                             MSG_DATA_LENGTH_SIZE;
const uint32_t MAX_COMPLETE_MSG_SIZE = HEADER_SIZE + MAX_MSG_SIZE;

TEST(t_handler_message, TestClassCreation) {
    t_handler_message message;
    t_string rawMessage = message.getRawMessage();

    t_string compareMessage;
    compareMessage.resize(MAX_COMPLETE_MSG_SIZE);
    EXPECT_EQ(rawMessage, compareMessage);
}

TEST(t_handler_message, TestDestinationBlock) {
    t_string EXPECTED_RESULT = "ParameterEstimation101";

    t_handler_message message;
    message.setDestinationBlock(EXPECTED_RESULT);
    t_string rawMessage = message.getRawMessage();

    // Make sure that the first BLOCK_SIZE bytes get filled
    for (int i = 0; i < DST_BLOCK_SIZE; i++) {
        char character = rawMessage[i];
        EXPECT_NE(character, '\0');
    }

    // Make sure the message type has the correct value
    EXPECT_EQ(message.getDestinationBlock(), EXPECTED_RESULT);
}

TEST(t_handler_message, TestSourceBlock) {
    t_string EXPECTED_RESULT = "DataAcquisition101";

    t_handler_message message;
    message.setSourceBlock(EXPECTED_RESULT);
    t_string rawMessage = message.getRawMessage();

    // Make sure that the source block gets filled
    for (int i = DST_BLOCK_SIZE; i < DST_BLOCK_SIZE + SRC_BLOCK_SIZE; i++) {
        char character = rawMessage[i];
        EXPECT_NE(character, '\0');
    }

    // Make sure the message type has the correct value
    EXPECT_EQ(message.getSourceBlock(), EXPECTED_RESULT);
}

TEST(t_handler_message, TestPrioritySuccess) {
    t_handler_message message;
    message.setPriority(3);
    t_string rawMessage = message.getRawMessage();

    // Make sure that the priority byte gets filled
    char priorityChar = rawMessage[DST_BLOCK_SIZE + SRC_BLOCK_SIZE];
    EXPECT_EQ(priorityChar, '3');

    // Convert from char to unsigned int
    uint8_t priority = std::atoi(&priorityChar);
    EXPECT_EQ(priority, 3);

    // Test get function
    EXPECT_EQ(message.getPriority(), 3);
}

TEST(t_handler_message, TestPriorityBad) {
    // The priority 6 should be replaced with 0, since it
    // surpasses the maximum value of 5
    t_handler_message message;
    message.setPriority(6);
    t_string rawMessage = message.getRawMessage();
    EXPECT_EQ(rawMessage[DST_BLOCK_SIZE + SRC_BLOCK_SIZE], '0');

    // Test get function
    EXPECT_EQ(message.getPriority(), 0);
}

TEST(t_handler_message, TestID) {
    uint32_t EXPECT_RESULT = 42;

    t_handler_message message;
    message.setSequentialId(EXPECT_RESULT);

    uint32_t headerSizeUntilId =
        DST_BLOCK_SIZE + SRC_BLOCK_SIZE + PRIORITY_SIZE;
    t_string rawMessage = message.getRawMessage();
    t_string rawId =
        rawMessage.substr(headerSizeUntilId, headerSizeUntilId + ID_SIZE);

    // Check the that the Id is correct
    uint32_t Id = std::stoul(rawId);
    EXPECT_EQ(Id, EXPECT_RESULT);

    // Test get function
    EXPECT_EQ(message.getSequentialId(), EXPECT_RESULT);
}

TEST(t_handler_message, TestMessageType) {
    t_handler_message message;
    t_string MSG_TYPE_CONFIGURATION = "12345";
    message.setMessageType(MSG_TYPE_CONFIGURATION);
    t_string rawMessage = message.getRawMessage();

    // Make sure that the next MSG_TYPE_SIZE bytes get filled
    uint32_t headerSizeUntilMessageType =
        DST_BLOCK_SIZE + SRC_BLOCK_SIZE + PRIORITY_SIZE + ID_SIZE;
    for (int i = headerSizeUntilMessageType;
         i < headerSizeUntilMessageType + MSG_TYPE_SIZE; i++) {
        char character = rawMessage[i];
        EXPECT_NE(character, '\0');
    }

    // Make sure the message type has the correct value
    EXPECT_EQ(message.getMessageType(), MSG_TYPE_CONFIGURATION);
}

TEST(t_handler_message, TestSourcePort) {
    t_handler_message message;
    message.setSourcePort(9);
    t_string rawMessage = message.getRawMessage();

    // Make sure that the Source Port has the correct value in the specified space
    uint32_t headerSizeUntilSourcePort = DST_BLOCK_SIZE + SRC_BLOCK_SIZE +
                                         PRIORITY_SIZE + ID_SIZE +
                                         MSG_TYPE_SIZE;
    char sourceChar = rawMessage[headerSizeUntilSourcePort];
    EXPECT_EQ(sourceChar, '9');

    // Convert from char to unsigned int
    uint8_t source = sourceChar - '0';

    EXPECT_EQ(source, 9);

    // Test get function
    EXPECT_EQ(message.getSourcePort(), 9);
}

TEST(t_handler_message, TestMessageDataLengthSuccess) {
    t_handler_message message;

    // Test valid input
    message.setMessageDataLength("123");
    t_string rawMessage = message.getRawMessage();
    uint32_t startDataLength = HEADER_SIZE - MSG_DATA_LENGTH_SIZE;

    EXPECT_EQ(message.getMessageDataLength(), "123");

    uint32_t dataLength = std::stoi(message.getMessageDataLength());
    // Assert that the data length is equal to 10
    EXPECT_EQ(dataLength, 123);

    // Test invalid input (string)
    EXPECT_THROW(message.setMessageDataLength("abc"), std::invalid_argument);

    // Test invalid input (too large)
    std::string maxLength = std::to_string(
        MAX_MESSAGE_SIZE - MESSAGE_TYPE_SIZE - MESSAGE_DATA_LENGTH_SIZE + 1);
    EXPECT_THROW(message.setMessageDataLength(maxLength),
                 std::invalid_argument);
}

TEST(t_handler_message, TestMessageData) {
    t_handler_message message;
    t_string messageData = "Hello, world!";
    message.setMessageData(messageData);
    t_string rawMessage = message.getRawMessage();

    // Make sure that the Message data gets filled
    for (int i = HEADER_SIZE; i < rawMessage.length(); i++) {
        char character = rawMessage[i];
        EXPECT_NE(character, '\0');
    }

    // Make sure the message data has the correct value
    EXPECT_EQ(message.getMessageData(), "Hello, world!");
};

TEST(t_handler_message, TestCompleteMessage) {
    /* 
     * This test is only used to observe the format of a complete message
     * Example of a complete message:
     * ("dstBlock                        srcBlock                        3124321                               1000008message2");
     */

    // Entry and expected values
    t_string messageData = "Hello, world!";
    t_string destinationBlock = "dstBlock", sourceBlock = "srcBlock",
             messageType = "4321",
             dataLength = std::to_string(messageData.length());
    uint8_t priority = 5, sourcePort = 9;
    uint32_t Id = 12;

    t_handler_message message;
    message.setHeader(destinationBlock, sourceBlock, priority, Id, messageType,
                      sourcePort, dataLength);
    message.setMessageData(messageData);
    t_string rawMessage = message.getRawMessage();

    // Change the NE, to EQ for the test to fail and see the message format
    EXPECT_NE(rawMessage, "");

    // Check if the message values are correct
    EXPECT_EQ(message.getMessageData(), messageData);
    EXPECT_EQ(message.getDestinationBlock(), destinationBlock);
    EXPECT_EQ(message.getSourceBlock(), sourceBlock);
    EXPECT_EQ(message.getMessageType(), messageType);
    EXPECT_EQ(message.getMessageDataLength(), dataLength);
    EXPECT_EQ(message.getPriority(), priority);
    EXPECT_EQ(message.getSourcePort(), sourcePort);
    EXPECT_EQ(message.getSequentialId(), Id);
}

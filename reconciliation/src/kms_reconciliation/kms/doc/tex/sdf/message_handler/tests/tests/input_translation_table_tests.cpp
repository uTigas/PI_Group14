// Import of standard or provided libraries
#include <gtest/gtest.h>

// Import of local libraries
#include "message_handler.h"

/*!
 * Tests for the InputTranslationTable and InputSignalInfo classes. This class 
 * is used by the MessageHandler class, while working in Tx mode and with 
 * signal types different than t_handler_message. With the Information of the 
 * connected signal/blocks in the InputSignalInfo class and the translation 
 * offered by the InputTranslationTable class, the MessageHandler can create 
 * messages, with the data transmitted by the signals
*/

TEST(InputTranslationTable, TestClassCreation) {
    InputTranslationTable table;

    // Check if exception is launched when the table is empty
    EXPECT_THROW(table.translate(1), std::runtime_error);

    // Check is size function works
    EXPECT_EQ(table.size(), 0);
}

TEST(InputTranslationTable, TestAdding) {
    InputTranslationTable table;

    // Add a new entry
    table.add(1, {"Destination1", "Source1"});

    // Check if the entry is added
    InputSignalInfo translated = table.translate(1);
    EXPECT_EQ(translated.destinationBlock, "Destination1");
    EXPECT_EQ(translated.sourceBlock, "Source1");

    // Add multiple entries
    table.add(2, {"Destination2", "Source2"});
    table.add(3, {"Destination3", "Source3"});

    // Check if the entries are added
    InputSignalInfo translated2 = table.translate(2);
    EXPECT_EQ(translated2.destinationBlock, "Destination2");
    EXPECT_EQ(translated2.sourceBlock, "Source2");

    InputSignalInfo translated3 = table.translate(3);
    EXPECT_EQ(translated3.destinationBlock, "Destination3");
    EXPECT_EQ(translated3.sourceBlock, "Source3");
}

TEST(InputTranslationTable, TestRemove) {
    std::map<t_integer, InputSignalInfo> translationTable{
        {0, {"Destination0", "Source0"}}, {1, {"Destination1", "Source1"}}, {2, {"Destination2", "Source2"}}};

    InputTranslationTable table{translationTable};

    // Check if the entries were added
    EXPECT_EQ(table.size(), 3);

    // Remove one entry
    table.remove(0);

    // Check if the entry was removed
    EXPECT_EQ(table.size(), 2);
    EXPECT_THROW(table.translate(56), std::runtime_error);
}

TEST(InputTranslationTable, TestTranslate) {
    std::map<t_integer, InputSignalInfo> translationTable{
        {0, {"Destination0", "Source0"}}, {1, {"Destination1", "Source1"}}, {2, {"Destination2", "Source2"}}};

    InputTranslationTable table{translationTable};

    // Check if the entries were added
    EXPECT_EQ(table.size(), 3);

    // Check if the entries have correct values
    InputSignalInfo translated1 = table.translate(0);
    EXPECT_EQ(translated1.destinationBlock, "Destination0");
    EXPECT_EQ(translated1.sourceBlock, "Source0");

    InputSignalInfo translated2 = table.translate(1);
    EXPECT_EQ(translated2.destinationBlock, "Destination1");
    EXPECT_EQ(translated2.sourceBlock, "Source1");

    InputSignalInfo translated3 = table.translate(2);
    EXPECT_EQ(translated3.destinationBlock, "Destination2");
    EXPECT_EQ(translated3.sourceBlock, "Source2");
}


TEST(InputTranslationTable, TestOtherValues) {
    InputTranslationTable table;

    // Test default values
    table.add(1, {"Destination1", "Source1"});
    EXPECT_EQ(table.translate(1).priority, 5);
    EXPECT_EQ(table.translate(1).SeqId, 0);

    // Test assigned values
    table.add(2, {"Destination1", "Source1", 3, 12});
    EXPECT_EQ(table.translate(2).priority, 3);
    EXPECT_EQ(table.translate(2).SeqId, 12);
}

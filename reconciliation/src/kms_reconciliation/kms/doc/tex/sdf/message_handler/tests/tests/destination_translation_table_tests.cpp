// Import of standard or provided libraries
#include <gtest/gtest.h>

// Import of local libraries
#include "message_handler.h"

/*!
 * Tests for the DestinationTranslationTable class, used by the MessageHandler 
 * to discover the correct port/output signal, according to the destination 
 * Block string
*/

TEST(DestinationTranslationTable, TestClassCreation) {
    DestinationTranslationTable table;

    // Check if exit port 0 is given when the table is empty
    EXPECT_EQ(table.translate("test"), 0);

    // Check is size function works
    EXPECT_EQ(table.size(), 0);
}

TEST(DestinationTranslationTable, TestAdding) {
    DestinationTranslationTable table;

    // Add a new entry
    table.add("test", 1);

    // Check if the entry is added
    EXPECT_EQ(table.translate("test"), 1);

    // Add multiple entries
    table.add("test2", 2);
    table.add("test3", 3);

    // Check if the entries are added
    EXPECT_EQ(table.translate("test2"), 2);
    EXPECT_EQ(table.translate("test3"), 3);
}

TEST(DestinationTranslationTable, TestRemove) {
    std::map<t_string, t_integer> translationTable{
        {"test", 1}, {"test2", 2}, {"test3", 3}};

    DestinationTranslationTable table{translationTable};

    // Check if the entries were added
    EXPECT_EQ(table.size(), 3);

    // Remove one entry
    table.remove("test");

    // Check if the entry was removed
    EXPECT_EQ(table.size(), 2);
    EXPECT_EQ(table.translate("test"), -1);
}

TEST(DestinationTranslationTable, TestTranslate) {
    std::map<t_string, t_integer> translationTable{
        {"test", 1}, {"test2", 2}, {"test3", 3}};

    DestinationTranslationTable table{translationTable};

    // Check if the entries were added
    EXPECT_EQ(table.size(), 3);

    // Check if the entries have correct values
    EXPECT_EQ(table.translate("test"), 1);
    EXPECT_EQ(table.translate("test2"), 2);
    EXPECT_EQ(table.translate("test3"), 3);
}

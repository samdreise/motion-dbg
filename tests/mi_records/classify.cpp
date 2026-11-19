#include <gtest/gtest.h>
#include "mi_record.h"


TEST(MiRecord, EachTypeCharacterMapsToItsRecordType) {
    EXPECT_EQ(Mi::ClassifyLine("^done").type,             Mi::RecordType::Result);
    EXPECT_EQ(Mi::ClassifyLine("*stopped").type,          Mi::RecordType::ExecAsync);
    EXPECT_EQ(Mi::ClassifyLine("=thread-created").type,   Mi::RecordType::NotifyAsync);
    EXPECT_EQ(Mi::ClassifyLine("+download").type,         Mi::RecordType::StatusAsync);
    EXPECT_EQ(Mi::ClassifyLine(R"(~"text")").type,        Mi::RecordType::Console);
    EXPECT_EQ(Mi::ClassifyLine(R"(@"text")").type,        Mi::RecordType::Target);
    EXPECT_EQ(Mi::ClassifyLine(R"(&"text")").type,        Mi::RecordType::Log);
}

TEST(MiRecord, TokenIsClassifiedCorrectly) {
    Mi::Record record = Mi::ClassifyLine("12^done");
    EXPECT_EQ(record.type, Mi::RecordType::Result);
    EXPECT_EQ(record.token, 12u);
    EXPECT_EQ(record.text, "12^done");
    EXPECT_EQ(record.cursor, 3u);
    EXPECT_EQ(record.text[record.cursor], 'd');
}

TEST(MiRecord, GdbPromptClassifiedCorrectly) {
    Mi::Record bare_prompt = Mi::ClassifyLine("(gdb)");
    Mi::Record spaced_prompt = Mi::ClassifyLine("(gdb) ");

    EXPECT_EQ(bare_prompt.type, Mi::RecordType::Prompt);
    EXPECT_EQ(spaced_prompt.type, Mi::RecordType::Prompt);
    EXPECT_EQ(bare_prompt.token, Mi::NO_TOKEN);
}

TEST(MiRecord, LineContainingOnlyDigitsIsOther) {
    Mi::Record record = Mi::ClassifyLine("12345");

    EXPECT_EQ(record.type, Mi::RecordType::Other);
    EXPECT_EQ(record.token, Mi::NO_TOKEN);
}



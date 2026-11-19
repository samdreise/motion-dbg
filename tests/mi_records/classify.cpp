#include <gtest/gtest.h>
#include "mi_records/mi_record.h"

TEST(MiRecord, EachTypeCharacterMapsToItsRecordType) {
    EXPECT_EQ(Mi::ClassifyLine("^done").type,             Mi::RecordType::Result);
    EXPECT_EQ(Mi::ClassifyLine("*stopped").type,          Mi::RecordType::ExecAsync);
    EXPECT_EQ(Mi::ClassifyLine("=thread-created").type,   Mi::RecordType::NotifyAsync);
    EXPECT_EQ(Mi::ClassifyLine("+download").type,         Mi::RecordType::StatusAsync);
    EXPECT_EQ(Mi::ClassifyLine(R"(~"text")").type,        Mi::RecordType::Console);
    EXPECT_EQ(Mi::ClassifyLine(R"(@"text")").type,        Mi::RecordType::Target);
    EXPECT_EQ(Mi::ClassifyLine(R"(&"text")").type,        Mi::RecordType::Log);
}


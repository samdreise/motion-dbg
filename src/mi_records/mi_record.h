#ifndef MDBG_SRC_MI_RECORD_MI_RECORD_H_
#define MDBG_SRC_MI_RECORD_MI_RECORD_H_


#include <string>
#include <cstdint>
#include <string_view>


namespace Mi {


constexpr size_t NO_TOKEN = SIZE_MAX;


enum class RecordType : uint8_t {

    Result,             // ^   reply to user cmd ^done, ^error, ^exit
    ExecAsync,          // *   execution state changed: *stopped, *running
    NotifyAsync,        // =   something changed: =thread-created, =breakpoint-modified

    // Stream Records
    StatusAsync,        // +   progress of a slow operation
    Console,            // ~   text gdb would print in a normal terminal
    Target,             // @   output from the debugged program (only on some targets)
    

    Log,                // &   gdb's log, e.g. the echo of a CLI command
    Prompt,             // (gdb)  marks the end of one batch of output
    Other               // none of the above (often the debugged program's own output)
};


struct Record {
    Mi::RecordType type;
    size_t token;
    std::string text;
    size_t cursor = 0;

    std::string_view offset_string_view() const {
        return std::string_view(text).substr(cursor);
    }
    
};

Record ClassifyLine(const std::string& raw_record);



} // namespace Mi

#endif // MDBG_SRC_VALUES_MI_LINE_CONTAINER_H_

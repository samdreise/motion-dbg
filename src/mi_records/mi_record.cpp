#include "mi_record.h"

Mi::Record Mi::Classify(const std::string& raw_record) {

    Record record;

    record.type = RecordType::Other; //default case = other
    record.text = raw_record;
    record.token = Mi::NO_TOKEN; 
    
    if (raw_record == "(gdb)" || raw_record == "(gdb) ") {
        record.type = RecordType::Prompt;
        return record;
    }
    
    // if there is a token it comes through in pos[0] of the record and is a digit
    // ie 12^done or 32=thread-created
    size_t i = 0;
    size_t token = 0;
    
    // using char arithmatic to detect integer digits
    while (i < raw_record.size() &&
           raw_record[i] >= '0'  &&
           raw_record[i] <= '9'  )
    {
        token = token * 10 + (raw_record[i] - '0');
        i++;
    }

    if (i == raw_record.size()) { // nothing but digits
        return record; // not a record so returning as { RecordType::Other, "<raw_record contents>", Mi::NO_TOKEN }
    }

    switch (raw_record[i]) {
        case '^': record.type = RecordType::Result;      break;
        case '*': record.type = RecordType::ExecAsync;   break;
        case '=': record.type = RecordType::NotifyAsync; break;
        case '+': record.type = RecordType::StatusAsync; break;
        case '~': record.type = RecordType::Console;     break;
        case '@': record.type = RecordType::Target;      break;
        case '&': record.type = RecordType::Log;         break;
        default:
            return record;   //  ie debugged program printing "Hello world" so output is { RecordType::Other, "Hello world" -1 }
    }
    
    if (i > 0) {
        record.token = token;
    }
    
    return record;

}


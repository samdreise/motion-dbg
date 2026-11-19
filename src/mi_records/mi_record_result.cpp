#include "mi_record_result.h"


void Mi::StackObjects::populate(Mi::Record &record) {

    if (record.type != Mi::RecordType::Result) {
        throw std::runtime_error("Incorrect record type");
    }

    get_local_var_names(record.offset_string_view());
    

}


bool Mi::StackObjects::get_local_var_names(std::string_view rec) {
    constexpr std::string_view key = "name=\"";

    size_t pos = rec.find(key);
    while (pos != std::string_view::npos) {
        size_t begin = pos + key.size();
        size_t end = rec.find('"', begin);
        if (end == std::string_view::npos) return false; //truncated or malformed record
        root_var_names_.push_back(rec.substr(begin, end - begin));
        pos = rec.find(key, end + 1);
    }
    
    return true;

}

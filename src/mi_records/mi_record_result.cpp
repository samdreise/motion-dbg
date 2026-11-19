#include "mi_record_result.h"





static inline Mi::ValueKind get_mi_value_kind(std::string_view& sv) {
    Mi::ValueKind kind;
    if (sv.empty()) kind = Mi::ValueKind::Invalid;
    switch (sv.front()) {
        case '"': kind = Mi::ValueKind::String;
        case '{': kind = Mi::ValueKind::Tuple;
        case '[': kind = Mi::ValueKind::List;
        default:  kind = Mi::ValueKind::Invalid;
    }
    

    return kind;
}

Mi::VarObject::VarObject(const Mi::Record& rec) {
     
    std::string_view sv = rec.offset_string_view();
    
    
    while(!sv.empty()) {
        VarField vf = consume_variable_field(sv);
        switch (vf) {
            case Mi::VarField::Name:    name = split_at_char(sv,'"');   break;
            case Mi::VarField::Type:    type = split_at_char(sv,'"');   break;
            case Mi::VarField::Dynamic: dynamic = true; break;
            case Mi::VarField::Value:
                Mi::ValueKind kind = get_mi_value_kind(sv);
                break;


        }
    }
    
}

















/*
void Mi::StackObjects::populate(Mi::Record &record) {

    if (record.type != Mi::RecordType::Result) {
        throw std::runtime_error("Incorrect record type");
    }

    get_local_var_names(record.offset_string_view());
    

}
*/

/*
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
*/

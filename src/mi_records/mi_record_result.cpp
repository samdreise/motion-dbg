#include "mi_record_result.h"
#include "string_helpers.h"



static inline Mi::ValueKind get_mi_value_kind(std::string_view& sv) {
    Mi::ValueKind kind;
    if (sv.empty()) kind = Mi::ValueKind::Invalid;
    switch (sv.front()) {
        case '\"': kind = Mi::ValueKind::String;    break;
        case '{': kind = Mi::ValueKind::Tuple;      break;
        case '[': kind = Mi::ValueKind::List;       break;
        default:  kind = Mi::ValueKind::Invalid;
    }
    
    return kind;
}

void Mi::VarObject::ParseMiValueField(std::string_view& sv) {
    Mi::ValueKind kind = get_mi_value_kind(sv);
    if (kind != Mi::ValueKind::Invalid) {
        switch(kind) {
            case Mi::ValueKind::String: value = cCreateUnescapedString(sv);      break;
            case Mi::ValueKind::List:   value = "<list>";                       break;
            case Mi::ValueKind::Tuple:  value = "<tuple>";                      break;
            case Mi::ValueKind::Invalid:  value = "<Invalid>";                  break;
        }
    }
    
}


static inline size_t cConsumeNumericValue(std::string_view& sv) {
    size_t n = sv.find('\"');
    sv.remove_prefix(n + 1);     // skip over '"'
    return cStringViewToSizeT(sv);
}

static inline std::string_view cConsumeConstCString(std::string_view& sv) {
    cConsumeUntilAlpha(sv);
    std::string_view huh = cSplitAtChar(sv, '\"');
    return huh;
}

Mi::VarObject::VarObject(Mi::Record& rec) {
     
    std::string_view sv = rec.offset_string_view();
    sv = TEMPORARY_POP_DONE(rec);
    while(!sv.empty()) {
        VarField vf = consume_variable_field(sv);
        switch (vf) {
            case Mi::VarField::Name:        name = cConsumeConstCString(sv);            break;
            case Mi::VarField::Type:        type = cConsumeConstCString(sv);            break;
            case Mi::VarField::Exp:         expression = cConsumeConstCString(sv);      break;
            case Mi::VarField::NumChild:    numchild = cConsumeNumericValue(sv);        break;
            case Mi::VarField::ThreadId:    threadid = cConsumeNumericValue(sv);        break;
            case Mi::VarField::Value:       ParseMiValueField(sv);                      break;
            case Mi::VarField::Dynamic:     dynamic = true;                             break;
            case Mi::VarField::HasMore:     has_more = cConsumeNumericValue(sv);        break;
        }
        
        if(!cConsumeUntilAlpha(sv)) break;
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

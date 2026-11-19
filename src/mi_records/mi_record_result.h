#ifndef MDBG_SRC_PARSE_MI_RESULT_H_
#define MDBG_SRC_PARSE_MI_RESULT_H_

#include <string>
#include <string_view>
#include <memory>
#include "mi_record.h"
#include <unordered_map>
#include <vector>
#include <cstdint>


namespace Mi {


enum class ValueKind : uint8_t {
    Invalid,
    String,
    Tuple,
    List
};

struct Value {
    std::string text;
    Mi::ValueKind kind;
};

struct SVHash {
    using is_transparent = void;
    size_t operator()(std::string_view sv) const noexcept { 
        return std::hash<std::string_view>{}(sv); 
    }
};



struct VarObject {
    std::string name;
    std::string expression;
    std::string type;
    Mi::Value value;
    VarObject* parent = nullptr;

    std::vector<std::unique_ptr<VarObject>> children;
    
    bool hidden = false;
    bool in_scope = true;
    bool changed = false;
    bool dynamic = false;
    bool has_more = false;
};


class StackObjects {
public:
    void populate(Mi::Record& rec);
    
    

private:
    
    std::vector<std::unique_ptr<VarObject>> root_var_objects_;
    std::unordered_map<std::string, Mi::VarObject*, Mi::SVHash, std::equal_to<>> var_map_;
    std::vector<VarObject*> changed_;
    std::vector<std::string_view> root_var_names_;

    
    bool get_local_var_names(std::string_view rec);
    
};












inline std::string_view consume_till_char(std::string_view& sv, const char c) {

    size_t pos = sv.find(c);
    if (pos == std::string_view::npos) {
        // at end so consume everything that's left.
        std::string_view rest = sv;
        sv = {};
        return rest;
    }

    std::string_view field_value = sv.substr(0, pos);
    sv.remove_prefix(pos + 1);
    return field_value;

}

inline bool consume_string(std::string_view& sv, const std::string_view key) {

    size_t pos = sv.find(key);
    if (pos == std::string_view::npos) return false;
    sv.remove_prefix(pos + key.size());
    return true;

}




enum class VarField : uint8_t {
    Invalid,    // malformed input: no '=' found
    Unknown,    // well-formed, but a variable we don't handle (skip its value)

    Name,
    Value,
    Type,
    ThreadId,

    NumChild,
    Children,
    Child,

    DisplayHint,
    Dynamic,
    HasMore,
    Exp
};

// MI grammar: { "variable"="value", }
struct VarFieldEntry {
    std::string_view variable;  //variable / key
    VarField field;          //value (variable = value)
};

inline constexpr VarFieldEntry kVarFields[] = {
    {.variable = "name",        .field = VarField::Name},
    {.variable = "value",       .field = VarField::Value},
    {.variable = "type",        .field = VarField::Type},
    {.variable = "thread-id",   .field = VarField::ThreadId},
    {.variable = "numchild",    .field = VarField::NumChild},
    {.variable = "children",    .field = VarField::Children},
    {.variable = "child",       .field = VarField::Child},
    {.variable = "displayhint", .field = VarField::DisplayHint},
    {.variable = "dynamic",     .field = VarField::Dynamic},
    {.variable = "has_more",    .field = VarField::HasMore},
    {.variable = "exp",         .field = VarField::Exp},
};

inline VarField consume_variable(std::string_view& sv) {

    consume_till_char(sv, ',');

    size_t eq_pos = sv.find('=');
    if (eq_pos == std::string::npos) {
        return Mi::VarField::Invalid;
    }

    std::string_view variable = sv.substr(0, eq_pos);

    sv.remove_prefix(eq_pos + 1); // remove front of string_view until the char after the '=' sign
    
    for (const auto& var_obj_field : kVarFields) {
        if (variable == var_obj_field.variable) { // comparing the strings

            return var_obj_field.field; // strings match so return the enumed type
        }
    }
    

    return Mi::VarField::Unknown;   // fallthrough case is ::Unknown
}



inline std::string_view consume_value_field(std::string_view& sv) {
    Mi::Value value;
    if (sv.empty()) value.kind = Mi::ValueKind::Invalid;
    switch (sv.front()) {
        case '"': value.kind = ValueKind::String;
        case '{': value.kind = ValueKind::Tuple;
        case '[': value.kind = ValueKind::List;
        default:  value.kind = ValueKind::Invalid;
    }


}

inline VarObject createVarObject(const Mi::Record& rec) {
     
    std::string_view sv = rec.offset_string_view();
    VarObject var;
   
    
    while(!sv.empty()) {
        VarField vf = consume_variable(sv);
        switch (vf) {
            case Mi::VarField::Name:    var.name = consume_till_char(sv,'"');   break;
            case Mi::VarField::Type:    var.type = consume_till_char(sv,'"');   break;
            case Mi::VarField::Value:   var.value = consume_value_field(sv);    break; 
            case Mi::VarField::Dynamic: var.dynamic = true; break;
            default: consume_till_char(sv,'"');
        }
    }
    
    return var;
}

} // namespace Mi


#endif // MDBG_SRC_PARSE_MI_RESULT_H_

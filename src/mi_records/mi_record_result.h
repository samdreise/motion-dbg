#ifndef MDBG_SRC_PARSE_MI_RESULT_H_
#define MDBG_SRC_PARSE_MI_RESULT_H_

#include <string>
#include <string_view>
#include <memory>
#include "mi_record.h"
#include <unordered_map>
#include <vector>
#include <cstdint>
#include "string_helpers.h"



namespace Mi {



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
struct VarFieldParseKey {
    std::string_view variable;  //variable / key
    VarField field;          //value (variable = value)
};

// helper array to pass all name=, numchild=. exp= etc from a ^ miline
inline constexpr VarFieldParseKey kVarFieldParseKeys[] = {
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

// if VarField = value, then Value's "kind" must be of the following types:
enum class ValueKind : uint8_t {
    Invalid,
    String,
    Tuple,
    List
};

/*
struct Value {
    std::string text;
    Mi::ValueKind kind; 
    std::vector<std::string> array;
};
*/

class VarObject {
public:
    VarObject(Mi::Record& record);
    void ParseMiValueField(std::string_view& sv);

    std::string name;
    std::string expression;
    std::string type;
    std::string value;

    std::vector<std::unique_ptr<VarObject>> children;
    size_t numchild = 0;
    size_t threadid = 0;
    bool hidden = false;
    bool in_scope = true;
    bool changed = false;
    bool dynamic = false;
    bool has_more = false;
    
private:
    
    VarObject* parent = nullptr;
};

inline std::string_view TEMPORARY_POP_DONE(Mi::Record &record) {
    std::string_view sv = record.offset_string_view();
    cSplitAtChar(sv, ',');
    return sv;
}

inline VarField consume_variable_field(std::string_view& sv) {
    

    std::string_view variable = cSplitAtChar(sv, '=');
     
    if (sv.empty()) {
        return Mi::VarField::Invalid;
    }

    for (const auto& var_obj_field : kVarFieldParseKeys) {
        if (variable == var_obj_field.variable) { // comparing the strings

            return var_obj_field.field; // strings match so return the enumed type
        }
    }
    

    return Mi::VarField::Unknown;   // fallthrough case is ::Unknown
}


inline std::vector<std::string_view> parse_stack_list_locals(std::string_view &sv) {
    cSplitAtChar(sv, '[');
    std::vector<std::string_view> stack_locals;
    while (!sv.empty()) {
        std::string_view key = "name=";
        if (cSplitAtChar(sv, '"') != key) { // consume the name=" component
            return stack_locals;
        }
        stack_locals.push_back(cSplitAtChar(sv, '"')); // push_back the name
        
        sv.remove_prefix(1); // remove the ',' at front of string
        
    }

    return stack_locals;
}











} // namespace Mi


#endif // MDBG_SRC_PARSE_MI_RESULT_H_

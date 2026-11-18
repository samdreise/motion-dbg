#ifndef MDBG_SRC_PARSE_MI_RESULT_H_
#define MDBG_SRC_PARSE_MI_RESULT_H_

#include <string>
#include <string_view>
#include <memory>
#include "mi_record.h"
#include <unordered_map>
#include <vector>


namespace Mi {

struct SVHash {
    using is_transparent = void;
    size_t operator()(std::string_view sv) const noexcept { 
        return std::hash<std::string_view>{}(sv); 
    }
};


enum class ValueKind {
    Invalid,
    String,
    Tuple,
    List
};


struct Node {
    Mi::ValueKind kind = Mi::ValueKind::Invalid;
    std::string_view variable;
    std::string_view raw;
    std::vector<Mi::Node> children;
};




struct VarObject {
    std::string name;
    std::string expression;
    std::string type;
    std::string value;
    VarObject* parent = nullptr;

    std::vector<std::unique_ptr<VarObject>> children;
    
    bool hidden = false;
    bool in_scope = true;
    bool changed = false;
    bool dynamic = false;
    bool has_more = false;
};


class StackObjects {

private:
    
    std::vector<std::unique_ptr<VarObject>> var_objects_;
    std::unordered_map<std::string, Mi::VarObject*, Mi::SVHash, std::equal_to<>> var_map_;
    std::vector<VarObject*> changed_;

};


} // namespace Mi


#endif // MDBG_SRC_PARSE_MI_RESULT_H_

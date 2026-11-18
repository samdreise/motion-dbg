#ifndef MDBG_SRC_PARSE_MI_RESULT_H_
#define MDBG_SRC_PARSE_MI_RESULT_H_

#include <string>
#include <string_view>
#include "mi_record_container.h"
#include <vector>


namespace Mi {

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




class VarObject {
public:
private:

};



} // namespace Mi


#endif // MDBG_SRC_PARSE_MI_RESULT_H_

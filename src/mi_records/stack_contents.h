#define MDBG_SRC_MI_RECORDS_STACK_CONTENTS_H_
#ifndef MDBG_SRC_MI_RECORDS_STACK_CONTENTS_H_
#define MDBG_SRC_MI_RECORDS_STACK_CONTENTS_H_

#include <string>
#include <string_view>

struct SVHash {
    using is_transparent = void;
    size_t operator()(std::string_view sv) const noexcept { 
        return std::hash<std::string_view>{}(sv); 
    }
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


#endif // MDBG_SRC_MI_RECORDS_STACK_CONTENTS_H_

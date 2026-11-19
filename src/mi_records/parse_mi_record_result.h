#ifndef MDGB_SRC_MI_RECORD_PARSE_MI_RECORD_RESULT_H_
#define MDGB_SRC_MI_RECORD_PARSE_MI_RECORD_RESULT_H_

#include "mi_record.h"
#include "mi_record_result.h"
#include <cstring>

namespace Mi {


inline std::vector<std::string_view> get_local_var_names(std::string_view rec) {
    constexpr std::string_view key = "name=\"";
    std::vector<std::string_view> var_names;

    size_t pos = rec.find(key);
    while (pos != std::string_view::npos) {
        size_t begin = pos + key.size();
        size_t end = rec.find('"', begin);
        if (end == std::string_view::npos) break; //truncated or malformed record
        var_names.push_back(rec.substr(begin, end - begin));
        pos = rec.find(key, end + 1);
    }

    return var_names;

}



}





#endif // MDGB_SRC_MI_RECORD_PARSE_MI_RECORD_RESULT_H_

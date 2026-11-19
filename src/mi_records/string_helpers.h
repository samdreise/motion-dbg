#ifndef MDBG_SRC_MI_RECORDS_STRING_HELPERS_H_
#define MDBG_SRC_MI_RECORDS_STRING_HELPERS_H_

#include <string_view>
#include <cctype>
/**
 * @brief - consume's a string view up until a char c, where the string between
 * the sv.start() and string_view.find(c) is returned as a 'field_value' string_view
 * and the original string has it's prefix removed up to and including the char c.
 *
 * @example - std::string_view sv = "first-second"
 * consume_till_char(sv, '-') returns "first"   and   sv = "second"
 *
 * @retval - substring of all chars before the char c was found (not including c)
 * 
 * 
*/
inline std::string_view split_at_char(std::string_view& sv, const char c) {


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

inline bool consume_until_alpha(std::string_view &sv) {
    bool found_char = false;
    while(!sv.empty() && !(found_char = std::isalpha(sv[0]))) {
        sv.remove_prefix(1);
    }

    return found_char;
}






/*
inline bool consume_string(std::string_view& sv, const std::string_view key) {

    size_t pos = sv.find(key);
    if (pos == std::string_view::npos) return false;
    sv.remove_prefix(pos + key.size());
    return true;

}
*/


#endif // MDBG_SRC_MI_RECORDS_STRING_HELPERS_H_

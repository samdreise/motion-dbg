#ifndef MDBG_SRC_MI_RECORDS_STRING_HELPERS_H_
#define MDBG_SRC_MI_RECORDS_STRING_HELPERS_H_

#include <string_view>
#include <string>
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


static inline __attribute__((always_inline)) bool IsOctalDigit(char character) {
    return character >= '0' && character <= '7';
}


inline bool ParseAsStringLiteral(const std::string_view& text, size_t& pos, std::string& out) {

    if (pos >= text.size() || text[pos] != '"') {
        return false;
    }

    std::string unescaped;
    size_t cursor = pos + 1; // walks the text; pos is only updated once we know the whole string is valid

    while (cursor < text.size()) {
        char current = text[cursor]; 

        if (current == '"') { // unescaped quote: end of the string
            out = unescaped; // value of string
            pos = cursor + 1;// generally keep parsing from here
            return true;
        }

        // looking for \n \r \\ \0 and escape sequences etc
        if (current != '\\') { // ordinary character
            unescaped.push_back(current);
            cursor++;
            continue;
        }

        cursor++; // step onto the character after the backslash
        if (cursor >= text.size()) {
            return false; // text ended right after a backslash
        }

        char escaped = text[cursor];

        // three octal digits are one byte ie \303 is 0xC3
        bool is_octal_escape = IsOctalDigit(escaped) &&             //  3
                               cursor + 2 < text.size() && // make sure we don't read past end of string (ewpsy)
                               IsOctalDigit(text[cursor + 1]) &&    //  0
                               IsOctalDigit(text[cursor + 2]);      //  3  <- read an octal


        if (is_octal_escape) {
            int byte_value  = (escaped - '0') * 64              // c0 * 8^2
                            + (text[cursor + 1] - '0') * 8      // c1 * 8^1
                            + (text[cursor + 2] - '0');         // c2 * 8^0
            
            
            unescaped.push_back(static_cast<char>(byte_value & 0xFF)); // converted to hex to display as a byte -> 0xC3

            cursor += 3; // consume all 3 chars in the octal and read from next
            continue;
        }

        switch (escaped) {
            case 'n': unescaped.push_back('\n'); break;
            case 't': unescaped.push_back('\t'); break;
            case 'e': unescaped.push_back('\033'); break; // an escape sequence for a gdb color code (ESC = byte 27)
            default:  unescaped.push_back(escaped); break; // covers \" and \\ + keeps char of any other escape
        }
        cursor++;
    }

    return false; // no closing quote before end of string
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

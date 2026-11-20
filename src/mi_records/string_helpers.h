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
inline std::string_view cSplitAtChar(std::string_view& sv, const char c) {


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

inline bool cConsumeUntilAlpha(std::string_view &sv) {
    bool found_char = false;
    while(!sv.empty() && !(found_char = std::isalpha(sv[0]))) {
        sv.remove_prefix(1);
    }

    return found_char;
}


static inline __attribute__((always_inline)) bool IsOctalDigit(char character) {
    return character >= '0' && character <= '7';
}



/* 
 * @brief - pass in a string_view to return an escape stripped string 
 *
 * @usage - pass in a string_view of a record which has been consumed up
 * till the first " of the string, ie, the record:
 *
 * value="\"Hello\\n\"",type="std::string", ...
 *
 * should be passed through as:
 *
 * "\"Hello\\n\"",type="std::string", ...
 *
 *  "Hello\n" will then be returned and sv will be consumed up until 
 *  the ',' before type so sv = ",type="std::string", ... "
*/
inline std::string cCreateUnescapedString(std::string_view& sv) {

    if (sv[0] != '"') {
        return "<Malformed string>";
    }

    std::string unescaped;
    size_t n = 1; // start at 1 to skip first "

    while (n < sv.size()) {
        char current = sv[n]; 


        /* check first if the current char is a " as if there is a preceding \ to
        escape any given " this " will never have been read as we skip over it
        intentionally to ensure we read till end of string literal */


        if (current == '"') { // unescaped quote: end of the string
            sv.remove_prefix(n); // remove parsed string literal from from front
            return unescaped;
        }

        // looking for \" \n \r \\ \0 and escape sequences etc
        if (current != '\\') { // ordinary character
            unescaped.push_back(current);
            n++;
            continue;
        }

        n++; // step onto the character after the backslash
        if (n >= sv.size()) {
            sv = {};
            return unescaped + " ... <MDGB> parsed record malformed (ended after a backslash)"; // text ended right after a backslash
            
        }

        char escape_char_type = sv[n];

        // three octal digits are one byte ie \303 is 0xC3
        bool is_octal_escape = IsOctalDigit(escape_char_type) &&             //  3
                               n + 2 < sv.size() && // make sure we don't read past end of string (ewpsy)
                               IsOctalDigit(sv[n + 1]) &&    //  0
                               IsOctalDigit(sv[n + 2]);      //  3  <- read an octal


        if (is_octal_escape) {
            int byte_value  = (escape_char_type - '0') * 64              // c0 * 8^2
                            + (sv[n + 1] - '0') * 8      // c1 * 8^1
                            + (sv[n + 2] - '0');         // c2 * 8^0
            
            
            unescaped.push_back(static_cast<char>(byte_value & 0xFF)); // converted to hex to display as a byte -> 0xC3

            n += 3; // consume all 3 chars in the octal and read from next
            continue;
        }

        switch (escape_char_type) {
            case 'n': unescaped.push_back('\n'); break;
            case 't': unescaped.push_back('\t'); break;
            case 'e': unescaped.push_back('\033'); break; // an escape sequence for a gdb color code (ESC = byte 27)
            default:  unescaped.push_back(escape_char_type); break; // covers \" and \\ + keeps char of any other escape
        }
        n++;
    }

    sv = {};
    return unescaped + " ... <MGDB> parsed record malformed (non-terminating string)"; // no closing quote before end of string
    
}

inline size_t cStringViewToSizeT (std::string_view& sv) {
    // using char arithmatic to detect integer digits
    size_t num = 0;

    while (!sv.empty() && sv.front() >= '0' && sv.front() <= '9'  )
    {
        num = num * 10 + (sv.front() - '0');
        sv.remove_prefix(1);
    }

    return num;
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

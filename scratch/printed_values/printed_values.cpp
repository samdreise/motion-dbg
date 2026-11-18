#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <charconv>
#include <string_view>
#include <cstdint>
#include <optional>
#include <cstring>
#include <stdexcept>
#include <variant>


namespace Mi {
enum class Kind {
    INVALID,
    STRING,
    TUPLE,
    LIST
};

struct Node {
    Kind kind = Kind::INVALID;
    std::string_view key;
    std::string_view text;
    std::vector<Node> children;
};




}


constexpr bool IsOctalDigit(char c) noexcept { return c>= '0' && c <= '7'; }

std::string ParseAsStringLiteral(std::string_view s) {
    std::string out;
    out.reserve(s.size());
    
    for (size_t i = 0; i < s.size(); ++i) {
        char c = s[i];

        if (c != '\\') {        // not dealing with escape sequence
            out.push_back(c);   // add char then keep reading
            continue;
        }

        i++;        // increment past the backslash to read the next char
        const char escaped = s[i];

        if (IsOctalDigit(escaped) && i + 2 < s.size()) { // ensuring we will not read past end of string
            const char octal[3] = {
                s[i],
                s[i + 1],
                s[i + 2]
            };
            
            if (IsOctalDigit(octal[0]) &&
                IsOctalDigit(octal[1]) && 
                IsOctalDigit(octal[2])) {
                std::uint_fast16_t octal_byte_value = 0;
                
                octal_byte_value =  (octal[0] - '0') * 64; // c0 * 8^2
                octal_byte_value += (octal[1] - '0') * 8;  // c1 * 8^1
                octal_byte_value += (octal[2] - '0');      // c2 * 8^0
                
                if(octal_byte_value > 0377) throw std::runtime_error("octal escape out of byte range");

                out.push_back(static_cast<char>(octal_byte_value & 0xFF));
                i += 2;

                continue;
            }
            
        }
        switch (escaped) {
            case 'n':  out.push_back('\n'); break;
            case 't':  out.push_back('\t'); break;
            case 'r':  out.push_back('\r'); break;
            case 'a':  out.push_back('\a'); break;
            case 'b':  out.push_back('\b'); break;
            case 'f':  out.push_back('\f'); break;
            case 'v':  out.push_back('\v'); break;
            case 'e':  out.push_back('\x1b'); break;  // GDB extension
            case '"':  out.push_back('"');  break;
            case '\\': out.push_back('\\'); break;
            default:   out.push_back(escaped); break;  // be lenient: unknown escape, keep the char
        }

    }

    return out;
}


int main() {
    return 0;
}

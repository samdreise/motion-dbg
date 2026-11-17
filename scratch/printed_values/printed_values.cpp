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

struct PrintedValue {
    std::string name;
    std::optional<std::string> value;
    std::optional<std::string> type;

    std::vector<PrintedValue> children;
    bool is_arg = false;
};

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

// Reads a c-string starting at s[pos], which must be the opening '"'.
// Leaves pos just past the closing '"' and returns the unescaped contents.
std::string ParseCString(std::string_view s, size_t& pos) {
    if (pos >= s.size() || s[pos] != '"') throw std::runtime_error("expected '\"'");
    pos++;                                  // past the opening quote

    const size_t start = pos;
    while (pos < s.size() && s[pos] != '"') {
        if (s[pos] == '\\') pos++;          // skip the escaped char, so \" doesn't end the string
        pos++;
    }
    if (pos >= s.size()) throw std::runtime_error("unterminated string");

    std::string_view raw = s.substr(start, pos - start);
    pos++;                                  // past the closing quote
    return ParseAsStringLiteral(raw);
}

// Reads a tuple starting at s[pos], which must be '{'.
// Leaves pos just past the matching '}'. A value that is itself a tuple becomes a child.
PrintedValue ParsePrintedValue(std::string_view s, size_t& pos) {
    if (pos >= s.size() || s[pos] != '{') throw std::runtime_error("expected '{'");
    pos++;                                  // past the '{'

    PrintedValue v;
    while (pos < s.size() && s[pos] != '}') {
        const size_t eq = s.find('=', pos);
        if (eq == std::string_view::npos) throw std::runtime_error("expected '='");

        std::string_view key = s.substr(pos, eq - pos);
        pos = eq + 1;                       // past the '='

        if (pos < s.size() && s[pos] == '{') {
            v.children.push_back(ParsePrintedValue(s, pos));   // recurse, advances pos past its '}'
        } else {
            std::string val = ParseCString(s, pos);            // advances pos past the closing '"'
            if      (key == "name")  v.name  = std::move(val);
            else if (key == "type")  v.type  = std::move(val);
            else if (key == "value") v.value = std::move(val);
            else if (key == "arg")   v.is_arg = (val == "1");
            // else ignore unknown keys
        }

        if (pos < s.size() && s[pos] == ',') pos++;
    }
    if (pos >= s.size()) throw std::runtime_error("unterminated tuple, expected '}'");
    pos++;                                  // past the '}'

    return v;
}

void Print(const PrintedValue& v, int depth = 0) {
    std::string indent(depth * 2, ' ');
    std::cout << indent << "name=" << v.name
              << " type=" << v.type.value_or("<none>")
              << " value=" << v.value.value_or("<none>")
              << (v.is_arg ? " (arg)" : "") << '\n';
    for (const PrintedValue& child : v.children) Print(child, depth + 1);
}

int main() {
    std::string path("./parse_printed_value_input");
    std::ifstream file(path, std::ios::binary);
    if (!file) { std::cerr << "could not open " << path << '\n'; return 1; }

    std::vector<std::string> contents;
    std::string line{};
    for (size_t i = 0; i < 8 && std::getline(file, line); ++i) {
        contents.push_back(line);
    }

    for (const std::string& l : contents) {
        try {
            size_t pos = 0;
            PrintedValue v = ParsePrintedValue(l, pos);
            if (pos != l.size()) std::cerr << "trailing text after value at col " << pos << '\n';
            Print(v);
        } catch (const std::exception& e) {
            std::cerr << "parse error: " << e.what() << " in: " << l << '\n';
        }
    }

    return 0;
}

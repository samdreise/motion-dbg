#include "printed_value.h"

namespace Val{
    


static inline __attribute__((always_inline)) bool ParseCount(const std::string& nums, size_t &out) {

    if (nums.empty() || nums.size() > 18) {
        return false;
    }
    
    size_t value = 0;
    for (char c : nums) {
        if (c < '0' || c > '9') { 
            return false;           // nums contains a non digit char
        }
        value = value * 10 + static_cast<size_t>(c - '0');
    }

    out = value;
    return true;
}

static inline __attribute__((always_inline)) bool IsNamedCharacter(char c) {
    bool is_letter = (  (c >= 'a' && c <= 'z') ||
                        (c >= 'A' && c <= 'Z')  );
    bool is_digit = c >= '0' && c <= '9';
    
    bool is_syntax = false;
    
    if (c == '_' ||
        c == '.' ||
        c == ':' ||
        c == '~' ||
        c == '(' ||
        c == ')')
    {
        is_syntax = true;
    }

    return is_letter || is_digit || is_syntax;

}

static inline __attribute__((always_inline)) bool TokenStart(const std::string& text, size_t index) {
    if (index == 0) {
        return true;
    }
    char prev = text[index - 1];
    
    return  prev == ' ' || prev == '{' ||
            prev == ',' || prev == '=';     // comma
}




// PLACEHOLDER so the tests link: every text is a leaf for now (the header's rule for text that is not a list of
// members or elements). Replace with the real parser, one test at a time.
PrintedValue ParsePrintedValue(const std::string& text) {
    PrintedValue value;
    value.text = text;
    return value;
}






}










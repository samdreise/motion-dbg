
#ifndef MDBG_SRC_VALUES_PRINTED_VALUE_H_
#define MDBG_SRC_VALUES_PRINTED_VALUE_H_

#include <string>
#include <vector>

/* Value printed by gdb, parsed into a tree from the output(s)
"-stack-list-variables --all-values" cmd gives every local's whole printed value with contents inside

examples:

    {id = 1, name = "bolt", position = {x = 0.5, y = 0}}            struct w named members id, name, position
    {1, 2, 3}   {0 <repeats 15 times>, 7}   {0, 1, 2...}            an array and it's positions
                
    std::vector of length 2, capacity 2 = {1, 2}                            a pretty printer: its summary, then the elements
    std::map with 2 elements = {["one"] = 1, ["two"] = 2}                   ... or keys
    {<Base> = {pub = 1}, extra = 4}                                         a base class is a member named <Base>
    @0x7ffe0010: {id = 1}                                                   a reference: the address, then the object

*/


namespace Val {
    
    struct PrintedValue {
        std::string name = "";
        std::string text = "";
        std::vector<PrintedValue> children{};
        bool incomplete = false;    // gdb prints "..." after 200 elements
    };

    PrintedValue ParsePrintedValue(const std::string &test);

} // namespace Val

#endif // MDBG_SRC_VALUES_PRINTED_VALUE_H_

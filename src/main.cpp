#include <iostream>
#include "mi_record.h"
#include "mi_record_result.h"
#include "test_cmd_sender.h"
#include "string_helpers.h"


int main() {
    std::string nums = "123hey";
    std::string_view sv = nums;
    size_t n = cStringViewToSizeT(sv);
    std::cout << n << "\n";
    std::string string1 = "Hello-string";
    std::string_view sv1 = string1;
    std::string string2 = "+-)";
    std::string_view sv2 = string2;
    cSplitAtChar(sv1, '-');
    bool found_char = cConsumeUntilAlpha(sv2);


    TestCmdSender sender("src/file.txt");
    Mi::Record stack_list_locals = Mi::ClassifyLine(sender.send());
    Mi::Record var_create_var1 = Mi::ClassifyLine(sender.send());
    std::string_view locals = Mi::TEMPORARY_POP_DONE(stack_list_locals);
   

    std::vector<std::string_view> locallist = Mi::parse_stack_list_locals(locals);

    Mi::VarObject var(var_create_var1);
    
    return 0;
}

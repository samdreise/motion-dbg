#include <iostream>
#include "mi_record.h"
#include "mi_record_result.h"
#include "test_cmd_sender.h"
#include "string_helpers.h"


int main() {
    std::string string1 = "Hello-string";
    std::string_view sv1 = string1;
    std::string string2 = "+-)";
    std::string_view sv2 = string2;
    split_at_char(sv1, '-');
    bool found_char = consume_until_alpha(sv2);


    TestCmdSender sender("src/file.txt");
    Mi::Record stack_list_locals = Mi::ClassifyLine(sender.send());
    Mi::Record var_create_var1 = Mi::ClassifyLine(sender.send());
    std::string_view locals = Mi::TEMPORARY_POP_DONE(stack_list_locals);
   

    std::vector<std::string_view> locallist = Mi::parse_stack_list_locals(locals);
    
    return 0;
}

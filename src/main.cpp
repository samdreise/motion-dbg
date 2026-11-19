#include <iostream>
#include "mi_record.h"
#include "mi_record_result.h"
#include "test_cmd_sender.h"

int main() {

    TestCmdSender sender("src/file.txt");
    Mi::StackObjects stack;
    Mi::Record stack_list_locals = Mi::ClassifyLine(sender.send());
    Mi::Record var_create_var1 = Mi::ClassifyLine(sender.send());
    stack.populate(stack_list_locals);

    Mi::createVarObject(var_create_var1);
    std::string_view str = R"(,")";
    Mi::VarField field = Mi::consume_variable(str);
    return 0;
}

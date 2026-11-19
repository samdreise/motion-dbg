#include <iostream>
#include "mi_record.h"
#include "test_cmd_sender.h"

int main() {

    TestCmdSender sender("src/file.txt");
    std::cout << sender.send() << "\n";

    return 0;
}

#ifndef MDBG_SRC_MI_RECORDS_TEST_CMD_SENDER_H_
#define MDBG_SRC_MI_RECORDS_TEST_CMD_SENDER_H_

#include <fstream>
#include <vector>
#include <string>
#include <iostream>


class TestCmdSender {
public:
    TestCmdSender(const std::string &path) {
        std::ifstream file;
        std::string line;
        file.open(path, std::ios::binary);
        
        if (!file) {
            std::cerr << "Could not open file\n";
        }
    
        while(std::getline(file, line)) {
            cmds_.push_back(line);
        }
        
        file.close();
    }

    std::string& send() {
        return cmds_[cmd_counter_++];
    }

private:
    std::vector<std::string> cmds_;
    size_t cmd_counter_ = 0;
};


#endif

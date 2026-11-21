#include "gdb_handler.h"


#include <cerrno>
#include <chrono>
#include <csignal>
#include <cstdio>
#include <iterator>

#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>


using Mi::Record;

Mdbg::Handler::~Handler() {
    Stop();
}


bool Mdbg::Handler::Start(const std::string& debug_elf, const std::vector<std::string>& program_arguments) {

    if (pid_ != -1) {
        return false;
    }

    // gdb's command line
    std::vector<std::string> cmd = { GDB_MI3_ARGS };
    if (!program_arguments.empty()) {
        cmd.emplace_back("--args");
    }
    if (!debug_elf.empty()) {
        cmd.push_back(debug_elf);
    }
    for (const std::string& argument : program_arguments) {
        cmd.push_back(argument);
    }

    std::vector<char*> gdb_argv;
    for (std::string& word : cmd) {
        gdb_argv.push_back(word.data());
    }
    gdb_argv.push_back(nullptr);

    //defs:
    //  SIGPIPE -> write on pipe with no reader process (terminating failure usually)
    //  SIG_IGN -> request this failure be ignored
    signal(SIGPIPE, SIG_IGN); // ignore SIGPIPE for entire process so that a write to gdb after exit fails with errno == EPIPE



    int to_gdb[2];     // [0] gdb read       [1] write pipe
    int from_gdb[2];   // [1] gdb write      [0] read pipe

    // O_CLOEXEC the pipe ends close themselves when gdb is exec'd
    if (pipe2(to_gdb, O_CLOEXEC) == -1) {
        perror("Mdbg::Handler::start -> pipe to_gdb failed");
        return false;
    }

    if (pipe2(from_gdb, O_CLOEXEC) == -1) {
        perror("Mdbg::Handler::start -> pipe from_gdb failed");
        close(to_gdb[0]);
        close(to_gdb[1]);
        return false;
    }



    pid_t pid = fork(); // #### ---- FORK AROUND AND FIND OUT ---- #### //

    if (pid == -1) {
        perror("Mdbg::Handler::start -> process forking failed");
        close(to_gdb[0]);
        close(to_gdb[1]);
        close(from_gdb[0]);
        close(from_gdb[1]);
        return false;
    }


    //child
    if (pid == 0) {
        dup2(to_gdb[0], STDIN_FILENO);    // copy gdb read fd to ./gdb process's STDIN
        dup2(from_gdb[1], STDOUT_FILENO); // copy gdb write fd to ./gdb process's STDOUT


        // establish default SIGPIPE behaviour for child process as the parent used SIG_IGN
        signal(SIGPIPE, SIG_DFL); 

        // close all pipe ends as gdb will use STDIN and OUT for r/w
        close(to_gdb[0]);
        close(to_gdb[1]);
        close(from_gdb[0]);
        close(from_gdb[1]);


        execvp("gdb", gdb_argv.data());


        //fell through error handling
        perror("Mdbg::Handler::start -> execvp gdb fell through");
        _exit(COMMAND_NOT_FOUND); // (code 127)
    }


    
    close(to_gdb[0]);
    close(from_gdb[1]);


    pid_            = pid; // copy PID of the child process (gdb)
    fd_to_gdb_      = to_gdb[1];
    fd_from_gdb_    = from_gdb[0];


    // gdb is setup ready to write and read, pipe ends have been configured ->
    reader_done_ = false;
    reader_thread_ = std::thread(&Mdbg::Handler::ReaderLoop, this);

    return true;
}

/**
 * @brief function that returns weather or not the Mdbg Handler's reader thread is currently running
 *
 * @retval - TRUE if running - FALSE if not
 *
 * @code
 *
 *  - `pid_` != -1                 //process is spawned
 *  - `!reader_done_` == false     //read thread is running
 *
 * @endcode
 */
bool Mdbg::Handler::ProcessActive() const {
    bool successful_start = (pid_ != -1);

    return successful_start && !reader_done_;
}




size_t Mdbg::Handler::SendCommand(const std::string& command) {

    if (!ProcessActive()) {
        return CMD_SEND_FAIL;
    }

    std::string token = std::to_string(cmd_counter_);

    std::string line = token + command + "\n"; // MI commands end in a line feed
    bool sent = WriteAll(fd_to_gdb_, line.data(), line.size());

    if (!sent) {
        return CMD_SEND_FAIL;
    }

    return cmd_counter_++;
}

bool Mdbg::Handler::WriteAll(int fd, const char* buf, size_t bufBytes) {

    while (bufBytes > 0) { // there are bytes to be written

        ssize_t n = write(fd, buf, bufBytes); // get number of bytes written

        if (n < 0 && errno == EINTR) {
            continue; // interuppted by a signal before writing, retry
        }

        if (n <= 0) {
            return false; // did not write all bytes, was not interrupted by a signal -> write fail
        }

        buf += n; // index n bytes into buf from current write
        bufBytes -= static_cast<size_t>(n); // reflect number of bytes left to be written
    }

    return true;
}


bool Mdbg::Handler::WaitForExit(int timeout_ms) {

    for (int ms_count = 0; ms_count < timeout_ms; ms_count += 10) {

        pid_t pid = waitpid(pid_, nullptr, WNOHANG); // pid = 0 -> still running <--> ELSE pid = pid_ -> exited

        if (pid == pid_) {
            return true; // successfully exited
        }

        if (pid == -1 && errno != EINTR) {
            return true;    // something bad happend but we free baby
        }


        std::this_thread::sleep_for(std::chrono::milliseconds(10));

    }

    return false;
}

void Mdbg::Handler::Stop() {

    if (pid_ == -1) { // process is already stopped
        return;
    }

    SendCommand("-gdb-exit"); // quit request first w builtin command so gdb can do cleanup


    close(fd_to_gdb_); //gdb reads EOF on stdin
    fd_to_gdb_ = -1; //sentinel value

    bool exit_success = WaitForExit(500); // give gdb time to process exit requst before escalating

    // gdb did not quit by itself (it is stuck in something)
    // SIGINT was consumed(?) internally by gdb so we use TERM instead 
    if (!exit_success) {
        kill(pid_, SIGTERM);
        exit_success = WaitForExit(2000);
    }

    if (!exit_success) {
        kill(pid_, SIGKILL); // probably not needed
        waitpid(pid_, nullptr, 0);
    }

    if(reader_thread_.joinable()) {
        reader_thread_.join();
    }

    close(fd_from_gdb_); //exit status collected
    fd_from_gdb_ = -1;
    pid_ = -1;
}


void Mdbg::Handler::DrainRecords(std::vector<Record>& out) {

    out.clear();            // out is cleared to ensure records_ recieves an empty vector


    std::scoped_lock<std::mutex> lock(records_mutex_); // protecting against a write from readerloop() while draining
    out.swap(records_);
}

inline std::vector<std::string> ExtractCompleteLines(std::string& pending_text) {

    std::vector<std::string> complete_lines;
    size_t line_start = 0;

    while (true) {
        size_t newline_pos = pending_text.find('\n', line_start);

        if (newline_pos == std::string::npos) break; // no complete line left, the rest is unfinished

        size_t line_length = newline_pos - line_start;
        std::string line = pending_text.substr(line_start, line_length);

        line_start = newline_pos + 1;

        if (!line.empty() && line.back() == '\r') { // pop \r if the line ended in \r\n
            line.pop_back();
        }

        if (line.empty()) { // ignore empty lines
            continue;
        }

        complete_lines.push_back(std::move(line));
    }

    pending_text.erase(0, line_start); // drop the lines that were handed back, keep the unfinished tail

    return complete_lines;
}

void Mdbg::Handler::ReaderLoop() {
    char buf[4096]; 
    std::string pending_text; // bytes received that do not yet end in '\n'

    while (true) {

        ssize_t n = read(fd_from_gdb_, buf, sizeof(buf)); // get n chars from gdb (through pipe) -> attempt to read 4096

        if (n < 0) {
            if (errno == EINTR) continue; // interrupted by a signal, retry read
            break;
        }

        if (n == 0) { // EOF read, gdb closed its STDOUT ( gdb exited )
            break;
        }

        pending_text.append(buf, static_cast<size_t>(n));

        // pulls the complete lines out of pending_text, leaving any unfinished line behind for the next read
        std::vector<Record> current_read_records{};

        for (const std::string& line : ExtractCompleteLines(pending_text)) {
            current_read_records.push_back(Mi::ClassifyLine(line));
        }





        { // SAFE MUTEX LOCK_GUARD SCOPE START 

            std::scoped_lock<std::mutex> lock(records_mutex_); // shared resource w DrainRecords proccess

            // append current_read_records to records_, if empty it will just .swap()
            auto batch_begin = std::make_move_iterator(current_read_records.begin());
            auto batch_end = std::make_move_iterator(current_read_records.end());
            records_.insert(records_.end(), batch_begin, batch_end);

        } // SAFE MUTEX LOCK_GUARD SCOPE END 

    }

    reader_done_ = true; // read is done, no more bytes left to be read

}


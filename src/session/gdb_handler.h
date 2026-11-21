#include <atomic>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
#include "mi_record.h"
#include <sys/types.h>



namespace Mdbg {


    constexpr int COMMAND_NOT_FOUND = 127; // used if execlp gdb falls through
    constexpr size_t CMD_SEND_FAIL = 0; // for prog counter
    constexpr const char* const GDB_MI3_ARGS =      "gdb \
                                                    --quiet \
                                                    --interpreter=mi3 \
                                                    -iex set style emoji off \
                                                    -iex set confirm off \
                                                    -iex set pagination off \
                                                    -iex set mi-async on";
    
    class Handler // Owns the gdb process and the thread that reads its output
    {
    public:
        Handler() = default;
        ~Handler();

        Handler(const Handler&) = delete;
        Handler& operator=(const Handler&) = delete;

        // Starts gdb and creates a forked process which execlp's gdb
        bool Start(const std::string& debug_elf, const std::vector<std::string>& program_arguments = {});
        void Stop();

        size_t SendCommand(const std::string& command);

        bool ProcessActive() const;

        // Moves every record the reader thread has collected into out (out is cleared first).
        void DrainRecords(std::vector<Mi::Record>& out);


    private:

        void ReaderLoop();
        bool WaitForExit(int timeout_ms);
        static bool WriteAll(int fd, const char* buf, size_t bufBytes);

        size_t cmd_counter_ = 1; // tokens start at 1 because 0 is CMD_SEND_FAIL

        pid_t pid_          = -1;
        int fd_to_gdb_      = -1;
        int fd_from_gdb_    = -1;


        std::thread reader_thread_;
        std::atomic<bool> reader_done_{false};


        std::mutex records_mutex_;
        std::vector<Mi::Record> records_;
    };


} // namespace mgdb


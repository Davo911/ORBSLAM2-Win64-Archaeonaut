#ifndef Log_h
#define Log_h

#include <chrono>
#include <iomanip>
#include <string>
#include <mutex>
#include <fstream>
#include <iostream>
#include <sstream>


namespace ORB_SLAM2 {
    
    using namespace std;
    using namespace chrono;
    
    class Log final {
    public:
        // this is static class
        // to avoid creating more than one object the public construction
        // methods are deleted
        Log(Log const&) = delete;             // Copy construct
        Log(Log&&) = delete;                  // Move construct
        Log& operator=(Log const&) = delete;  // Copy assign
        Log& operator=(Log &&) = delete;      // Move assign
        
    public:
        /*
         @brief write a variadic number of values (can have different types)
         @param calleeName name of the function which called "write(__FUNCTION__, ...)"
         @param args a variadic number of values of same or different types
         */
        template<typename... Values>
        static void write(const string& calleeName, Values... args) {
            time_t t = system_clock::to_time_t(system_clock::now());
            ostringstream oss;
            using expander = int[];
            (void) expander{ 0, (oss << " " << args, void(), 0)... };
            std::cout << put_time(localtime(&t), "[%T]")
                      << "[" << calleeName << "]:" << oss.str() << std::endl;
        }
        
    private:
        Log();
        ~Log();
    };
    
}

#endif

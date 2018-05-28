#ifndef Settings_h
#define Settings_h

#include <string>

namespace ORB_SLAM2 {
    
    class Settings {
        
    public:
        Settings(Settings const&) = delete;             // Copy construct
        Settings(Settings&&) = delete;                  // Move construct
        Settings& operator=(Settings const&) = delete;  // Copy assign
        Settings& operator=(Settings &&) = delete;      // Move assign
        
        static std::string path;



    };
    
}

#endif

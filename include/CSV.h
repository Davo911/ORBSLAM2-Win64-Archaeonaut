#ifndef CSV_h
#define CSV_h


#include <string>
#include <mutex>
#include <fstream>
#include <iostream>
#include <sstream>


namespace ORB_SLAM2 {
    
    class CSV final {
    public:
        // this class implements the singleton pattern
        // to avoid creating more than one object the public construction
        // methods are deleted
        CSV(CSV const&) = delete;             // Copy construct
        CSV(CSV&&) = delete;                  // Move construct
        CSV& operator=(CSV const&) = delete;  // Copy assign
        CSV& operator=(CSV &&) = delete;      // Move assign
        
    public:
        /* 
         @brief get unique instance from singleton
         @return instance of singleton
         */
        static CSV& getInstance();
        
        /*
         @brief get unique instance from singleton
         @param fileName
         @return instance of singleton
         */
        static CSV& getInstance(const std::string& fileName);
        
        /*
         @brief change file to write to. This will cause creation of a new file.
         @param fileName absolute or relative path to file including file extension
         */
        void setFileName(const std::string& fileName);
        
        /*
         @brief get current file path
         @return curent file Path
         */
        std::string getFileName() const;
        
        /*
         @brief redefine the separtor to place between CSVged values.
         This will cause creation of the same File. Older CSVs get lost.
         @param seperator new seperator
         */
        void setSeperator(const std::string& seperator);
        
        /*
         @brief get current value seperator
         @return current seperator
         */
        std::string getSeperator() const;
        
        /*
         @brief write a variadic number of values (can have different types) to file
         @param start if true a new line be added to the file
         @param linebreak if true a linebreak is added to the file
         @param first first entry value
         @param args a variadic number of values of same or different types
         */
        template<typename Value, typename... Values>
        void write(bool start, bool linebreak, Value first, Values... args) {
            std::lock_guard<std::mutex> lock(m);
            std::ostringstream oss;
            using expander = int[];
            
            if (start)
                oss << first;
            else
                oss << this->sep << first;
            
            (void) expander{ 0, (oss << this->sep << args, void(), 0)... };
            
            if (!this->stream.is_open())
                this->stream.open(this->fileName);

            this->stream << oss.str();
            
            if (linebreak)
                this->stream << std::endl;
        }
        
    private:
        CSV();
        CSV(const std::string& fileName);
        ~CSV();
        
    private:
        std::string sep;
        std::string fileName;
        std::ofstream stream;
        std::mutex m;
    };
    
}

#endif

#include "CSV.h"

#include <sstream>

namespace ORB_SLAM2 {
    CSV::CSV() : CSV("./ORB_SLAM2_CSV.csv") {
    }
    
    
    CSV::CSV(const std::string& fileName) : sep(";"), fileName(fileName), stream(fileName) {
    }
    
    
    CSV::~CSV() {
        if (this->stream.is_open()) {
            this->stream.close();
        }
    }
    
    
    CSV& CSV::getInstance(const std::string& fileName) {
        static CSV instance(fileName);
        return instance;
    }
    
    
    void CSV::setFileName(const std::string &fileName) {
        std::lock_guard<std::mutex> lock(m);
        this->fileName = fileName;
        if (this->stream.is_open())
            this->stream.close();
        this->stream.open(this->fileName);
    }
    
    
    std::string CSV::getFileName() const {
        return this->fileName;
    }
    
    
    void CSV::setSeperator(const std::string &seperator) {
        std::lock_guard<std::mutex> lock(m);
        this->sep = seperator;
        if (this->stream.is_open())
            this->stream.close();
        this->stream.open(this->fileName);
    }
    
    
    std::string CSV::getSeperator() const {
        return this->sep;
    }
}

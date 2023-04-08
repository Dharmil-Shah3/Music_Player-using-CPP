#include "logger.h"

/* ========== DATA MEMBERS ===========*/
bool Logger::fileOutput = false;
std::string Logger::log_filename = "tracelogs.txt";
FILE * Logger::file;
std::mutex Logger::write_lock;
std::mutex Logger::display_lock;
LogPriority Logger::priority = LogPriority::Debug;

/* ============= METHODS ==============*/
void Logger::enableFileOutput(const std::string &filename){
    fileOutput = true;
    if(!filename.empty()){
        Logger::log_filename = filename;
    }
}

void Logger::disableFileOutput(){
    fileOutput = false;
}

bool Logger::isFileOutputEnabled(){
    return fileOutput;
}

void Logger::setFilename(const char *filename){
    log_filename = filename;
}

void Logger::setPriority(const LogPriority &priority){
    Logger::priority = priority;
}

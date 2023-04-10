#include "logger.h"

/* ========== DATA MEMBERS ===========*/
bool Logger::consoleOutput = true;
bool Logger::fileOutput = false;
std::string Logger::log_filename = "tracelogs.txt";
LogPriority Logger::priority = LogPriority::Debug;
FILE * Logger::file = NULL;
std::mutex Logger::write_lock;
std::mutex Logger::display_lock;
time_t Logger::currentTime;
tm * Logger::timeStamp;
char Logger::timeBuffer[];

/* ============= METHODS ==============*/
void Logger::enableConsoleOutput(){
    consoleOutput = true;
}

void Logger::disableConsoleOutput(){
    consoleOutput = false;
}

bool Logger::isConsoleOutputEnabled(){
    return consoleOutput;
}

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

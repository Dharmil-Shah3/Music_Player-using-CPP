#include "logger.h"

/* ========== DATA MEMBERS ===========*/
bool Logger::saveLogsIntoFile = false;
std::string Logger::log_filename = "tracelogs.log";
std::ofstream Logger::fout;
std::mutex Logger::write_lock;
std::mutex Logger::display_lock;
LogPriority Logger::priority = LogPriority::Debug;


/* ============= METHODS ==============*/
void Logger::setSaveLogsIntoFile(const bool &true_false){
    saveLogsIntoFile = true_false;
}

bool Logger::isSavingIntoFile(){
    return saveLogsIntoFile;
}

void Logger::setFilename(const char *filename){
    log_filename = filename;
}

void Logger::setPriority(const LogPriority &priority){
    Logger::priority = priority;
}

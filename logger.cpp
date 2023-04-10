#include "logger.h"
#include <cstring>

/* ========== CONSTRUCTOR - DESTRUCTOR ===========*/
Logger::Logger(){
    this->priority = Debug;
    this->consoleOutput = true;
    this->fileOutput = true;
    filename = new char[9];
    strcpy(filename, "logs.log");
    this->file = fopen(filename, "a");
}

Logger::~Logger(){
    this->log(Trace, "-> Logger DESTRUCTOR START");
    if(file != NULL){
        fclose(file);
    }
    delete filename;
    printf("\n[Trace] : Logger DESTRUCTOR END\n");
}

/* ============= STATIC MEMBERS & METHODS ==============*/
std::mutex Logger::get_instance_lock;
Logger* Logger::logger = NULL;

Logger* Logger::GetInstance(){
    std::lock_guard<std::mutex> lock(get_instance_lock);
    if(logger == NULL)
        logger = new Logger();
    return logger;
}

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

void Logger::enableFileOutput(const char *filename){
    fileOutput = true;
    if(filename != NULL){
        this->setFilename(filename);
    }
    if(file != 0){ // if file is opened, then close it to reopen it.
        fclose(file);
    }
    file = fopen(filename, "a");
    if(file == 0){ // if file is not opened
        printf("[Warn] : Failed to open file '%s' to write logs.", filename);
    }
}

void Logger::disableFileOutput(){
    fileOutput = false;
    if(file != 0){
        fclose(file);
    }
}

bool Logger::isFileOutputEnabled(){
    return fileOutput;
}

void Logger::setFilename(const char *filename){
    delete this->filename;
    this->filename = new char[strlen(filename)+1];
    strcpy(this->filename, filename);
}

void Logger::setPriority(const LogPriority &priority){
    this->priority = priority;
}

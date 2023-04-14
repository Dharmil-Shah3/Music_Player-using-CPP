#include "logger.h"
#include <cstring>  // for strcpy(), strlen() etc.


/* ========== CONSTRUCTOR - DESTRUCTOR ===========*/
Logger::Logger(){
    priority = trace;
    consoleOutput = true;
    fileOutput = true;
    filename = new char[9];
    strcpy(filename, "logs.log");
    file = fopen(filename, "a");
}

Logger::~Logger(){
    //LOG(trace, "-> Logger destructor called", NULL);
    if(file != NULL)
        fclose(file);
    if(filename != NULL)
        delete filename;
}


/* ============= STATIC MEMBERS & METHODS ==============*/
std::mutex Logger::get_instance_lock;
Logger* Logger::logger = NULL;

Logger* Logger::get(){
    std::lock_guard<std::mutex> lock(get_instance_lock);
    if(logger == NULL)
        logger = new Logger();
    return logger;
}


/* ============= METHODS ==============*/
void Logger::setPriority(const LogPriority &priority){
    this->priority = priority;
}

LogPriority Logger::getPriority() const{
    return this->priority;
}

void Logger::enableConsoleOutput(){
    consoleOutput = true;
}

void Logger::disableConsoleOutput(){
    consoleOutput = false;
}

bool Logger::isConsoleOutputEnabled() const{
    return consoleOutput;
}

bool Logger::isFileOutputEnabled() const{
    return fileOutput;
}

void Logger::setFilename(const char *filename){
    std::lock_guard<std::mutex> lock(file_lock); // locking file before making any change

    if(file) fclose(file);
    delete this->filename;
    this->filename = new char[strlen(filename)+1];
    strcpy(this->filename, filename);
    file = fopen(filename, "a");
}

void Logger::enableFileOutput(const char *filename){
    fileOutput = true;
    if(filename != NULL){
        this->setFilename(filename);
    }
    else {
        std::lock_guard<std::mutex> lock(file_lock); // locking file before making any change
        if(file != NULL){ // if file is opened, then close it to reopen it.
            fclose(file);
        }
        file = fopen(this->filename, "a");
        if(file == NULL)
        {
            LOG(error, "Failed to open file '%s' to write logs.", this->filename);
            if(!consoleOutput) /* print on console using printf() if consoleOutput is OFF */
                printf("[ERROR] : Failed to open file '%s' to write logs.\n", this->filename);
        }
    }
}

void Logger::disableFileOutput(){
    std::lock_guard<std::mutex> lock(file_lock); // locking file before making any change
    fileOutput = false;
    if(file != NULL){
        fclose(file);
    }
}

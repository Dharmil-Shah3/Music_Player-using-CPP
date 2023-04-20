#include "logger.h"
#include <cstring>  // for strcpy(), strlen() etc.
#include <sstream>  // to use 'ostringstream' to convert std::thread::id to std::string
#include <iomanip>  // to use setw(), setfill() etc.

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
            LOG(error, "Failed to open file '" +std::string(this->filename)+ "' to write logs.");
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

void Logger::log(const LogPriority &logPriority, const std::thread::id &threadId, const unsigned short _line_number_, const char* _function_name_, const std::string &message)
{
    using namespace std;

    // Either consoleOutput or fileOutput must be true.
    // And log priority also should be equal and greater than Logger's priority.
    if((consoleOutput || fileOutput) && (logPriority >= this->priority))
    {
        string logType;
        switch (logPriority) {
            case trace:   logType="trace"; break;
            case debug:   logType="debug"; break;
            case info:    logType="info "; break;
            case warning: logType="warn "; break;
            case error:   logType="error"; break;
            case fatal:   logType="fatal"; break;
        }

        // ---------- getting current date and time ----------
        currentTime = time(0);
        timeStamp = localtime(&currentTime);

        // ---------- getting milliseconds and microseconds for timestamp ----------
        using namespace chrono;
        microseconds ms = duration_cast<microseconds>(system_clock::now().time_since_epoch());
        unsigned long int milliseconds = ms.count()%1000000/1000;
        unsigned long int microseconds = ms.count()%1000000%1000;

        // ---------- make a complete log message ----------
        ostringstream logMessage;
        logMessage << setfill('0') << "[" << timeStamp->tm_year+1900 << "-" << setw(2) << timeStamp->tm_mon+1 << "-" << setw(2) << timeStamp->tm_mday
        << " " << setw(2) << timeStamp->tm_hour << ":" << setw(2) << timeStamp->tm_min << ":" << setw(2) << timeStamp->tm_sec
        << "." << setw(3) << milliseconds << "." << setw(3) << microseconds << "]" << setfill(' ')
        << " ["<<threadId<<"] ["<<logType<<"] ["<<setw(4)<<_line_number_<<"] " << message
        << " -> ["<<_function_name_<<"] " << endl;

        // ---------- display the log message ----------
        if(consoleOutput){
            lock_guard<mutex> lock(display_lock);
            cout << logMessage.str();
        }

        // ---------- write logs into file if enabled ----------
        if(fileOutput){
            lock_guard<mutex> lock(file_lock);
            fprintf(file, logMessage.str().c_str());
        }
    }
}

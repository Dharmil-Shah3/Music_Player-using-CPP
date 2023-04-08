#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <fstream>
#include <mutex>

/*****************************************************************//**
 * @brief The LogPriority enum defines the priority of different logs.
 * We can use it to avoid displaying logs that has lower priority,
 * and to avoid too many logs on the display.
 ********************************************************************/
enum LogPriority{
    Trace,
    Debug,
    Info,
    Warn,
    Error,
    Critical
};


/******************************************************************************************************************//**
 * @class Logger
 * @brief The Logger class is used for trace logging and debugging purpose.
 * This class provides the static members and methods to print the differernt log messages with different priorities.\n
 * This class uses values of enum LogPriority to display the differernt type of logs.
 *********************************************************************************************************************/
class Logger
{
public:

    /**********************************************************************************//**
     * @brief enables the file output.
     * @param sets the log file name if provided. Ohterwise default value is already there.
     *************************************************************************************/
    void static enableFileOutput(const std::string &filename="");

    /************************************//**
     * @brief disables file output.
     * Logs will not be saved into the files.
     ***************************************/
    void static disableFileOutput();

    /********************************************************************//**
     * @brief used to know whether the flag saveLogsIntoFile are true or not.
     * @return value of static member saveLogsIntoFile.
     ***********************************************************************/
    bool static isFileOutputEnabled();

    /***************************************************************************//**
     * @brief setFilename used to change the filename in which the logs are written.
     * @param filename is the new filename to write logs into.
     ******************************************************************************/
    void static setFilename(const char *filename);

    /************************************************************************************************//**
     * @brief setPriority is used to set log priority, to filter logs to display.
     * @param priority is the new priority to set.
     * This method changes value of the static data member called priority, with the given LogPriority.\n
     * Logger class checks the variable priority before printing any Log into screen.
     ****************************************************************************************************/
    void static setPriority(const LogPriority &priority);

    /**************************************************************************************//**
     * @brief log displays the log message and log's type.
     * @param logPriority is the type of the log message.
     * @param message to be displayed.
     * @param args is the arguments to bind into the log message, and it is optional.
     * It checks the priority of the log message and compare it with private member priority.\n
     * If the priority of the log message is equal or higher than the member,
     * then log message will be displayed, else it will be skipped.\n
     * This way, we can decide what types of logs to be displayed and what not to be displayed.
     *****************************************************************************************/
    template <typename... Args>
    static void log(const LogPriority &logPriority, const char *message, const Args... args)
    {
        if(logPriority >= Logger::priority)
        {
            std::string logType;
            switch (logPriority) {
                case Trace:    logType="Trace"; break;
                case Debug:    logType="Debug"; break;
                case Info:     logType="Info"; break;
                case Warn:     logType="Warn"; break;
                case Error:    logType="Error"; break;
                case Critical: logType="Critical"; break;
            }

            // getting current date and time
            time_t t;
            tm *_time;
            time(&t);
            _time = localtime(&t);

            // getting microseconds to display
            using namespace std::chrono;
            microseconds ms = duration_cast<microseconds>(system_clock::now().time_since_epoch());
            unsigned long int milliseconds = ms.count()%1000000/1000;
            unsigned long int microseconds = ms.count()%1000000%1000;

            // lock mutex, and display the log message
            display_lock.lock();
            printf("[%s][%4d/%02d/%02d %02d:%02d:%02d.%03ld.%03ld]: ",
                    logType.c_str(),
                    _time->tm_year+1900, _time->tm_mon+1, _time->tm_mday,
                    _time->tm_hour, _time->tm_min, _time->tm_sec,
                    milliseconds, microseconds);
            printf(message, args...);
            printf("\n");
            display_lock.unlock();

            // write logs into file
            if(fileOutput)
            {
                write_lock.lock();
                file = fopen(log_filename.c_str(), "a");
                if(file != 0) // file is opened
                {
                    fprintf(file, "[%s][%4d/%02d/%02d %02d:%02d:%02d.%03ld.%03ld]: ",
                            logType.c_str(),
                            _time->tm_year+1900, _time->tm_mon+1, _time->tm_mday,
                            _time->tm_hour, _time->tm_min, _time->tm_sec,
                            milliseconds, microseconds);
                    fprintf(file, message, args...);
                    fprintf(file, "\n");
                    fclose(file);
                } else {
                    printf("\n[Warn]: fail to open file \"%s\" to write logs", log_filename.c_str());
                }
                write_lock.unlock();
            }
        }
    }

private:

    /** @brief used to determine whether to write logs into file or not (default = false). */
    bool static fileOutput;

    /** @brief log_filename stores the name of the file to write logs into. */
    static std::string log_filename;

    /** @brief file is used to write logs into log file. */
    static FILE *file;

    /** @brief write_lock mutex is used to prevent the race condition to write logs into file. */
    static std::mutex write_lock;

    /** @brief display_lock used to prevent race condition while displaying logs. */
    static std::mutex display_lock;

    /************************************************************************************************************************************************//**
     * @brief priority variable is used to compare with priority of the log message to be displayed.
     * Before displaying the log, this variable is used to check and compare with the priority of the log message is equal or higher than this or not.\n
     * If the priority of the log message is lesser that this variable, then that log message will not be displayed.\n
     * Default value is LogPriority::Debug.
     ***************************************************************************************************************************************************/
    static LogPriority priority;

    /** @brief make_this_class_abstract is just declared to make this class abstract. */
    virtual void make_this_class_abstract() = 0;
};

#endif // LOGGER_H

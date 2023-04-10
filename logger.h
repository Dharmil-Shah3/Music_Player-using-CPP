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
 * This class uses values of enum LogPriority to display the differernt type of logs.\n
 * Reference video: https://youtu.be/1rnmYBSppYY
 *********************************************************************************************************************/
class Logger
{
public:

    /** @brief enables the console log output. */
    void static enableConsoleOutput();

    /** @brief logs will not be shown in the console. */
    void static disableConsoleOutput();

    /*******************************************************//**
     * @brief whether logs are displaying on the console or not.
     * @return value of boolean consoleOutput flag.
     **********************************************************/
    bool static isConsoleOutputEnabled();

    /*******************************************************************************************//**
     * @brief enables the file log output.
     * @param filename sets the log file name if provided. Ohterwise default value is already there.
     **********************************************************************************************/
    void static enableFileOutput(const std::string &filename="");

    /************************************//**
     * @brief disables file output.
     * Logs will not be saved into the files.
     ***************************************/
    void static disableFileOutput();

    /*************************************************************//**
     * @brief used to know whether the flag fileOutput is true or not.
     * @return value of static member fileOutput.
     ****************************************************************/
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
        // Either consoleOutput or fileOutput must be true.
        // And log priority also should be equal and greater
        if((consoleOutput || fileOutput) && (logPriority >= Logger::priority))
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
            currentTime = time(0);
            timeStamp = localtime(&currentTime);
            strftime(timeBuffer, 25, "%F %T", timeStamp);

            // getting microseconds to display
            using namespace std::chrono;
            microseconds ms = duration_cast<microseconds>(system_clock::now().time_since_epoch());
            unsigned long int milliseconds = ms.count()%1000000/1000;
            unsigned long int microseconds = ms.count()%1000000%1000;

            // lock mutex, and display the log message
            if(consoleOutput){
                display_lock.lock();
                printf("[%s][%s.%03ld.%03ld]: ", logType.c_str(), timeBuffer, milliseconds, microseconds);
                printf(message, args...);
                printf("\n");
                display_lock.unlock();
            }

            // write logs into file, if enabled
            if(fileOutput){
                write_lock.lock();
                file = fopen(log_filename.c_str(), "a");
                if(file != 0) // file is opened
                {
                    fprintf(file, "[%s][%s.%03ld.%03ld]: ", logType.c_str(), timeBuffer, milliseconds, microseconds);
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

    /** @brief used to determine whether to display logs into console or not (default = true). */
    bool static consoleOutput;

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

    /*********************************************************************************************************//**
     * @brief priority variable is used to compare with priority of the log message to be displayed.
     * Before displaying the log, this variable is used to check and compare with the priority of the log message is equal or higher than this or not.\n
     * If the priority of the log message is lesser that this variable, then that log message will not be displayed.\n
     * Default value is LogPriority::Debug.
     ************************************************************************************************************/
    static LogPriority priority;

    /** @brief it is used to get the current system time to display log time. */
    static time_t currentTime;

    /** @brief it is used to hold the localtime of the system to display log timestamp. */
    static tm *timeStamp;

    /** @brief it holds the string formatted(YYYY-MM-DD HH:MM:SS) timestamp of the system to display with log. */
    static char timeBuffer[25];

    /** @brief make_this_class_abstract is just declared to make this class abstract. */
    virtual void make_this_class_abstract() = 0;
};

#endif // LOGGER_H

#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <fstream>  // for file output
#include <mutex>    // to avoid race conditions in output.
#include <thread>   // to use std::thread::id
#include <sstream>  // to use 'ostringstream' to convert std::thread::id to std::string


/***************************************************************************************************************//**
  * @def LOG(priority, message, ...)
  * @brief It is used for logging, and is a short hand of the `Logger::log()`.
  * @param priority is the `LogPriority` of the log.
  * @param message is the log message.
  * @param ... is the variable arguments to go with the message (if any). Have to pass NULL if there is no argument.
  *
  * It is used as an alternative of `Logger::log()`\n
  * Main objective is, to pass static arguments like **thread id**, **line number** and **function name** into `Logger::log()`.\n
  * This macro internally calls the `log()` with all the necessary arguments passing by it self.\n
  * So user have to pass only necessary parameters.
  *
  * There are 2 differences between `LOG()` and `Logger::log()`.\n
  * 1. Unlike `log()`, in `LOG`, if there is no extra argument after `message`, we have to pass **NULL** explicitely.
  * 2. Unlike `log()`, in `LOG`, we don't have to pass **thread id**, **line number** and **function name** explicitely.
  *
  * **Examples**\n
  * 1. LOG(trace, "custome log 1", NULL);
  * 2. LOG(debug, "object id %d, object name %s", id, name);
  *****************************************************************************************************************/
#define LOG(priority, message, ...) (Logger::get()-> Logger::log(priority, std::this_thread::get_id(), __LINE__, __PRETTY_FUNCTION__, message, __VA_ARGS__))


/*****************************************************************//**
 * @enum LogPriority
 * @brief The LogPriority enum defines the priority of different logs.
 * We can use it to avoid displaying logs that has lower priority,
 * and to avoid too many logs on the display.
 ********************************************************************/
enum LogPriority{
    trace,
    debug,
    info,
    warning,
    error,
    fatal
};


/***********************************************************************************//**
 * @class Logger
 * @brief Logger class is used for trace logging and debugging purpose.
 *
 * This class provides members and methods to display and save(into files),
 * logs with different types/priorities.\n
 * This class uses values of `enum LogPriority` to display the differernt type of logs.\n
 * This class uses std::mutex to be a **thread safe** class.\n
 * [Reference YouTube video](https://youtu.be/1rnmYBSppYY "link").\n
 * \n Along with the priority and log message, this also displays
 * 1. thread-id
 * 2. line number
 * 3. function name
 **************************************************************************************/
class Logger
{
public:

    /** @brief Destructor ensures to close the file and delete pointers, before instance is deleted. */
    ~Logger();

    /*********************************************************************************//**
     * @brief It creates new instance (if does not exist) of [class Logger](@ref Logger).
     *
     * It is used to get the reference of the singleton object of the [Logger](#Logger).
     * @return pointer to the singleton [Logger](#Logger) instance.
     ************************************************************************************/
    static Logger* get();

    /** @brief enables the console output of logs. */
    void enableConsoleOutput();

    /** @brief desables the console output of logs. */
    void disableConsoleOutput();

    /******************************************************//**
     * @brief used to know if console output is enabled on not.
     * @return value of `bool consoleOutput` flag.
     *********************************************************/
    bool isConsoleOutputEnabled() const;

    /******************************************************************//**
     * @brief enables the file output of logs.
     * @param filename sets the log file name if provided (default `NULL`).
     *********************************************************************/
    void enableFileOutput(const char *filename = NULL);

    /*****************************************************//**
     * @brief disables file output and closes the opened file.
     * Logs will not be saved into the file.
     ********************************************************/
    void disableFileOutput();

    /*************************************************************//**
     * @brief used to know whether the file output is enabled or not.
     * @return value of `bool fileOutput` member.
     ****************************************************************/
    bool isFileOutputEnabled() const;

    /********************************************************************//**
     * @brief is used to change the filename in which the logs are written.
     * @param filename is the new filename to save output into.
     * It closes the current open file(if any),
     * changes the filename,
     * and reopen the file with new filename.
     ***********************************************************************/
    void setFilename(const char *filename);

    /**********************************************************//**
     * @brief is used to set log priority, to filter logs.
     * @param priority is the new `LogPriority`to set.
     * This method changes value of `LogPriority priority`.\n
     * [Logger](@ref Logger) checks the `priority` before logging.
     *************************************************************/
    void setPriority(const LogPriority &priority);

    /**************************************//**
     * @brief used to get the current priority.
     * @return value of `LogPriority priority`.
     *****************************************/
    LogPriority getPriority() const;

    /*****************************************************************************************************************//**
     * @brief displays and/or saves(into file) logs with timestamp, thread-id, line-number and function-name.
     * @param logPriority is the priority of the log message.
     * @param threadId is id of the thread who called this function (generally taken care by macro `LOG`).
     * @param _line_number_ is the number of line on which the `log()` is called.
     * @param _function_name_ is the name of the function in which the `log()` is called.
     * @param message to be displayed and/or saved.
     * @param args are the arguments to bind into the log message, and it is optional.
     *
     * This function checks the priority of the log message and compare it with member `priority`.\n
     * If priority of the log message is equal or higher than `priority`,
     * then log message will be displayed, else it will be skipped.\n
     * Format of the log message will be\n
     * i.e) [2023-04-14 13:17:05.041.354] [140613438010304] [trace  ] [31  ] Execution Begin -> [int main()]\n
     * [Time stamp with milliseconds and microseconds] [thread-id] [priority] [line-number] Log message -> [function_name]
     *
     * **Example**
     * 1. Logger::get()->log(trace, this_thread::get_id(), __LINE__, __PRETTY_FUNCTION__, "log message");
     * 2. Logger::get()->log(trace, this_thread::get_id(), __LINE__, __PRETTY_FUNCTION__, "sstudent added, id:%d, name:%s", studentId, studentName);
     *
     * Better to use macro `LOG()` instead of this function.\n
     * `LOG()` takes care of passing all the static arguments(thread id, line number and function name) by itself.
     *
     * **Above Examples with `LOG()` macro**
     * 1. LOG(trace, "log message", NULL);
     * 2. LOG(trace, "sstudent added, id:%d, name:%s", studentId, studentName);
     *********************************************************************************************************************/
    template <typename... Args>
    void log(const LogPriority &logPriority,
             const std::thread::id &threadId,
             const unsigned short _line_number_,
             const char* _function_name_,
             const char *message,
             const Args... args)
    {
        // Either consoleOutput or fileOutput must be true.
        // And log priority also should be equal and greater than Logger's priority.
        if((consoleOutput || fileOutput) && (logPriority >= this->priority))
        {
            std::string logType;
            switch (logPriority) {
                case trace:   logType="trace"; break;
                case debug:   logType="debug"; break;
                case info:    logType="info"; break;
                case warning: logType="warning"; break;
                case error:   logType="error"; break;
                case fatal:   logType="fatal"; break;
            }

            // getting current date and time
            currentTime = time(0);
            timeStamp = localtime(&currentTime);

            // getting milliseconds and microseconds for timestamp
            using namespace std::chrono;
            microseconds ms = duration_cast<microseconds>(system_clock::now().time_since_epoch());
            unsigned long int milliseconds = ms.count()%1000000/1000;
            unsigned long int microseconds = ms.count()%1000000%1000;

            // lock mutex, and display the log message
            if(consoleOutput){
                std::lock_guard<std::mutex> lock(display_lock);
                printf("[%4d-%02d-%02d %02d:%02d:%02d.%03ld.%03ld] [%ld] [%-7s] [%-4u] ",
                       (timeStamp->tm_year+1900), (timeStamp->tm_mon+1), timeStamp->tm_mday,
                       timeStamp->tm_hour, timeStamp->tm_min, timeStamp->tm_sec,
                       milliseconds, microseconds, threadId, logType.c_str(), _line_number_
                );
                printf(message, args...);
                printf(" -> [%s] \n", _function_name_);
            }

            // write logs into file, if enabled
            if(fileOutput){
                std::lock_guard<std::mutex> lock(file_lock);
                if(file == 0){ // if file is not opened
                    file = fopen(filename, "a");
                }
                fprintf(file, "[%4d-%02d-%02d %02d:%02d:%02d.%03ld.%03ld] [%ld] [%-7s] [%-4u] ",
                       (timeStamp->tm_year+1900), (timeStamp->tm_mon+1), timeStamp->tm_mday,
                       timeStamp->tm_hour, timeStamp->tm_min, timeStamp->tm_sec,
                       milliseconds, microseconds, threadId, logType.c_str(), _line_number_
                );
                fprintf(file, message, args...);
                fprintf(file, " -> [%s] \n", _function_name_);
            }
        }
    }

private:

    /***********************************************************************//**
     * @brief `Logger()` is a private default constructor.
     *
     * This is declared private, to make this class singleton.\n
     * Singleton class ensures that only one object of the class can be created.
     **************************************************************************/
    Logger();

    /*******************************************************************//**
     * @brief a copy constructor.
     *
     * Singleton class should not be clonable, so deleting copy constructor.
     **********************************************************************/
    Logger(const Logger &) = delete;

    /***************************************//**
     * @brief operator =
     *
     * Singleton class should not be assignable.
     * @return
     ******************************************/
    Logger& operator= (const Logger &) = delete;

    /** @brief used to determine whether to display logs into console or not (default `true`). */
    bool consoleOutput;

    /** @brief used to determine whether to write logs into file or not (default `true`). */
    bool fileOutput;

    /** @brief filename stores the name of the file to write logs into (default `logs.log`). */
    char *filename;

    /** @brief file is used to write logs into log file. */
    FILE *file;

    /** @brief file_lock mutex is used to prevent the race condition to write logs into `file`. */
    std::mutex file_lock;

    /** @brief display_lock used to prevent race condition while displaying logs into console. */
    std::mutex display_lock;

    /** @brief get_instance_lock is used to make `Logger::get()` thread safe, to prevent creation of more than one objects. */
    static std::mutex get_instance_lock;

    /** @brief logger is a pointer to the singleton object of the class. */
    static Logger *logger;

    /*********************************************************************************************************//**
     * @brief It is used filter the log messages based on priority.
     *
     * Before logging each log,
     * `log()` checks the priority of the log message is equal or higher than `priority`(member) or not.\n
     * If the priority of the log message is lesser that this, then that log message will be skipped.\n
     * *Default* value is `LogPriority::trace`.
     ************************************************************************************************************/
    LogPriority priority;

    /** @brief it is used to get the current system time to display log time(used by `tm *timeStamp`). */
    time_t currentTime;

    /** @brief it is used to hold the localtime of the system to display log's timestamp. */
    tm *timeStamp;
};

#endif // LOGGER_H

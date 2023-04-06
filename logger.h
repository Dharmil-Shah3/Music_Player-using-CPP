#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <fstream>
#include <mutex>

/**
 * @brief The LogPriority enum defines the priority of different logs.
 * We can use it to avoid displaying logs that has lower priority,
 * and to avoid too many logs on the display.
 */
enum LogPriority{
    Trace,
    Debug,
    Info,
    Warn,
    Error,
    Critical
};


/**
 * @class Logger
 * @brief The Logger class is used for trace logging and debugging purpose.
 * This class provides the static members and methods to print the differernt log messages with different priorities.\n
 * This class uses values of enum LogPriority to display the differernt type of logs.
 */
class Logger
{
public:

    /**
     * @brief setSaveLogsIntoFile is set whether to write logs into file or not.
     * @param true_false is the new bool value to assign to saveLogsIntoFiles flag.
     */
    void static setSaveLogsIntoFile(const bool &true_false);

    /**
     * @brief isSavingIntoFile is used to know whether the flag saveLogsIntoFile are true or not.
     * @return value of static member saveLogsIntoFile.
     */
    bool static isSavingIntoFile();

    /**
     * @brief setFilename used to change the filename in which the logs are written.
     * @param filename is the new filename to write logs into.
     */
    void static setFilename(const char *filename);

    /**
     * @brief setPriority is used to set log priority, to filter logs to display.
     * @param priority is the new priority to set.
     * This method changes value of the static data member called priority, with the given LogPriority.\n
     * Logger class checks the variable priority before printing any Log into screen.
     */
    void static setPriority(const LogPriority &priority);

    /**
     * @brief log displays the log message and log's type.
     * @param logPriority is the type of the log message.
     * @param message to be displayed.
     * @param args is the arguments to bind into the log message, and it is optional.
     * It checks the priority of the log message and compare it with private member priority.\n
     * If the priority of the log message is equal or higher than the member,
     * then log message will be displayed, else it will be skipped.\n
     * This way, we can decide what types of logs to be displayed and what not to be displayed.
     */
    template <typename... Args>
    static void log(const LogPriority &logPriority, const char *message, const Args... args)
    {
        if(logPriority >= Logger::priority)
        {
            std::string logType;
            switch (logPriority) {
                case LogPriority::Trace:    logType="Trace"; break;
                case LogPriority::Debug:    logType="Debug"; break;
                case LogPriority::Info:     logType="Info"; break;
                case LogPriority::Warn:     logType="Warn"; break;
                case LogPriority::Error:    logType="Error"; break;
                case LogPriority::Critical: logType="Critical"; break;
            }

            // lock and display the log message
            display_lock.lock();
            printf("[%s]: ", logType.c_str());
            printf(message, args...);
            printf("\n");
            display_lock.unlock();

            // write logs into file
            if(saveLogsIntoFile)
            {
                write_lock.lock();
                fout.open(log_filename, std::ios_base::app);
                if(fout){
                    fout << "["<< logType <<"]: " << message << std::endl;
                    fout.close();
                } else {
                    printf("\n[Warn]: fail to open file \"%s\" to write logs", log_filename);
                }
                write_lock.unlock();
            }
        }
    }

private:

    /** @brief saveLogsIntoFile flag is used to determine whether to write logs into file or not (default = false). */
    bool static saveLogsIntoFile;

    /** @brief log_filename stores the name of the file to write logs into. */
    static std::string log_filename;

    /** @brief fout is used to write logs into log file. */
    static std::ofstream fout;

    /** @brief write_lock mutex is used to prevent the race condition to write logs into file. */
    static std::mutex write_lock;

    /** @brief display_lock used to prevent race condition while displaying logs. */
    static std::mutex display_lock;

    /**
     * @brief priority variable is used to compare with priority of the log message to be displayed.
     * Before displaying the log, this variable is used to check and compare with the priority of the log message is equal or higher than this or not.\n
     * If the priority of the log message is lesser that this variable, then that log message will not be displayed.\n
     * Default value is LogPriority::Debug.
     */
    static LogPriority priority;

    /** @brief make_this_class_abstract is just declared to make this class abstract. */
    virtual void make_this_class_abstract() = 0;
};

#endif // LOGGER_H

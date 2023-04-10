#ifndef DISPLAYDATA_H
#define DISPLAYDATA_H

#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "song.h"
#include "logger.h"

/**
 * @brief The DisplayData class.
 *
 * Objective of the class is to display the details continously of the song currently being played.\n
 * It maintains a queue of type Song class named "playlist".\n
 * Objects of class Song are pushed into that queue.\n\n
 *
 * displaySongDetails() takes care of the display operation.\n\n
 *
 * playNextSong() pops the first song from the playlis(queue) after it is played successfully.\n\n
 *
 * checkForException() waits until any exception is raised or occured,
 * and prints the exception and returns value 1.
 */
class DisplayPlaylist
{
public:

    /** @brief DisplayPlaylist is a default constructor. */
    DisplayPlaylist();

    /** @brief ~DisplayPlaylist is a destructor. */
    ~DisplayPlaylist();

    /***********************************************************************************************//**
     * @brief displaySongDetails continouslly desplays the song details that is being played.
     * It waits if the songPlaying flag is false.\n
     * It goes into waiting queue until someone wakes him up.\n
     * It waits on the songCondition variable with the unique_lock using _lock_ attribut of the class.\n
     * After song finishes its duration,
     * it will set songPlaying flag to false and notify the waiting thread on songCondition variable.
     **************************************************************************************************/
    void playPlaylist();

    /********************************************************************************************//**
     * @brief playNextSong pops the first song from the playlist.
     * This function executes until the queue is not empty.\n
     * Inside loop, it waits if the songPlaying flag is true.\n
     * It waits on the songCondition variable with the uniqie_lock and _lock_ mutex of the class.\n
     * It wakes up after someone notify on the songCondition variable.\n
     * After waking up, it pops the song from the playlist, and sets the songPlaying flag to true.\n
     * And it notifies on the songPlaying variable.
     **********************************************************************************************/
    void playNextSong();

    /****************************************************************************************************************//**
     * @brief monitorException is used to check/monitor any exception during execution.
     * This method appies unique_lock on _lock_ mutex, and then goes into waiting using errorRaised condition variable.\n
     * It wakes from the sleep once anyone notify on the errorRaised condition variable.\n
     * After waking up, it displays the error message if any and terminates program.\n
     *******************************************************************************************************************/
    void monitorException(int &returnValue);

    /**********************************************************************************//**
     * @brief pushSongIntoPlaylist method pushes the Song object into the playlist (queue).
     * @param song is the objecet of the Song, that needs to be pushed into the playlist.
     ***********************************************************************************/
    void pushSongIntoPlaylist(const Song &song);

private:

    /** @brief logger is a pointer to logger class's singleton object. */
    Logger *logger;

    /** @brief songPlaying is the boolean that indicates either any song is being played or not. */
    bool songPlaying;

    /************************************************************************//**
     * @brief executionComplete flag indicates if all songs are completed or not.
     * This flag is specially used into monitorException method.
     * If its true, then error thread can end it's waiting to complete execution.
     ***************************************************************************/
    bool executionComplete;

    /************************************************************************************************************//**
     * @brief errorMessage is used to store error message if any exception occurs during the execution.
     * It is used as a flag for terminating the error thread properly.\n
     * Error thread wakes up from the sleep under 2 situations.\n
     * 1) if any exception has occured during execution.\n
     * 2) if execution is completed, and its time to complete the execution of all threads, so that they don't stuck in infinate sleep/wait.\n
     * So, using this data member, error thread can determine if there is any error or not.\n
     * If there is any error, then print the error message stored inside this variable, and terminate the program.\n
     ***************************************************************************************************************/
    std::string errorMessage;

    /*******************************************************************************************//**
     * @brief _lock_ is the mutex lock,\n
     * which is used with the unique_lock and condition_vatiable to make thread wait and awake them.
     **********************************************************************************************/
    std::mutex _lock_;

    /** @brief songCondition is the condition variable to synchronize the display and pop thread. */
    std::condition_variable songCondition;

    /*******************************************************//**
     * @brief used to send error monitoring thread into waiting,
     * and wake error thread back from the sleep.
     **********************************************************/
    std::condition_variable errorRaised;

    /** @brief playlist represents the song playlist which holds the songs to be played. */
    std::queue<Song> playlist;
};

#endif // DISPLAYDATA_H

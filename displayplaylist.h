#ifndef DISPLAYDATA_H
#define DISPLAYDATA_H

#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "song.h"

/**
 * @brief The DisplayData class.
 *
 * Objective of the class is to display the details continously of the song currently being played.
 * It maintains a queue of type Song class named "playlist".
 * Objects of class Song are pushed into that queue.
 *
 * displaySongDetails() takes care of the display operation.
 *
 * playNextSong() pops the first song from the playlis(queue) after it is played successfully.
 *
 * checkForException() waits until any exception is raised or occured,
 * and prints the exception and terminates the program with return value 1.
 */
class DisplayPlaylist
{
public:

    /** @brief DisplayPlaylist is a default constructor. */
    DisplayPlaylist();

    /**
     * @brief displaySongDetails continouslly desplays the song details that is being played.
     * It waits if the songPlaying flag is false.
     * It goes into waiting queue until someone wakes him up.
     * It waits on the songCondition variable with the unique_lock using _lock_ attribut of the class.
     * After song finishes its duration,
     * it will set songPlaying flag to false and notify the waiting thread on songCondition variable.
     */
    void displaySongDetails();

    /**
     * @brief playNextSong pops the first song from the playlist.
     * This function executes until the queue is not empty.
     * Inside loop, it waits if the songPlaying flag is true.
     * It waits on the songCondition variable with the uniqie_lock and _lock_ mutex of the class.
     * It wakes up after someone notify on the songCondition variable.
     * After waking up, it pops the song from the playlist, and sets the songPlaying flag to true.
     * And it notifies on the songPlaying variable.
     */
    void playNextSong();

    /**
     * @brief checkForException is used to check/monitor any exception during execution.
     * This method appies unique_lock on _lock_ mutex, and then goes into waiting using errorRaised condition variable.
     * It wakes from the sleep once anyone notify on the errorRaised condition variable.
     * After waking up, it displays the error message and terminates the program using exit(1).
     */
    void checkForException();

    /**
     * @brief pushSongIntoPlaylist method pushes the Song object into the playlist (queue).
     * @param song is the objecet of the Song, that needs to be pushed into the playlist.
     */
    void pushSongIntoPlaylist(const Song &song);

private:

    /** @brief songPlaying is the boolean that indicates either any song is being played or not. */
    bool songPlaying;

    /**
     * @brief _lock_ is the mutex lock,
     * which is used with the unique_lock and condition_vatiable to make thread wait and awake them.
     */
    std::mutex _lock_;

    /** @brief songCondition is the condition variable to synchronize the display and pop thread. */
    std::condition_variable songCondition;

    /** @brief errorRaised condition variable is used to send error monitoring thread into waiting,
     * and wake error thread back from the sleep.
     */
    std::condition_variable errorRaised;

    /** @brief playlist represents the song playlist which holds the songs to be played. */
    std::queue<Song> playlist;
};

#endif // DISPLAYDATA_H

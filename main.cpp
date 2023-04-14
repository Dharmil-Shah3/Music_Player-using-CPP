#include <iostream>
#include "song.h"
#include "displayplaylist.h"
#include <thread>
#include "logger.h"

using namespace std;

/***********************************************************************************************************//**
 * @brief push_songs_into_playlist is a global function that pushes objects of Song into queue of object of class DisplayPlaylist.
 *
 * This function is called from the main().\n
 * Objective of the method is to populate the playlist(queue) of an object of class DisplayPlaylist.
 * @param playlist is an object of DisplayPlaylist class, in which the Song object will be pushed.
 *************************************************************************************************************/
void push_songs_into_playlist(DisplayPlaylist &playlist);

/********************************   ********************************//**
 * @brief main method is used to handle the flow of the program.
 *
 * It creates an object of class DisplayPlaylist.\n
 * Calls push_songs_into_playlist() to push songs into that object.\n
 * Creates 3 threads that run paralally.\n
 * 1. for DisplayPlaylist::displaySongDetails() method.
 * 2. for DisplayPlaylist::playNextSong() method.
 * 3. for DisplayPlaylist::checkForException() method.
 * .
 * @return 0 on successfull execution, else returns 1.
 *******************************************************************/
int main()
{
    unique_ptr<Logger> logger(Logger::get());
    try {
        LOG(trace, "Execution Begin", NULL);

        DisplayPlaylist playlist;

        push_songs_into_playlist(playlist);

        int returnValueOfExceptionThread;

        thread t_playSongs(&DisplayPlaylist::playPlaylist, &playlist);
        thread t_monitorException(&DisplayPlaylist::monitorException, &playlist, ref(returnValueOfExceptionThread));
        thread t_changeSong(&DisplayPlaylist::playNextSong, &playlist);

        LOG(trace, "All threads are created", NULL);
        t_playSongs.join();
        t_changeSong.join();
        t_monitorException.join();

        LOG(trace, "All threads are joined", NULL);
        return returnValueOfExceptionThread;
    }
    catch (const exception &e) {
        LOG(error, "%s", e.what());
        return 1;
    }
}

void push_songs_into_playlist(DisplayPlaylist &playlist)
{
    try {
        LOG(trace, "Pushing the songs", NULL);
        Song song1("Daku", chrono::seconds(1), "/thumbnails/daku.jpeg");
        Song song2("Shape of You", chrono::seconds(1), "/thumbnails/shape_of_you.jpeg");
        Song song3("Dandelion", chrono::seconds(1), "/thumbnails/dandelion.jpeg");

        playlist.pushSongIntoPlaylist(song1);
        playlist.pushSongIntoPlaylist(song2);
        playlist.pushSongIntoPlaylist(song3);
        LOG(trace, "Pushed all songs", NULL);
    }
    catch (const exception &e) {
        LOG(error, "%s", e.what());
    }
}

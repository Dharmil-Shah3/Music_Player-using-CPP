#include <iostream>
#include "song.h"
#include "displayplaylist.h"
#include <thread>
using namespace std;

/**
 * @brief push_songs_into_playlist is a global function that pushes objects of Song into queue of object of class DisplayPlaylist.
 * This function is called from the main.\n
 * Objective of the method is to populate the playlist(queue) of an object of DisplayPlaylist.
 * @param playlist is an object of DisplayPlaylist class, in which the Song object will be pushed.
 */
void push_songs_into_playlist(DisplayPlaylist &playlist);

/**
 * @brief main method is used to handle the flow of the program.
 * It creates an object of DisplayPlaylist.\n
 * Calls push_songs_into_playlist() to push songs into that object.\n
 * Creates 3 threads that run paralally.\n
 * 1st thraed for DisplayPlaylist::displaySongDetails() method.\n
 * 2nd thraed for DisplayPlaylist::playNextSong() method.\n
 * And 3rd thraed for DisplayPlaylist::checkForException() method.\n
 * @return 0 on successfull execution, else returns 1.
 */
int main()
{
    try {
        DisplayPlaylist playlist;

        push_songs_into_playlist(playlist);

        int returnValueOfExceptionThread;

        thread t_displayData(&DisplayPlaylist::displaySongDetails, &playlist);
        thread t_monitorException(&DisplayPlaylist::monitorException, &playlist, ref(returnValueOfExceptionThread));
        thread t_changeSong(&DisplayPlaylist::playNextSong, &playlist);

        t_displayData.join();
        t_changeSong.join();
        t_monitorException.join();

        return returnValueOfExceptionThread;
    } catch (const exception &error) {
        cout << "\n => ERROR: " << error.what() << endl
             << "\t : in -> " << __PRETTY_FUNCTION__ << endl;
        return 1;
    }
}

void push_songs_into_playlist(DisplayPlaylist &playlist)
{
    try {
        Song song1("Daku", chrono::seconds(2), "/thumbnails/daku.jpeg");
        Song song2("Shape of You", chrono::seconds(4), "/thumbnails/shape_of_you.jpeg");
        Song song3("Dandelion", chrono::seconds(3), "/thumbnails/dandelion.jpeg");

        playlist.pushSongIntoPlaylist(song1);
        playlist.pushSongIntoPlaylist(song2);
        playlist.pushSongIntoPlaylist(song3);
    } catch (const exception &error) {
        cout << "\n => ERROR: " << error.what() << endl
             << "\t : in -> " << __PRETTY_FUNCTION__ << endl;
    }
}

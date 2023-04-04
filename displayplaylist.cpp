#include "displayplaylist.h"
#include <iomanip>
#include <ctime>

using namespace std;
using namespace SongError;

DisplayPlaylist::DisplayPlaylist(){
    this->songPlaying = true;
}

void DisplayPlaylist::pushSongIntoPlaylist(const Song &song){
    try {
        playlist.push(song);
    } catch (const exception &error) {
        cout << "\n => ERROR: " << error.what() << endl
             << "\t : in -> " << __PRETTY_FUNCTION__ << endl;
        errorRaised.notify_one();
    }
}

void DisplayPlaylist::displaySongDetails()
{
    try {
        if(playlist.empty()){
            cout << "\n\n => NO SONGS IN PLAYLIST <=\n" << endl;
            return;
        }
        while(playlist.size()>1 || songPlaying)
        {
            unique_lock<mutex> uniqueLock(_lock_);
            while (!songPlaying){ // wait until the song starts playing
                songCondition.wait(uniqueLock);
            }
            if(!uniqueLock.owns_lock())
                uniqueLock = unique_lock<mutex>(_lock_);

//            Custom exception throwing test
//            if(playlist.size() == 3)
//                throw ErrorCode::NO_INTERNET_CONNECTION;

            system("clear");

            chrono::seconds songLength = playlist.front().getDuration();

            printf("\n\n  ===== LALIFY MUSIC PLAYER =====\n");
            printf("\n\tSong   : %s\n", playlist.front().getName().c_str());
            cout << "\n\tLength : " << setfill('0') << setw(2) << (songLength.count()/60)
                 << ":" << setw(2) << (songLength.count()%60) << endl;

            // wait/sleep until the duration of the song is completed
            this_thread::sleep_for(songLength);

            // unlock after the song is played and notify the pop thread.
            uniqueLock.unlock();
            songPlaying = false;
            songCondition.notify_one();
        }
        cout<<"\n\n => Playlist Ended <=\n\n";
    }
    catch(const ErrorCode &error) {
        cout << "\n => ERROR: " << ErrorMessage::what(error) << endl
             << "\t : in -> " << __PRETTY_FUNCTION__ << endl;
        errorRaised.notify_one();
        return;
    }
    catch (const exception &error) {
        cout << "\n => ERROR: " << error.what() << endl
             << "\t : in -> " << __PRETTY_FUNCTION__ << endl;
        errorRaised.notify_one();
        return;
    }
}

void DisplayPlaylist::playNextSong()
{
    try {
        while(!playlist.empty())
        {
            unique_lock<mutex> uniqueLock(_lock_);
            while (songPlaying){
                songCondition.wait(uniqueLock);
            }
            if(!uniqueLock.owns_lock())
                uniqueLock = unique_lock<mutex>(_lock_);

            playlist.pop();
            songPlaying = true;
            uniqueLock.unlock();
            songCondition.notify_one();
        }
    }
    catch (const ErrorCode &error){
        cout << "\n => ERROR: " << ErrorMessage::what(error) << endl
             << "\t : in -> " << __PRETTY_FUNCTION__ << endl;
        errorRaised.notify_one();
        return;
    }
    catch (const exception &error){
        cout << "\n => ERROR: " << error.what() << endl
             << "\t : in -> " << __PRETTY_FUNCTION__ << endl;
        errorRaised.notify_one();
        return;
    }
}

void DisplayPlaylist::checkForException()
{
    try {
        unique_lock<mutex> uniqueLock(_lock_);
        errorRaised.wait(uniqueLock);
        songPlaying = false;
        printf("\n => ERROR: an exception occurred during execution, terminating the program.");
    } catch (const exception &error) {
        cout << "\n => ERROR: " << error.what() << endl
             << "\t : in " << __PRETTY_FUNCTION__ << endl;
    }
    exit(1);
}

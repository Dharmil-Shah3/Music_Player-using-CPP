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
        while(playlist.size()>1 || songPlaying)
        {
            unique_lock<mutex> uniqueLock;
            while (!songPlaying) // wait until the song starts playing
            {
                uniqueLock = unique_lock<mutex>(_lock_);
                songCondition.wait(uniqueLock);
            }

//            Custom exception throwing test
//            if(playlist.size() == 3)
//                throw ErrorCode::NO_INTERNET_CONNECTION;

            system("clear");

            chrono::seconds songLength = playlist.front().getDuration();
            auto songStartTime = chrono::system_clock::now();

            printf("\n\n  ===== LALIFY MUSIC PLAYER =====\n");
            cout << "\n\tSong   : " << playlist.front().getName() << endl;
            cout << "\n\tLength : " << setfill('0') << setw(2) << (songLength.count()/60)
                 << ":" << setw(2) << (songLength.count()%60) << endl;


            auto currentTime = chrono::system_clock::now();
            while(currentTime < (songStartTime + songLength))
            {
                currentTime = chrono::system_clock::now();
            }

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
            unique_lock<mutex> uniqueLock;
            while (songPlaying)
            {
                uniqueLock = unique_lock<mutex>(_lock_);
                songCondition.wait(uniqueLock);
            }
            playlist.pop();
            songPlaying = true;
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
        unique_lock<mutex> uinqueLock(_lock_);
        errorRaised.wait(uinqueLock);
        songPlaying = false;
        cout << "\n => ERROR: an exception occurred during execution, terminating the program." << endl;
    } catch (const exception &error) {
        cout << "\n => ERROR: " << error.what() << endl
             << "\t : in " << __PRETTY_FUNCTION__ << endl;
    }
    //std::terminate();
    exit(1);
}

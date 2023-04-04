#include "displayplaylist.h"
#include <iomanip>
#include <ctime>

using namespace std;
using namespace SongError;

DisplayPlaylist::DisplayPlaylist(){
    this->songPlaying = true;
}

DisplayPlaylist::~DisplayPlaylist(){
    // it makes sure to wake the error thread before destroying the object,
    // to save error thread from infinate waiting.
    this->errorRaised.notify_all();
}

void DisplayPlaylist::pushSongIntoPlaylist(const Song &song){
    try {
        playlist.push(song);
    } catch (const exception &error) {
        errorMessage = "\n => ERROR: "+string(error.what()) + "\n\t : in -> "+__PRETTY_FUNCTION__+"\n";
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

//            Custom exception throwing test
//            throw ErrorCode::NO_INTERNET_CONNECTION;

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
        errorMessage = "\n => ERROR: "+ErrorMessage::what(error) + "\n\t : in -> "+__PRETTY_FUNCTION__+"\n";
    }
    catch (const exception &error) {
        errorMessage = "\n => ERROR: "+string(error.what()) + "\n\t : in -> "+__PRETTY_FUNCTION__+"\n";
    }
    errorRaised.notify_one();
}

void DisplayPlaylist::playNextSong()
{
    try {
        while(!playlist.empty())
        {
            unique_lock<mutex> uniqueLock(_lock_);
            while (songPlaying){ // wait until the song is playing
                songCondition.wait(uniqueLock);
            }
            playlist.pop();
            songPlaying = true;
            uniqueLock.unlock();
            songCondition.notify_one();
        }
    }
    catch (const ErrorCode &error){
        errorMessage = "\n => ERROR: "+ErrorMessage::what(error) + "\n\t : in -> "+__PRETTY_FUNCTION__+"\n";
    }
    catch (const exception &error){
        errorMessage = "\n => ERROR: "+string(error.what()) + "\n\t : in -> "+__PRETTY_FUNCTION__+"\n";
    }
    errorRaised.notify_one();
}

void DisplayPlaylist::checkForException()
{
    try {
        // tempErrorLock is used instead of global _lock_ with unique_lock,
        // to maintain the proper sequence of the program.
        mutex tempErrorLock;
        unique_lock<mutex> uniqueLock(tempErrorLock);
        errorRaised.wait(uniqueLock, [&](){ return !errorMessage.empty(); });
        songPlaying = false;

        // to understand below condition, see the documentation of errorMessage variable.
        if(!errorMessage.empty()){
            cout << this->errorMessage;
            exit(1);
        }
    } catch (const exception &error) {
        cout << "\n => ERROR: " << error.what() << endl
             << "\t : in " << __PRETTY_FUNCTION__ << endl;
    }
}

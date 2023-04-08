#include "displayplaylist.h"
#include <iomanip>
#include "logger.h"

using namespace std;
using namespace SongError;

DisplayPlaylist::DisplayPlaylist()
{
    this->songPlaying = true;
    this->executionComplete = false;

    ostringstream threadId;
    threadId << this_thread::get_id();
    Logger::log(Trace, "MusicPlayer object created (thread id: %s)", threadId.str().c_str());
}

DisplayPlaylist::~DisplayPlaylist(){
    // it makes sure to wake the error thread before destroying the object,
    // to save error thread from infinate waiting.
    this->errorRaised.notify_all();
}

void DisplayPlaylist::pushSongIntoPlaylist(const Song &song){
    try {
        playlist.push(song);
        Logger::log(Trace, "Pushing song into playlist. Song id: %u, name: %s", song.getId(), song.getName().c_str());
    } catch (const exception &error) {
        errorMessage = string(error.what()) + " , in -> "+__PRETTY_FUNCTION__;
        Logger::log(Error, "%s", errorMessage.c_str());
    }
}

void DisplayPlaylist::playPlaylist()
{
    Logger::log(Trace, "Execution started -> displaySongDetails()");
    try {
        if(playlist.empty()){
            cout << "\n\n => NO SONGS IN PLAYLIST <=\n" << endl;
            executionComplete = true;
            return;
        }
        while(playlist.size()>1 || songPlaying)
        {
            Logger::log(Debug, "displaySongDetails() inside while loop");

            unique_lock<mutex> uniqueLock(_lock_);
            while (!songPlaying){ // wait until the song starts playing
                Logger::log(Debug, "displaySongDetails() is waiting");
                songCondition.wait(uniqueLock);
                if(executionComplete) return; // if any exception occures during execution, this flag will be true, means stop the execution.
            }

            Logger::log(Debug, "displaySongDetails() going to play a Song");
            /* Custom exception throwing test. Uncomment below line to throw exception. */
            //throw ErrorCode::NO_INTERNET_CONNECTION;

            system("clear"); // comment this if you want to display logs

            chrono::seconds songLength = playlist.front().getDuration();

            printf("\n\n  ===== LALIFY MUSIC PLAYER =====\n");
            printf("\n\tSong   : %s\n", playlist.front().getName().c_str());
            cout << "\n\tLength : " << setfill('0') << setw(2) << (songLength.count()/60)
                 << ":" << setw(2) << (songLength.count()%60) << endl;

            Logger::log(Debug, "Song Playing id: %u, name: %s", playlist.front().getId(), playlist.front().getName().c_str());

            /* wait/sleep until the duration of the song is completed */
            this_thread::sleep_for(songLength);

            Logger::log(Debug, "Song Completed id: %u, name: %s", playlist.front().getId(), playlist.front().getName().c_str());

            /* unlock after the song is played and notify the pop thread. */
            uniqueLock.unlock();
            songPlaying = false;
            songCondition.notify_one();
        }
        cout<<"\n\n => Playlist Completed... <=\n\n";
        Logger::log(Trace, "Playlist Completed");
    }
    catch(const ErrorCode &error) {
        errorMessage = ErrorMessage::what(error) + " , in -> "+__PRETTY_FUNCTION__;
        errorRaised.notify_all();
    }
    catch (const exception &error) {
        errorMessage = string(error.what()) + " , in -> "+__PRETTY_FUNCTION__;
        errorRaised.notify_all();
    }
    executionComplete = true;
    Logger::log(Trace, "Execution completed -> displaySongDetails()");
}

void DisplayPlaylist::playNextSong()
{
    Logger::log(Trace, "Execution started -> playNextSong()");
    try {
        while(!playlist.empty())
        {
            Logger::log(Debug, "playNextSong() inside while loop");

            unique_lock<mutex> uniqueLock(_lock_);
            while (songPlaying){ // wait until the song stops playing
                Logger::log(Debug, "playNextSong() is waiting");
                songCondition.wait(uniqueLock);
                if(executionComplete) return; // if any exception occures during execution, this flag will be true, means stop the execution.
            }
            Logger::log(Debug, "playNextSong() is going to pop song id: %u, name: %s", playlist.front().getId(), playlist.front().getName().c_str());
            playlist.pop();
            songPlaying = true;
            uniqueLock.unlock();
            songCondition.notify_one();
            Logger::log(Debug, "playNextSong() popped song");
        }
    }
    catch (const ErrorCode &error){
        errorMessage = ErrorMessage::what(error)+" , in -> "+__PRETTY_FUNCTION__;
        errorRaised.notify_all();
    }
    catch (const exception &error){
        errorMessage = string(error.what())+" , in -> "+__PRETTY_FUNCTION__;
        errorRaised.notify_all();
    }
    executionComplete = true;
    Logger::log(Trace, "Execution completed of playNextSong()");
}

void DisplayPlaylist::monitorException(int &returnValue)
{
    Logger::log(Trace, "Execution started -> monitorException()");
    try {
        /*
         * tempErrorLock is used instead of global _lock_ with unique_lock,
         * to maintain the proper sequence of the program.
         */

        mutex tempErrorLock;
        unique_lock<mutex> uniqueLock(tempErrorLock);

        /* wait until either execution completed or exception occured. */
        while(!executionComplete && errorMessage.empty()){
            errorRaised.wait_for(uniqueLock, chrono::milliseconds(333));
        }
        Logger::log(Debug, "monitorException() is waken up");
        returnValue = 0;
        songPlaying = false;

        /* to understand the use of below condition, see the documentation of errorMessage variable. */
        if(!errorMessage.empty())
        {
            Logger::log(Error, "%s", errorMessage.c_str());
            returnValue = 1;
            songCondition.notify_all(); // to wake all the sleeping threads so that they can end their execution.
        }
    } catch (const exception &error) {
        cout << "\n => ERROR: " << error.what() << endl
             << "\t : in " << __PRETTY_FUNCTION__ << endl;
        Logger::log(Error, "%s , in -> %s", error.what(), __PRETTY_FUNCTION__);
        returnValue = 1;
    }
    Logger::log(Trace, "Execution completed -> monitorException()");
}

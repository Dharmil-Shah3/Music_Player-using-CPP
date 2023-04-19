#include "displayplaylist.h"
#include <iomanip>
#include "logger.h"

using namespace std;
using namespace SongError;

DisplayPlaylist::DisplayPlaylist()
{
    this->songPlaying = true;
    this->executionComplete = false;
    LOG(trace, "MusicPlayer object created");
}

DisplayPlaylist::~DisplayPlaylist(){
    // it makes sure to wake the error thread before destroying the object,
    // to save error thread from infinate waiting.
    LOG(trace, "Execution Begin");
    this->errorRaised.notify_all();
    LOG(trace, "Execution End");
}

void DisplayPlaylist::pushSongIntoPlaylist(const Song &song){
    try {
        playlist.push(song);
        LOG(trace, "Pushing song into playlist. Song id: "+to_string(song.getId())+", name: "+song.getName());
    } catch (const exception &e) {
        LOG(error, e.what());
    }
}

void DisplayPlaylist::playPlaylist()
{
    LOG(trace, "Execution Begin");
    try {
        if(playlist.empty()){
            LOG(trace,"NO SONGS IN PLAYLIST");
            executionComplete = true;
            return;
        }
        while(playlist.size()>1 || songPlaying)
        {
            LOG(debug, "displaySongDetails() inside while loop");

            unique_lock<mutex> uniqueLock(_lock_);
            while (!songPlaying){ // wait until the song starts playing
                LOG(debug, "displaySongDetails() is waiting");
                songCondition.wait(uniqueLock);
                if(executionComplete) return; // if any exception occures during execution, this flag will be true, means stop the execution.
            }

            LOG(debug, "displaySongDetails() going to play a Song");
            /* Custom exception throwing test. Uncomment below line to throw exception. */
            //throw ErrorCode::NO_INTERNET_CONNECTION;

            system("clear"); // comment this if you want to display logs

            chrono::seconds songLength = playlist.front().getDuration();

            printf("\n\n  ===== LALIFY MUSIC PLAYER =====\n");
            printf("\n\tSong   : %s\n", playlist.front().getName().c_str());
            cout << "\n\tLength : " << setfill('0') << setw(2) << (songLength.count()/60)
                 << ":" << setw(2) << (songLength.count()%60) << endl;

            LOG(debug, "Song Playing id: "+to_string(playlist.front().getId())+", name: "+playlist.front().getName());

            /* wait/sleep until the duration of the song is completed */
            this_thread::sleep_for(songLength);

            LOG(debug, "Song Completed id: "+to_string(playlist.front().getId())+", name: "+playlist.front().getName());

            /* unlock after the song is played and notify the pop thread. */
            uniqueLock.unlock();
            songPlaying = false;
            songCondition.notify_one();
        }
        LOG(trace, "Playlist Completed");
    }
    catch(const ErrorCode &error) {
        errorMessage = ErrorMessage::what(error) + " , in -> %s"+__PRETTY_FUNCTION__;
        errorRaised.notify_all();
    }
    catch (const exception &error) {
        errorMessage = string(error.what()) + " , in -> %s"+__PRETTY_FUNCTION__;
        errorRaised.notify_all();
    }
    executionComplete = true;
    LOG(trace, "Execution End");
}

void DisplayPlaylist::playNextSong()
{
    LOG(trace, "Execution Begin");
    try {
        while(!playlist.empty())
        {
            LOG(debug, "playNextSong() inside while loop");

            unique_lock<mutex> uniqueLock(_lock_);
            while (songPlaying) // wait until the song stops playing
            {
                LOG(debug, "playNextSong() is waiting");
                songCondition.wait(uniqueLock);
                if(executionComplete) return; // if any exception occures during execution, this flag will be true, means stop the execution.
            }
            LOG(debug, "playNextSong() is poping song id: "+to_string(playlist.front().getId())+", name: "+playlist.front().getName());

            playlist.pop();
            songPlaying = true;
            uniqueLock.unlock();
            songCondition.notify_one();
            LOG(debug, "playNextSong() popped song");
        }
    }
    catch (const ErrorCode &e){
        errorMessage = ErrorMessage::what(e)+" , in -> %s"+__PRETTY_FUNCTION__;
        errorRaised.notify_all();
        LOG(error, ErrorMessage::what(e));
    }
    catch (const exception &e){
        errorMessage = string(e.what())+" , in -> %s"+__PRETTY_FUNCTION__;
        errorRaised.notify_all();
        LOG(error, e.what());
    }
    executionComplete = true;
    LOG(trace, "Execution End");
}

void DisplayPlaylist::monitorException(int &returnValue)
{
    LOG(trace, "Execution Begin");
    try {
        /* tempErrorLock is used instead of global _lock_ with unique_lock,
         * to maintain the proper sequence of the program. */
        mutex tempErrorLock;
        unique_lock<mutex> uniqueLock(tempErrorLock);

        /* wait until either execution completed or exception occured. */
        while(!executionComplete && errorMessage.empty()){
            errorRaised.wait_for(uniqueLock, chrono::milliseconds(333));
        }
        LOG(debug, "monitorException() is waken up");
        returnValue = 0;
        songPlaying = false;

        /* to understand the use of below condition, see the documentation of errorMessage variable. */
        if(!errorMessage.empty())
        {
            cout << "\n ERROR: " << errorMessage << endl;
            executionComplete = true;
            returnValue = 1;
            songCondition.notify_all(); // to wake all the sleeping threads so that they can end their execution.
        }
    } catch (const exception &e) {
        LOG(error, e.what());
        executionComplete = true;
        returnValue = 1;
    }
    LOG(trace, "Execution End");
}

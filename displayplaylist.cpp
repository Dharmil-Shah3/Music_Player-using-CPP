#include "displayplaylist.h"
#include <iomanip>
#include "logger.h"

using namespace std;
using namespace SongError;

DisplayPlaylist::DisplayPlaylist(){
    this->songPlaying = true;
    this->executionComplete = false;
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
    }
}

void DisplayPlaylist::displaySongDetails()
{
    try {
        if(playlist.empty()){
            cout << "\n\n => NO SONGS IN PLAYLIST <=\n" << endl;
            executionComplete = true;
            return;
        }
        while(playlist.size()>1 || songPlaying)
        {
            unique_lock<mutex> uniqueLock(_lock_);
            while (!songPlaying){ // wait until the song starts playing
                songCondition.wait(uniqueLock);
                if(executionComplete) return; // if any exception occures during execution, this flag will be true, means stop the execution.
            }

            /* Custom exception throwing test. Uncomment below line to throw exception. */
            //throw ErrorCode::NO_INTERNET_CONNECTION;

            system("clear"); // comment this if you want to display logs

            chrono::seconds songLength = playlist.front().getDuration();

            printf("\n\n  ===== LALIFY MUSIC PLAYER =====\n");
            printf("\n\tSong   : %s\n", playlist.front().getName().c_str());
            cout << "\n\tLength : " << setfill('0') << setw(2) << (songLength.count()/60)
                 << ":" << setw(2) << (songLength.count()%60) << endl;

            /* wait/sleep until the duration of the song is completed */
            this_thread::sleep_for(songLength);

            /* unlock after the song is played and notify the pop thread. */
            uniqueLock.unlock();
            songPlaying = false;
            songCondition.notify_one();
        }
        cout<<"\n\n => Playlist Completed... <=\n\n";
    }
    catch(const ErrorCode &error) {
        errorMessage = "\n => ERROR: "+ErrorMessage::what(error) + "\n\t : in -> "+__PRETTY_FUNCTION__+"\n";
        errorRaised.notify_all();
    }
    catch (const exception &error) {
        errorMessage = "\n => ERROR: "+string(error.what()) + "\n\t : in -> "+__PRETTY_FUNCTION__+"\n";
        errorRaised.notify_all();
    }
    executionComplete = true;
}

void DisplayPlaylist::playNextSong()
{
    try {
        while(!playlist.empty())
        {
            unique_lock<mutex> uniqueLock(_lock_);
            while (songPlaying){ // wait until the song stops playing
                songCondition.wait(uniqueLock);
                if(executionComplete) return; // if any exception occures during execution, this flag will be true, means stop the execution.
            }
            playlist.pop();
            songPlaying = true;
            uniqueLock.unlock();
            songCondition.notify_one();
        }
    }
    catch (const ErrorCode &error){
        errorMessage = "\n => ERROR: "+ErrorMessage::what(error)+"\n\t : in -> "+__PRETTY_FUNCTION__+"\n";
        errorRaised.notify_all();
    }
    catch (const exception &error){
        errorMessage = "\n => ERROR: "+string(error.what())+"\n\t : in -> "+__PRETTY_FUNCTION__+"\n";
        errorRaised.notify_all();
    }
    executionComplete = true;
}

void DisplayPlaylist::monitorException(int &returnValue)
{
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
        returnValue = 0;
        songPlaying = false;

        /* to understand the use of below condition, see the documentation of errorMessage variable. */
        if(!errorMessage.empty()){
            cout << this->errorMessage;
            returnValue = 1;
            songCondition.notify_all(); // to wake all the sleeping threads so that they can end their execution.
        }
    } catch (const exception &error) {
        cout << "\n => ERROR: " << error.what() << endl
             << "\t : in " << __PRETTY_FUNCTION__ << endl;
        returnValue = 1;
    }
}

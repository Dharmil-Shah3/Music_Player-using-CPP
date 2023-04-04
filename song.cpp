#include "song.h"
using namespace std;

unsigned int Song::totalSongs = 0;

Song::Song(const string &name,
           const chrono::seconds &duration,
           const string &thumbnailPath)
{
    this->id = ++totalSongs;
    this->name = name;
    this->duration = duration;
    this->thumbnailPath = thumbnailPath;
}

Song::~Song(){}

unsigned int Song::getId() const { return  this->id; }
string Song::getName() const { return this->name; }
string Song::getThumbnailPath() const { return this->thumbnailPath; }
chrono::seconds Song::getDuration() const { return this->duration; }

string SongError::ErrorMessage::what(const ErrorCode &errorCode)
{
    switch (errorCode){
        case CORRUPTED_SONG: return "Song file is corrupted";
        case CORRUPTED_THUMBNAIL: return "Thumbnail file is corrupted";
        case NO_INTERNET_CONNECTION: return "No Internet connection found";
        default: return "Undefined exception";
    }
}

#ifndef SONG_H
#define SONG_H

#include <iostream>
#include <chrono>

/**
 * @brief The Song class represents a song, and contains song related attributes and methods.
 *
 * The Song class contains song attributes such as
 *  1) unique id of the song (auto-generated)
 *  2) Name of the song
 *  3) Duration of the songs (in chrono seconds)
 *  4) thubnail's path of the song
 *
 * In addition, it has also a static attribute named totalSongs.
 * totalSongs is used to provide the auto-generated id's to each object of the Song class.
 * initially it's zero(0), but it's value is being incremented in the constructor of Song class.
 *
 * It also provides the getter methods to get all value of the all the attributes listed above except totalSongs,
 * since all the attributes are private.
 */
class Song
{
public:
    /**
     * @brief Song is the parameterised constructor.
     * @param name of the song.
     * @param duration of the song in seconds.
     * @param thumbnailPath is the path of song thumbnail.
     */
    Song(const std::string &name,
         const std::chrono::seconds &duration,
         const std::string &thumbnailPath);

    /** @brief ~Song destructor. */
    ~Song();

    /**
     * @brief getId returns the unique id of the song.
     * @return id of the song.
     */
    unsigned int getId() const;
    /**
     * @brief getName returns the name of the song.
     * @return name of the song.
     */
    std::string getName() const;
    /**
     * @brief getThumbnailPath returns the path of the song's thumbnail.
     * @return thumbnailPath of the song.
     */
    std::string getThumbnailPath() const;
    /**
     * @brief getDuration returns the duration of the time in chrono::seconds.
     * @return duration of the song.
     */
    std::chrono::seconds getDuration() const;

private:

    /** @brief totalSongs is a static attribute used to allocate dynamic id to the songs in the constructor. */
    static unsigned int totalSongs;

    /** @brief id is the unique id of the song. */
    unsigned int id;

    /** @brief name of the song. */
    std::string name;

    /** @brief duration of the song in form of chrono::seconds. */
    std::chrono::seconds duration;

    /** @brief thumbnailPath is the path of the thumbnail image of the song. */
    std::string thumbnailPath;
};


/**
 * @namespace SongError
 * @brief SongError is a bundle of custom Song exceptions.
 * It contains the error codes and related messages too.
 */
namespace SongError
{
    /**
     * @brief The ErrorCode enum contains the errors related to Song class.
     * This enum is used to throw the Song related errors.
     */
    enum ErrorCode
    {
        CORRUPTED_SONG,
        CORRUPTED_THUMBNAIL,
        NO_INTERNET_CONNECTION
    };

    /**
     * @brief The ErrorMessage struct contains error messages of elements of ErrorCode enum.
     * This is an abstact structure that has a static method what(),
     * which takes ErrorCode element as an argument,
     * and returns the related error message.
     */
    struct ErrorMessage
    {
        /**
         * @brief what returns the error message of the given ErrorCode.
         * @param errorCode is an attribute of enum ErrorCode.
         * @return the error message related to the argument passed of ErrorCode.
         */
        static std::string what(const ErrorCode &errorCode);

        private:
            virtual void pure_virtual_function_to_make_this_class_abstract() = 0;
    };
}

#endif // SONG_H

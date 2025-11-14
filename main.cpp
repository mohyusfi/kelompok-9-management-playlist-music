#include <iostream>
#include <deque>
#include <algorithm>
#include <thread>

using namespace std;
typedef unsigned int ID_t;

typedef unsigned int ID_t;

struct User {
    ID_t id;
    string username;
    string password;
    string role;
    deque<ID_t> playlist_ids;
};

struct Music {
    ID_t id;
    string title;
    string artist;
    int duration_seconds;
};

struct Playlist {
    ID_t id;
    string name;
    string owner_username;
    deque<ID_t> music_ids;
};

// penyimpanan data
deque<User> users = {};
deque<Music> musics = {};
deque<Playlist> playlists = {};


int main()
{

}
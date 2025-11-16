#include <iostream>
#include <deque>
#include <algorithm>
#include <thread>

using namespace std;
typedef unsigned int ID_t;

struct User {
    ID_t id;
    string username;
    string password;
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

// Membuat id
ID_t generate_user_id();
ID_t generate_music_id();
ID_t generate_playlist_id();

int find_user_index_by_username(const string &username);
int find_music_index_by_id(ID_t id);
int find_playlist_index_by_id(ID_t id);

void clear_input();

bool register_user();
ID_t login_user(string &out_username);

// music
void list_all_music();

// Playlist manajemen
bool is_pl_id_in_playlist_ids(ID_t playlist_id, const string &username);
void add_pl_to_playlis_ids(ID_t playlist_id, const string &username);
void list_playlist_current_user(const string &username);
void list_playlists_all_user();
void show_playlist_contents(const Playlist &playlist);
void delete_user_playlist(ID_t playlist_id, const string &username);
void update_user_playlist(ID_t playlist_id, const string &username);
void user_create_playlist(const string &username);

bool is_music_already_in_playlist(ID_t music_id, ID_t playlist_id);
void add_music_to_playlist(const string &current_user);
void play_playlist(Playlist &playlist);

// Manajemen Menu
void user_menu(string &username);
void playlist_menu(const string &username, bool current_user = false);

// sample data
void seed_user();
void data_dummy();

int main() {
    seed_user();
    data_dummy();
    cout << "Welcome To Spotify Versi Jadul\n";
    while (true) {
        cout << "\n=== MAIN MENU ===\n"
                << "1) Register\n"
                << "2) Login\n"
                << "0) Exit\n"
                << "Choose: ";
        int choice;
        cin >> choice;
        if (choice == 1) {
            register_user();
        } else if (choice == 2) {
            string uname;
            ID_t uid = login_user(uname);
            if (uid != -1) {
                user_menu(uname);
            }
        } else if (choice == 0) {
            cout << "Bye.\n";
            break;
        } else {
            cout << "Pilihan tidak valid.\n";
        }
    }
    return 0;
}

ID_t generate_user_id() {
    static ID_t counter = 1;
    return counter++;
}

ID_t generate_music_id() {
    static ID_t counter = 1;
    return counter++;
}

ID_t generate_playlist_id() {
    static ID_t counter = 1;
    return counter++;
}

// utils
void clear_input() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

int find_user_index_by_username(const string &username) {
    for (size_t i = 0; i < users.size(); ++i) {
        if (users[i].username == username) return (int) i;
    }
    return -1;
}

int find_music_index_by_id(const ID_t id) {
    for (size_t i = 0; i < musics.size(); ++i) {
        if (musics[i].id == id) return (int) i;
    }
    return -1;
}

int find_playlist_index_by_id(const ID_t id) {
    for (size_t i = 0; i < playlists.size(); ++i) {
        if (playlists[i].id == id) return (int) i;
    }
    return -1;
}

bool register_user() {
    cout << "\n=== REGISTER ===\n";
    string username;
    cout << "Username: ";
    cin >> username;
    if (find_user_index_by_username(username) != -1) {
        cout << "Username sudah dipakai.\n";
        return false;
    }
    string password;
    cout << "Password: ";
    cin >> password;
    users.push_back({generate_user_id(), username, password});
    cout << "Register sukses. Lu sekarang user.\n";
    return true;
}

ID_t login_user(string &out_username) {
    cout << "\n=== LOGIN ===\n";
    string username, password;
    cout << "Username: ";
    cin >> username;
    cout << "Password: ";
    cin >> password;
    int idx = find_user_index_by_username(username);
    if (idx == -1) {
        cout << "User tidak ditemukan.\n";
        return -1;
    }

    if (users[idx].password != password) {
        cout << "Password salah.\n";
        return -1;
    }

    out_username = users[idx].username;
    cout << "Login berhasil" << "\n";
    return users[idx].id;
}

void list_all_music() {
    cout << "\n--- ALL MUSIC ---\n";
    if (musics.empty()) {
        cout << "(kosong)\n";
        return;
    }
    for (auto &m: musics) {
        cout << "id:" << m.id
                << " | " << m.title
                << " - " << m.artist
                << " (" << m.duration_seconds << "s)\n";
    }
}

bool is_pl_id_in_playlist_ids(ID_t playlist_id, const string &username) {
    int user_index = find_user_index_by_username(username);

    for (auto &pl_id: users.at(user_index).playlist_ids) {
        if (pl_id == playlist_id) return true;
    }

    return false;
}

void add_pl_to_playlis_ids(ID_t playlist_id, const string &username) {
    bool is_exists = is_pl_id_in_playlist_ids(playlist_id, username);

    if (is_exists) {
        cout << "playlist telah ada dikoleksimu mas" << endl;
        return;
    }

    int user_index = find_user_index_by_username(username);
    users.at(user_index).playlist_ids.push_back(playlist_id);
    cout << "playlist dengan id:" << users.at(user_index).playlist_ids.back() << " ditambahkan ke koleksimu" "\n";
}


void list_playlist_current_user(const string &username) {
    int user_index = find_user_index_by_username(username);
    if (user_index == -1) {
        cout << "user tidak di termukan" << endl;
        return;
    }

    User &current_user = users.at(user_index);

    cout << "\n--- PLAYLISTS ---\n";
    if (current_user.playlist_ids.empty()) {
        cout << "(kosong)\n";
        return;
    }

    for (size_t i = 0; i < current_user.playlist_ids.size(); i++) {
        int pl_index = find_playlist_index_by_id(current_user.playlist_ids.at(i));
        if (pl_index != -1) {
            Playlist &p = playlists.at(pl_index);
            cout << "id:" << p.id << " | " << p.name << " | pemilik:" << p.owner_username
                    << " | Jumlah Musik:" << p.music_ids.size() << "\n";
        }
    }
}

void list_playlists_all_user() {
    cout << "\n--- PLAYLISTS ---\n";
    if (playlists.empty()) {
        cout << "(kosong)\n";
        return;
    }
    for (auto &p: playlists) {
        cout << "id:" << p.id << " | " << p.name << " | owner:" << p.owner_username
                << " | Jumlah Musik:" << p.music_ids.size() << "\n";
    }
}

void show_playlist_contents(const Playlist &playlist) {
    cout << "--- Daftar Musik DI " << playlist.name << " ---" << endl;
    cout << "\nPlaylist: " << playlist.name << " (id:" << playlist.id << ") pemilik:" << playlist.owner_username <<
            "\n";
    if (playlist.music_ids.empty()) {
        cout << "(kosong)\n";
        return;
    }
    for (auto mid: playlist.music_ids) {
        int music_index = find_music_index_by_id(mid);
        if (music_index != -1) {
            auto &m = musics[music_index];
            cout << " - " << m.id << " | " << m.title << " - " << m.artist << " (" << m.duration_seconds << "s)\n";
        } else {
            cout << " - " << mid << " | (music tidak ditemukan)\n";
        }
    }
}

void delete_user_playlist(const ID_t playlist_id, const string &username) {
    int pl_index = find_playlist_index_by_id(playlist_id);
    int user_index = find_user_index_by_username(username);

    if (pl_index == -1 || user_index == -1) {
        cout << "Playlist Tidak Ditemukan.\n";
        return;
    }

    auto &playlist_ids = users.at(user_index).playlist_ids;

    if (username != playlists.at(pl_index).owner_username) {
        if (is_pl_id_in_playlist_ids(playlist_id, username) == true) {
            playlist_ids.erase(
                remove(playlist_ids.begin(), playlist_ids.end(), playlist_id),
                playlist_ids.end()
            );
            cout << "playlist berhasil dikoleksimu mas" << endl;
            return;
        }
        cout << "tidak bisa delete playlist orang bego" << endl;
        return;
    }

    playlist_ids.erase(
        remove(playlist_ids.begin(), playlist_ids.end(), playlist_id),
        playlist_ids.end()
    );

    playlists.erase(playlists.begin() + pl_index);
    cout << "playlist dengan id:" << playlist_id << "berhasil di hapus" << endl;
}

void update_user_playlist(const ID_t playlist_id, const string &username) {
    int pl_index = find_playlist_index_by_id(playlist_id);

    if (playlists.at(pl_index).owner_username != username) {
        cout << "Can't update playlist\n";
        return;
    }

    string new_pl_name;
    cout << "New Name: ";
    getline(cin >> ws, new_pl_name);

    if (pl_index == -1) {
        cout << "Playlist Id not found.\n";
        return;
    }

    playlists.at(pl_index).name = new_pl_name;
    cout << "Success Update playlist" << endl;
}


void user_create_playlist(const string &username) {
    cout << "\n=== CREATE PLAYLIST ===\n";
    clear_input();

    string name;
    cout << "Playlist name: ";
    getline(cin, name);

    playlists.push_back({
        .id = generate_playlist_id(),
        .name = name,
        .owner_username = username,
        .music_ids = {},
    });

    int user_index = find_user_index_by_username(username);
    users.at(user_index).playlist_ids.push_back(playlists.back().id);

    cout << "Playlist dibuat id=" << playlists.back().name << "\n";
}

bool is_music_already_in_playlist(ID_t music_id, ID_t playlist_id) {
    int pl_id = find_playlist_index_by_id(playlist_id);
    for (const auto &id: playlists.at(pl_id).music_ids) {
        if (id == music_id) {
            return true;
        }
    }
    return false;
}

void add_music_to_playlist(const string &current_user) {
    cout << "\nMasukkan playlist id target: ";
    int input_pl_index;
    cin >> input_pl_index;
    int pl_index = find_playlist_index_by_id(input_pl_index);
    if (pl_index == -1) {
        cout << "Playlist tidak ada.\n";
        return;
    }


    if (playlists[pl_index].owner_username != current_user) {
        cout << "Akses ditolak. Kamu hanya bisa menambahkan ke playlist milikmu.\n";
        return;
    }

    int mid;
    cout << "Masukkan music id yang mau ditambahkan: ";
    cin >> mid;

    int midx = find_music_index_by_id(mid);
    if (midx == -1) {
        cout << "Music tidak ada.\n";
        return;
    }

    if (is_music_already_in_playlist(mid, playlists.at(pl_index).id) == true) {
        cout << "Musik Telah ada di playlist ini" << endl;
        return;
    }

    playlists[pl_index].music_ids.push_back(mid);
    cout << "Music ditambahkan ke playlist.\n";
}

void play_playlist(Playlist &playlist) {
    if (playlist.music_ids.empty()) {
        cout << "Music Tidak ada Tambah dulu.\n";
        return;
    }

    for (auto const &music_id: playlist.music_ids) {
        int music_index = find_music_index_by_id(music_id);
        if (music_index != -1) {
            cout << musics.at(music_index).title << " is Playingg now......." << endl;
            this_thread::sleep_for(chrono::milliseconds(500));
        }
    }
}

void playlist_menu(const string &username, bool current_user) {
    while (true) {
        if (current_user) {
            list_playlist_current_user(username);
        } else {
            list_playlists_all_user();
        }

        cout << "\n=== PLAYLIST MENU ===\n"
                << "1) Putar Playlist \n"
                << "2) Delete playlist\n"
                << "3) Edit playlists\n"
                << "4) Tampilkan Daftar Musik\n"
                << "0) Keluar\n";

        int option;
        cout << "menu: ";
        cin >> option;

        if (option == 0) return;

        int playlist_id;
        cout << "ID Playlist: ";
        cin >> playlist_id;

        if (current_user) {
            if (!is_pl_id_in_playlist_ids(playlist_id, username)) {
                cout << "Playlist tidak ada di koleksi." << endl;
                return;
            }
        }

        int pl_index = find_playlist_index_by_id(playlist_id);

        if (pl_index != -1) {
            if (option == 1) {
                play_playlist(playlists.at(pl_index));
            } else if (option == 2) {
                delete_user_playlist(playlist_id, username);
            } else if (option == 3) {
                update_user_playlist(playlist_id, username);
            } else if (option == 4) {
                show_playlist_contents(playlists.at(pl_index));
                if (current_user != true) {
                    string choice;
                    cout << "Tambah ke koleksi 1:(yes)/0:(no)? ";
                    getline(cin >> ws, choice);
                    if (choice == "1") {
                        add_pl_to_playlis_ids(playlist_id, username);
                    }
                }
            } else {
                cout << "Pilihan tidak valid.\n";
            }
        } else {
            cout << "ID playlist tidak di temukan\n";
        }
    }
}

void user_menu(string &username) {
    while (true) {
        cout << "\n=== USER MENU ===\n"
                << "1) Buat playlist\n"
                << "2) List playlists\n"
                << "3) Tambah Musik ke Playlist\n"
                << "4) List Semua music\n"
                << "5) Koleksimu\n"
                << "0) Logout\n"
                << "Pilih: ";
        int option;
        cin >> option;
        switch (option) {
            case 1: user_create_playlist(username);
                break;
            case 2: {
                playlist_menu(username, false);
                break;
            }
            case 3:
                list_all_music();
                list_playlists_all_user();
                add_music_to_playlist(username);
                break;
            case 4: list_all_music();
                break;
            case 5: {
                playlist_menu(username, true);
                break;
            }
            case 0: return;
            default: cout << "Pilihan tidak valid.\n";
        }
    }
}

void seed_user() {
    users.push_back({
        .id = generate_user_id(),
        .username = "yusfi",
        .password = "123",
        .playlist_ids = {},
    });

    playlists.push_back({
        .id = generate_playlist_id(),
        .name = "Coding Booster",
        .owner_username = users.back().username,
    });
    users.back().playlist_ids.push_back(playlists.back().id);

    users.push_back({
        .id = generate_user_id(),
        .username = "rizal",
        .password = "123",
        .playlist_ids = {},
    });

    playlists.push_back({
        .id = generate_playlist_id(),
        .name = "Playlist Wibu",
        .owner_username = users.back().username,
    });
    users.back().playlist_ids.push_back(playlists.back().id);
}

void data_dummy() {
    string title[50] = {
        "Blue", "Garam & Madu", "Mangu", "Kita Usahakan Lagi", "Lesung Pipi",
        "Tanpa Cinta", "Pretty Little Baby", "Stecu", "That's So True", "H.S.K.T",
        "Moonlight Echo", "Fading Neon", "Midnight Run", "Silent Waves", "Broken Frame",
        "Dreamwalker", "After the Rain", "Lost Coordinates", "Velvet Sky", "Running Slow",
        "Summer Calls", "Golden Hour Glass", "Static Heart", "Drowned Voices", "Paper Crown",
        "Crystal Sun", "Under the Bridge Lights", "Neon Valley", "Glassbound", "Digital Breeze",
        "Shadowtone", "Lightphase", "Horizon Line", "Snowrunner", "Faultline Nights",
        "Wonderdusk", "Rainfall Sketch", "Timeless Echo", "Gravity Sleep", "Bright Again",
        "Echo Chamber", "Sundown Memory", "Cold Coffee", "Minute Before Dawn", "Airplane Glow",
        "Lost City Radio", "Silver Path", "Night Arcade", "Sunset Rewind", "Skyless"
    };

    string artist[50] = {
        "Yung Kai", "Tenxi", "Fourtwnty", "Batas Senja", "Raim Laode",
        "Tiara Andini", "Connie Francis", "Faris Adam", "Gracie Abrams", "Lee Hi",
        "Arden Vox", "Lumen Gray", "Kaito Blue", "Vierra Solis", "Moonshift",
        "Ethereal One", "Atlas Vale", "Nira Kaze", "Ollin Ray", "Juno M",
        "Soft Season", "Amber Lain", "Kiro Wave", "Della Nova", "Ives Oracle",
        "Ryn Marlowe", "Velanote", "Cryden", "Juniper Vale", "Nova Faye",
        "Zed Avalon", "Leora Hale", "Soren Vale", "Cryst Lys", "Veria Noir",
        "Ember Kid", "Miko Shore", "Caelin Drift", "Draey Luna", "Kyen Mori",
        "Sable Arc", "Noir Vale", "Auri Lix", "Greyson Hale", "Rhea Mori",
        "Ollan Drift", "Sierra Vale", "Cryo Pulse", "Nova Aure", "Vyre Sol"
    };

    int duration[50] = {
        192, 168, 241, 225, 190,
        236, 142, 185, 178, 213,
        221, 175, 252, 199, 169,
        207, 230, 245, 182, 238,
        224, 250, 181, 177, 216,
        248, 206, 239, 173, 228,
        200, 194, 160, 261, 238,
        213, 171, 240, 189, 227,
        191, 179, 214, 208, 196,
        243, 164, 233, 205, 198
    };

    for (int i = 0; i < 50; ++i) {
        musics.push_back({
            generate_music_id(),
            title[i],
            artist[i],
            duration[i]
        });
    }
}
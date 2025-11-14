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

// Membuat id
ID_t generate_user_id();
ID_t generate_music_id();
ID_t generate_playlist_id();

int find_user_index_by_username(const string &username);
int find_music_index_by_id(ID_t id);
int find_playlist_index_by_id(ID_t id);

void clear_input();

bool register_user();
ID_t login_user(string &out_username, string &out_role);

void add_music();
void list_all_music();
void update_music();
void delete_music();

// Playlist manajemen (admin & user create)
void list_playlists_all_user();
void show_playlist_contents(const Playlist &playlist);
void delete_user_playlist(ID_t playlist_id, const string &username);
void update_user_playlist(ID_t playlist_id, const string &username);
void user_create_playlist(const string &username);
void admin_add_playlist();
void admin_update_playlist();
void admin_delete_playlist();

void add_music_to_playlist(const string &current_user, bool admin_mode);
void play_playlist(Playlist &playlist);

// Manajemen Menu
void admin_menu(const string &username);
void user_menu(const string &username);
void playlist_menu(const string &username);

// sample data
void seed_admin();
void data_dummy();


int main()
{


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
    users.push_back({generate_user_id(), username, password, "user"});
    cout << "Register sukses. Lu sekarang user.\n";
    return true;
}

ID_t login_user(string &out_username, string &out_role) {
    cout << "\n=== LOGIN ===\n";
    string username, password;
    cout << "Username: "; cin >> username;
    cout << "Password: "; cin >> password;
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
    out_role = users[idx].role;
    cout << "Login berhasil. Role: " << out_role << "\n";
    return users[idx].id;
}

void add_music() {
    cout << "\n=== ADD MUSIC ===\n";
    string title, artist;
    int dur;
    clear_input();
    cout << "Title: "; getline(cin, title);
    cout << "Artist: "; getline(cin, artist);
    cout << "Duration (seconds): "; cin >> dur;

    musics.push_back({
        generate_music_id(),
        title,
        artist,
        dur
    });

    cout << "Music ditambah. name = " << musics.back().title << "\n";
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

void update_music() {
    list_all_music();

    ID_t id;
    cout << "\nMasukkan music id untuk diupdate: "; cin >> id;

    int idx = find_music_index_by_id(id);
    if (idx == -1) {
        cout << "Music tidak ada.\n";
        return;
    }
    clear_input();

    string title;
    cout << "New Title (enter untuk skip): "; getline(cin, title);

    if (!title.empty()) musics[idx].title = title;

    string artist;
    cout << "New Artist (enter untuk skip): "; getline(cin, artist);

    if (!artist.empty()) musics[idx].artist = artist;

    cout << "New Duration (0 untuk skip): ";
    int dur;
    cin >> dur;

    if (dur > 0) musics[idx].duration_seconds = dur;
    cout << "Music updated.\n";
}

void delete_music() {
    list_all_music();
    int id;
    cout << "\nMasukkan music id untuk dihapus: "; cin >> id;
    int idx = find_music_index_by_id(id);
    if (idx == -1) {
        cout << "Music tidak ada.\n";
        return;
    }
    // remove references from playlists
    for (auto &pl: playlists) {
        pl.music_ids.erase(
            remove(pl.music_ids.begin(), pl.music_ids.end(), id),
            pl.music_ids.end()
        );
    }
    musics.erase(musics.begin() + idx);
    cout << "Music dihapus dan diremove dari semua playlist.\n";
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
    cout << "\nPlaylist: " << playlist.name << " (id:" << playlist.id << ") pemilik:" << playlist.owner_username << "\n";
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

    if (pl_index == -1) {
        cout << "Playlist Id not found.\n";
        return;
    }

    playlists.erase(playlists.begin() + pl_index);
    auto playlist_ids = playlists.at(user_index).music_ids;

    playlist_ids.erase(
        remove(playlist_ids.begin(), playlist_ids.end(), playlist_id),
        playlist_ids.end()
        );
}

void update_user_playlist(const ID_t playlist_id, const string &username) {
    int pl_index = find_playlist_index_by_id(playlist_id);

    if (playlists.at(pl_index).owner_username != username) {
        cout << "Can't update playlist\n";
        return;
    }

    string new_pl_name;
    cout << "New Name: "; getline(cin >> ws, new_pl_name);

    if (pl_index == -1) {
        cout << "Playlist Id not found.\n";
        return;
    }

    playlists.at(pl_index).name = new_pl_name;
    cout << "Success Update playlist" << endl;
}

void admin_add_playlist() {
    cout << "\n=== ADMIN ADD PLAYLIST ===\n";
    clear_input();
    string name, owner;
    cout << "Playlist name: "; getline(cin, name);
    cout << "Owner username: "; getline(cin, owner);
    if (find_user_index_by_username(owner) == -1) {
        cout << "Owner tidak ditemukan.\n";
        return;
    }
    playlists.push_back({generate_playlist_id(), name, owner, {}});

    int user_index = find_user_index_by_username(owner);
    users.at(user_index).playlist_ids.push_back(playlists.back().id);

    cout << "Playlist dibuat id=" << playlists.back().name << "\n";
}

void user_create_playlist(const string &username) {
    cout << "\n=== CREATE PLAYLIST ===\n";
    clear_input();

    string name;
    cout << "Playlist name: "; getline(cin, name);

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

void admin_update_playlist() {
    list_playlists_all_user();
    cout << "\nMasukkan playlist id untuk diupdate: ";
    int id;
    cin >> id;
    int pl_index = find_playlist_index_by_id(id);
    if (pl_index == -1) {
        cout << "Playlist tidak ada.\n";
        return;
    }
    clear_input();

    string name;
    cout << "New name (enter untuk skip): "; getline(cin, name);

    if (!name.empty()) playlists[pl_index].name = name;

    string owner;
    cout << "New owner (enter untuk skip): "; getline(cin, owner);

    if (!owner.empty()) {
        if (find_user_index_by_username(owner) == -1) {
            cout << "Owner baru tidak ditemukan. skip.\n";
        } else playlists[pl_index].owner_username = owner;
    }

    cout << "Playlist updated.\n";
}

void admin_delete_playlist() {
    list_playlists_all_user();
    ID_t id;
    cout << "\nMasukkan playlist id untuk dihapus: "; cin >> id;

    int pl_index = find_playlist_index_by_id(id);
    if (pl_index == -1) {
        cout << "Playlist tidak ada.\n";
        return;
    }

    playlists.erase(playlists.begin() + pl_index);
    cout << "Playlist dihapus.\n";
}

void add_music_to_playlist(const string &current_user, bool admin_mode) {
    list_all_music();
    list_playlists_all_user();
    cout << "\nMasukkan playlist id target: ";
    int input_pl_index;
    cin >> input_pl_index;
    int pl_index = find_playlist_index_by_id(input_pl_index);
    if (pl_index == -1) {
        cout << "Playlist tidak ada.\n";
        return;
    }

    if (!admin_mode && playlists[pl_index].owner_username != current_user) {
        cout << "Akses ditolak. Kamu hanya bisa menambahkan ke playlist milikmu.\n";
        return;
    }

    int mid;
    cout << "Masukkan music id yang mau ditambahkan: "; cin >> mid;

    int midx = find_music_index_by_id(mid);
    if (midx == -1) {
        cout << "Music tidak ada.\n";
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

void playlist_menu(const string &username) {
    while (true) {
        cout << "\n=== PLAYLIST MENU ===\n"
                << "1) Putar Playlist \n"
                << "2) Delete playlist\n"
                << "3) Edit playlists\n"
                << "4) Keluar\n";

        int option;
        cout << "menu: "; cin >> option;

        if (option == 4 || option == 0) return;

        int playlist_id;
        cout << "ID Playlist: "; cin >> playlist_id;

        int pl_index = find_playlist_index_by_id(playlist_id);

        clear_input();

        if (pl_index != -1) {
            if (option == 1) {
                play_playlist(playlists.at(pl_index));
            } else if (option == 2) {
                delete_user_playlist(playlist_id, username);
                list_playlists_all_user();
            } else if (option == 3) {
                update_user_playlist(playlist_id, username);
                list_playlists_all_user();
            } else {
                cout << "Pilihan tidak valid.\n";
            }
        } else {
            cout << "ID playlist tidak di temukan\n";
        }

    }
}

void admin_menu(const string &username) {
    while (true) {
        cout << "\n=== ADMIN MENU ===\n"
                << "1) Add music\n"
                << "2) Update music\n"
                << "3) Delete music\n"
                << "4) List music\n"
                << "5) Add playlist\n"
                << "6) Update playlist\n"
                << "7) Delete playlist\n"
                << "8) List playlists\n"
                << "9) Show playlist contents\n"
                << "10) Add music to playlist\n"
                << "0) Logout\n"
                << "Choose: ";
        int option;
        cin >> option;
        switch (option) {
            case 1: add_music();
                break;
            case 2: update_music();
                break;
            case 3: delete_music();
                break;
            case 4: list_all_music();
                break;
            case 5: admin_add_playlist();
                break;
            case 6: admin_update_playlist();
                break;
            case 7: admin_delete_playlist();
                break;
            case 8: list_playlists_all_user();
                break;
            case 9: {
                list_playlists_all_user();
                ID_t id;
                cout << "Masukkan playlist id: "; cin >> id;
                int pidx = find_playlist_index_by_id(id);
                if (pidx == -1) cout << "Playlist tidak ada.\n";
                else show_playlist_contents(playlists[pidx]);
                break;
            }
            case 10: add_music_to_playlist(username, true);
                break;
            case 0: return;
            default: cout << "Pilihan tidak valid.\n";
        }
    }
}

void user_menu(const string &username) {
    while (true) {
        cout << "\n=== USER MENU ===\n"
                << "1) Create playlist\n"
                << "2) List playlists\n"
                << "3) Show playlist contents\n"
                << "4) Add music to my playlist\n"
                << "5) List all music\n"
                << "6) Your Playlist\n"
                << "0) Logout\n"
                << "Choose: ";
        int option;
        cin >> option;
        switch (option) {
            case 1: user_create_playlist(username);
                break;
            case 2: {
                list_playlists_all_user();
                playlist_menu(username);
                break;
            }
            case 3: {
                list_playlists_all_user();
                ID_t id;
                cout << "Masukkan playlist id: "; cin >> id;
                int pidx = find_playlist_index_by_id(id);
                if (pidx == -1) cout << "Playlist tidak ada.\n";
                else {
                    show_playlist_contents(playlists[pidx]);
                }
                break;
            }
            case 4: add_music_to_playlist(username, false);
                break;
            case 5: list_all_music();
                break;
            case 0: return;
            default: cout << "Pilihan tidak valid.\n";
        }
    }
}

void seed_admin() {
    // create default admin if none exists
    if (find_user_index_by_username("admin") == -1) {
        users.push_back({generate_user_id(), "admin", "admin", "admin"});
    }

    users.push_back({
        .id = generate_music_id(),
        .username = "yusfi",
        .password = "123",
        .role = "user",
        .playlist_ids = {},
    });
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






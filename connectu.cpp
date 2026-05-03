/*
 * PROJECT: CONNECT-U (Starter Code)
 * Course: ECE367L Data Structures & Algorithms
 *
 * LAB 1: Timeline Linked List
 * - Implement a linked-list-based Timeline using Post nodes
 * - addPost(): insert new posts at the FRONT of the list in O(1)
 * - printTimeline(): traverse and print posts from newest to oldest
 *
 * SAFETY NOTE:
 * The saveData() function call is currently COMMENTED OUT in the main menu.
 * This prevents accidentally overwriting your CSV files while Lab 1 is incomplete.
 * ONLY uncomment saveData() after you verify:
 *   1) You can view existing posts
 *   2) You can create a new post
 *   3) The new post appears at the top of the timeline
 */

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <ctime>
#include <vector>
#include <set>
#include <unordered_set>
#include <queue>
#include <algorithm>
#include <limits>

using namespace std;

// ==========================================
// MODELS & DATA STRUCTURES
// ==========================================

struct Post {
    int postId;
    int userId;
    string content;
    int likes;
    long timestamp;
    Post* next;

    Post(int pid, int uid, string txt, int lk, long time)
        : postId(pid), userId(uid), content(txt), likes(lk), timestamp(time), next(nullptr) {}

    double getScore() {
        long currentTime = time(nullptr);
        double hoursOld = difftime(currentTime, timestamp) / 3600.0;
        return (likes * 10) + (1000.0 / (hoursOld + 1));
    }
};

class Timeline {
public:
    Post* head;
    Timeline() : head(nullptr) {}

    void addPost(int pid, int uid, string content, int likes, long time) {
        Post* newPost = new Post(pid, uid, content, likes, time);
        newPost->next = head;
        head = newPost;
    }

    void printTimeline() {
        Post* current = head;
        if (!current) {
            cout << "  (No posts yet)" << endl;
            return;
        }

        while (current != nullptr) {
            cout << "  > [ID: " << current->postId << "] "
                 << current->content
                 << " (" << current->likes << " likes)" << endl;
            current = current->next;
        }
    }
};

// Forward declaration
class User;

// ==========================================
// LAB 2 HASH MAP
// ==========================================

struct HashNode {
    string key;
    User* value;
    HashNode* next;
    HashNode(string k, User* v) : key(k), value(v), next(nullptr) {}
};

class UserMap {
private:
    static const int TABLE_SIZE = 10007;
    HashNode** table;

    unsigned long hashFunction(string key) {
        const unsigned long base = 31;
        unsigned long hash = 0;
        for (unsigned char c : key) {
            hash = (hash * base + c) % TABLE_SIZE;
        }
        return hash;
    }

public:
    UserMap() {
        table = new HashNode*[TABLE_SIZE];
        for (int i = 0; i < TABLE_SIZE; i++) table[i] = nullptr;
    }

    void put(string key, User* user) {
        unsigned long idx = hashFunction(key);

        if (table[idx] == nullptr) {
            table[idx] = new HashNode(key, user);
            return;
        }

        HashNode* curr = table[idx];
        while (curr != nullptr) {
            if (curr->key == key) {
                curr->value = user;
                return;
            }
            curr = curr->next;
        }

        HashNode* node = new HashNode(key, user);
        node->next = table[idx];
        table[idx] = node;
    }

    User* get(string key) {
        unsigned long idx = hashFunction(key);
        HashNode* curr = table[idx];

        while (curr != nullptr) {
            if (curr->key == key) return curr->value;
            curr = curr->next;
        }
        return nullptr;
    }
};

// ==========================================
// LAB 4 BST
// ==========================================

struct BSTNode {
    User* user;
    BSTNode* left;
    BSTNode* right;
    BSTNode(User* u) : user(u), left(nullptr), right(nullptr) {}
};

class FriendBST {
public:
    BSTNode* root;
    FriendBST() : root(nullptr) {}

    BSTNode* insert(BSTNode* node, User* u);
    void printInOrder(BSTNode* node);

    void addFriend(User* u) { root = insert(root, u); }

    void printFriends() {
        if (root == nullptr) cout << "  (No friends yet)" << endl;
        else printInOrder(root);
    }
};

class User {
public:
    int userId;
    string username;
    int techScore, artScore, sportScore;

    Timeline timeline;
    vector<User*> friends;
    FriendBST friendTree;

    User(int id, string name, int t, int a, int s)
        : userId(id), username(name), techScore(t), artScore(a), sportScore(s) {}

    void addPost(int pid, string content, int likes, long time) {
        timeline.addPost(pid, userId, content, likes, time);
    }

    void addFriend(User* u) {
        friends.push_back(u);
        friendTree.addFriend(u);
    }

    vector<User*> getFriendsList() { return friends; }
};

// BST implementation
BSTNode* FriendBST::insert(BSTNode* node, User* u) {
    if (node == nullptr) {
        return new BSTNode(u);
    }

    if (u->username < node->user->username) {
        node->left = insert(node->left, u);
    }
    else if (u->username > node->user->username) {
        node->right = insert(node->right, u);
    }

    return node;
}

void FriendBST::printInOrder(BSTNode* node) {
    if (node == nullptr) {
        return;
    }

    printInOrder(node->left);
    cout << "  - " << node->user->username << endl;
    printInOrder(node->right);
}

// ==========================================
// LAB 3 MAX HEAP
// ==========================================

class FeedHeap {
private:
    Post* heap[1000];
    int size;

    void heapifyDown(int index) {
        while (true) {
            int left = 2 * index + 1;
            int right = 2 * index + 2;
            int largest = index;

            if (left < size && heap[left]->getScore() > heap[largest]->getScore()) {
                largest = left;
            }

            if (right < size && heap[right]->getScore() > heap[largest]->getScore()) {
                largest = right;
            }

            if (largest == index) {
                break;
            }

            Post* temp = heap[index];
            heap[index] = heap[largest];
            heap[largest] = temp;

            index = largest;
        }
    }

    void heapifyUp(int index) {
        while (index > 0) {
            int parent = (index - 1) / 2;

            if (heap[index]->getScore() <= heap[parent]->getScore()) {
                break;
            }

            Post* temp = heap[index];
            heap[index] = heap[parent];
            heap[parent] = temp;

            index = parent;
        }
    }

public:
    FeedHeap() : size(0) {}

    void push(Post* p) {
        if (size >= 1000) return;

        heap[size] = p;
        heapifyUp(size);
        size++;
    }

    Post* popMax() {
        if (size == 0) return nullptr;

        Post* maxPost = heap[0];
        heap[0] = heap[size - 1];
        size--;

        if (size > 0) {
            heapifyDown(0);
        }

        return maxPost;
    }

    bool isEmpty() { return size == 0; }
};

// ==========================================
// GLOBAL DATA
// ==========================================

vector<User*> allUsers;
UserMap userMap;
int GLOBAL_POST_ID_COUNTER = 1;

// ==========================================
// UTILITY FUNCTIONS
// ==========================================

vector<string> split(string s) {
    vector<string> tokens;
    string token;
    bool inQuotes = false;

    for (char c : s) {
        if (c == '"') {
            inQuotes = !inQuotes;
            continue;
        }

        if (c == ',' && !inQuotes) {
            tokens.push_back(token);
            token.clear();
        }
        else {
            token += c;
        }
    }

    tokens.push_back(token);
    return tokens;
}

Post* findPostById(int id) {
    for (User* u : allUsers) {
        Post* curr = u->timeline.head;
        while (curr != nullptr) {
            if (curr->postId == id) return curr;
            curr = curr->next;
        }
    }
    return nullptr;
}

void createNewPost(User* author, string content) {
    int postId = GLOBAL_POST_ID_COUNTER++;
    long timestamp = time(0);
    author->addPost(postId, content, 0, timestamp);
    cout << "\n[SUCCESS] Post saved to timeline." << endl;
}

void registerNewUser(string username, int tech, int art, int sport) {
    int newId = (int)allUsers.size() + 1;
    User* newUser = new User(newId, username, tech, art, sport);
    allUsers.push_back(newUser);
    userMap.put(username, newUser);
    cout << "\n[SUCCESS] Account created." << endl;
}

void addFriendship(User* requester, User* target) {
    requester->addFriend(target);
    target->addFriend(requester);
    cout << "\n[SUCCESS] You are now friends with @" << target->username << endl;
}

void recommendFriends(User* startUser) {
    cout << "\n[GRAPH ANALYSIS] Finding friends of friends..." << endl;

    if (startUser == nullptr) {
        cout << "No user selected." << endl;
        return;
    }

    queue<User*> q;
    set<int> visited;
    set<int> directFriends;
    set<int> recommended;

    visited.insert(startUser->userId);

    for (User* f : startUser->friends) {
        if (f != nullptr) {
            q.push(f);
            visited.insert(f->userId);
            directFriends.insert(f->userId);
        }
    }

    bool found = false;

    while (!q.empty()) {
        User* current = q.front();
        q.pop();

        for (User* candidate : current->friends) {
            if (candidate == nullptr) continue;
            if (candidate->userId == startUser->userId) continue;
            if (directFriends.count(candidate->userId)) continue;
            if (recommended.count(candidate->userId)) continue;

            cout << "  - " << candidate->username << endl;
            recommended.insert(candidate->userId);
            visited.insert(candidate->userId);
            found = true;
        }
    }

    if (!found) {
        cout << "  No friend recommendations found." << endl;
    }
}

// ==========================================
// LAB 6: MUTUAL FRIENDS
// ==========================================

// Kadon: fill this function with the hash-based mutual-friends logic.

vector<User*> getMutualFriends(User* currentUser, User* targetUser) {
    vector<User*> mutuals;

    // =========================
    // KADON CODE STARTS HERE
    // =========================

    if (currentUser == nullptr || targetUser == nullptr) {
        return mutuals;
    }

    unordered_set<int> currentFriendIds;
    unordered_set<int> mutualFriendIds;

    for (User* f : currentUser->friends) {
        if (f != nullptr) {
            currentFriendIds.insert(f->userId);
        }
    }

    for (User* f : targetUser->friends) {
        if (f != nullptr && currentFriendIds.count(f->userId) > 0
            && mutualFriendIds.count(f->userId) == 0) {
            mutuals.push_back(f);
            mutualFriendIds.insert(f->userId);
        }
    }

    // =======================
    // KADON CODE ENDS HERE
    // =======================

    return mutuals;
}
// Shelby: sorting + printing layer
void displayMutualFriends(User* currentUser, User* targetUser) {
    vector<User*> mutuals = getMutualFriends(currentUser, targetUser);

    sort(mutuals.begin(), mutuals.end(), [](User* a, User* b) {
        return a->username < b->username;
    });

    if (mutuals.empty()) {
        cout << "No mutual friends found." << endl;
        return;
    }

    cout << "Mutual friends between @" << currentUser->username
         << " and @" << targetUser->username << ":" << endl;

    for (User* u : mutuals) {
        cout << "  - " << u->username << endl;
    }

    cout << "Total mutual friends: " << mutuals.size() << endl;
}

// ==========================================
// FILE I/O
// ==========================================

void loadData() {
    cout << "Loading data from CSV files..." << endl;

    ifstream userFile("users.csv");
    string line;

    if (userFile.is_open()) {
        getline(userFile, line);
        while (getline(userFile, line)) {
            vector<string> row = split(line);
            if (row.size() < 5) continue;

            User* newUser = new User(
                stoi(row[0]),
                row[1],
                stoi(row[2]),
                stoi(row[3]),
                stoi(row[4])
            );

            allUsers.push_back(newUser);
            userMap.put(row[1], newUser);
        }
        userFile.close();
    }

    ifstream relFile("relations.csv");
    if (relFile.is_open()) {
        getline(relFile, line);
        while (getline(relFile, line)) {
            vector<string> row = split(line);
            if (row.size() < 2) continue;

            int u1 = stoi(row[0]);
            int u2 = stoi(row[1]);

            if (u1 <= (int)allUsers.size() && u2 <= (int)allUsers.size()) {
                allUsers[u1 - 1]->addFriend(allUsers[u2 - 1]);
                allUsers[u2 - 1]->addFriend(allUsers[u1 - 1]);
            }
        }
        relFile.close();
    }

    ifstream postFile("posts.csv");
    if (postFile.is_open()) {
        getline(postFile, line);
        while (getline(postFile, line)) {
            vector<string> row = split(line);
            if (row.size() < 5) continue;

            int pid = stoi(row[0]);
            int uid = stoi(row[1]);

            if (uid <= (int)allUsers.size()) {
                allUsers[uid - 1]->addPost(pid, row[2], stoi(row[3]), stol(row[4]));
                if (pid >= GLOBAL_POST_ID_COUNTER) GLOBAL_POST_ID_COUNTER = pid + 1;
            }
        }
        postFile.close();
    }
}

void saveData() {
    if (allUsers.empty()) {
        cout << "[SAFETY] No data in memory. Skipping save to prevent file wipe." << endl;
        return;
    }

    cout << "Saving data..." << endl;

    ofstream userFile("users.csv");
    userFile << "user_id,username,tech_score,art_score,sport_score\n";
    for (User* u : allUsers) {
        userFile << u->userId << "," << u->username << ","
                 << u->techScore << "," << u->artScore << "," << u->sportScore << "\n";
    }
    userFile.close();

    ofstream relFile("relations.csv");
    relFile << "user_id_1,user_id_2\n";
    for (User* u : allUsers) {
        for (User* f : u->friends) {
            if (u->userId < f->userId) {
                relFile << u->userId << "," << f->userId << "\n";
            }
        }
    }
    relFile.close();

    ofstream postFile("posts.csv");
    postFile << "post_id,user_id,content,likes,timestamp\n";
    for (User* u : allUsers) {
        vector<Post*> temp;
        Post* curr = u->timeline.head;

        while (curr) {
            temp.push_back(curr);
            curr = curr->next;
        }

        for (int i = (int)temp.size() - 1; i >= 0; i--) {
            Post* p = temp[i];
            string safeContent = p->content;
            if (safeContent.find(',') != string::npos) {
                safeContent = "\"" + safeContent + "\"";
            }

            postFile << p->postId << "," << p->userId << ","
                     << safeContent << "," << p->likes << "," << p->timestamp << "\n";
        }
    }
    postFile.close();

    cout << "Done." << endl;
}

// ==========================================
// MAIN MENU (UI)
// ==========================================

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

int readInt() {
    int x;
    while (!(cin >> x)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Enter a number: ";
    }
    return x;
}

void showUserDashboard(User* currentUser) {
    int choice = 0;
    while (choice != 8) {
        cout << "\n--- Welcome, @" << currentUser->username << " ---" << endl;
        cout << "1. View My Post (Lab 1)" << endl;
        cout << "2. Create New Post (Lab 1)" << endl;
        cout << "3. Add Friend (Lab 2)" << endl;
        cout << "4. Algorithmic Feed (Lab 3)" << endl;
        cout << "5. View Friends Sorted (Lab 4)" << endl;
        cout << "6. Get Friend Recommendations (Lab 5)" << endl;
        cout << "7. Mutual Friends (Lab 6)" << endl;
        cout << "8. Logout" << endl;
        cout << "Select >> ";

        cin >> choice;

        if (choice == 1) {
            cout << "\n[MY POST]" << endl;
            currentUser->timeline.printTimeline();
        }
        else if (choice == 2) {
            cout << "\nEnter post content: ";
            cin.ignore();
            string content;
            getline(cin, content);
            createNewPost(currentUser, content);
        }
        else if (choice == 3) {
            string friendName;
            cout << "Enter username to add: ";
            cin >> friendName;

            User* target = userMap.get(friendName);
            if (target && target != currentUser) {
                addFriendship(currentUser, target);
            }
            else {
                cout << "Invalid user (or Hash Map not implemented)." << endl;
            }
        }
        else if (choice == 4) {
            cout << "\n[ALGORITHMIC FEED]" << endl;
            FeedHeap feed;
            vector<User*> friends = currentUser->getFriendsList();

            for (User* f : friends) {
                Post* p = f->timeline.head;
                int limit = 0;
                while (p != nullptr && limit < 5) {
                    feed.push(p);
                    p = p->next;
                    limit++;
                }
            }

            int count = 0;
            while (!feed.isEmpty() && count < 10) {
                Post* top = feed.popMax();
                if (top) {
                    cout << "  > [ID: " << top->postId << "] [Score: " << (int)top->getScore() << "] @"
                         << allUsers[top->userId - 1]->username << ": " << top->content
                         << " (" << top->likes << " likes)" << endl;
                }
                count++;
            }

            if (count == 0) {
                cout << "  No posts found." << endl;
            }
            else {
                cout << "\nDo you want to like a post? (y/n): ";
                char resp;
                cin >> resp;

                if (resp == 'y' || resp == 'Y') {
                    int pid;
                    cout << "Enter Post ID: ";
                    cin >> pid;
                    Post* p = findPostById(pid);
                    if (p) {
                        p->likes++;
                        cout << "Liked!" << endl;
                    }
                }
            }
        }
        else if (choice == 5) {
            cout << "\n[MY FRIENDS]" << endl;
            currentUser->friendTree.printFriends();
        }
        else if (choice == 6) {
            recommendFriends(currentUser);
        }
        else if (choice == 7) {
            cout << "\n[MUTUAL FRIENDS]" << endl;

            vector<User*> friends = currentUser->getFriendsList();

            if (friends.empty()) {
                cout << "You have no friends to compare with." << endl;
            }
            else {
                cout << "Choose a friend to compare with:" << endl;

                for (int i = 0; i < (int)friends.size(); i++) {
                    cout << "  " << (i + 1) << ". " << friends[i]->username << endl;
                }

                cout << "Select friend number >> ";
                int friendChoice = readInt();

                if (friendChoice < 1 || friendChoice > (int)friends.size()) {
                    cout << "Invalid friend number." << endl;
                }
                else {
                    User* targetUser = friends[friendChoice - 1];
                    displayMutualFriends(currentUser, targetUser);
                }
            }
        }
        else if (choice == 8) {
            cout << "Logging out..." << endl;
        }
        else {
            cout << "Invalid choice. Pick 1-8." << endl;
        }
    }
}

void showMainMenu() {
    int choice = 0;
    while (choice != 3) {
        cout << "\n=== CONNECT-U ===" << endl;
        cout << "1. Login" << endl;
        cout << "2. Register" << endl;
        cout << "3. Exit & Save" << endl;
        cout << "Select >> ";

        choice = readInt();

        if (choice == 1) {
            string username;
            cout << "Username: ";
            cin >> username;

            User* user = userMap.get(username);
            if (user) showUserDashboard(user);
            else cout << "User not found." << endl;
        }
        else if (choice == 2) {
            string username;
            int t, a, s;

            cout << "Username: ";
            cin >> username;

            cout << "Tech/Art/Sport (1-10): ";
            t = readInt();
            a = readInt();
            s = readInt();

            registerNewUser(username, t, a, s);
        }
        else if (choice == 3) {
            saveData();
            cout << "Goodbye! " << endl;
        }
        else {
            cout << "Invalid choice. Pick 1-3." << endl;
        }
    }
}

int main() {
    loadData();
    clearScreen();
    showMainMenu();
    return 0;
}


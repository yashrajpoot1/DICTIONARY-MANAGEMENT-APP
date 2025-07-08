#include <bits/stdc++.h>
using namespace std;

class DictionaryEntry {
public:
    string word;
    string meaning;
    vector<string> synonyms;
    vector<string> antonyms;

    DictionaryEntry(string w, string m, vector<string> syns = {}, vector<string> ants = {})
        : word(w), meaning(m), synonyms(syns), antonyms(ants) {}
};

class DictionaryTree {
private:
    struct Node {
        DictionaryEntry entry;
        Node* left;
        Node* right;

        Node(DictionaryEntry e) : entry(e), left(nullptr), right(nullptr) {}
    };

    Node* root;

    Node* insertRecursive(Node* node, DictionaryEntry entry) {
        if (node == nullptr) {
            return new Node(entry);
        }
        if (entry.word < node->entry.word) {
            node->left = insertRecursive(node->left, entry);
        } else if (entry.word > node->entry.word) {
            node->right = insertRecursive(node->right, entry);
        }
        return node;
    }

    DictionaryEntry* searchRecursive(Node* node, const string& word) const {
        if (node == nullptr || node->entry.word == word) {
            return node ? &node->entry : nullptr;
        }
        if (word < node->entry.word) {
            return searchRecursive(node->left, word);
        } else {
            return searchRecursive(node->right, word);
        }
    }

    Node* deleteRecursive(Node* node, const string& word) {
        if (node == nullptr) {
            return nullptr;
        }

        if (word < node->entry.word) {
            node->left = deleteRecursive(node->left, word);
        } else if (word > node->entry.word) {
            node->right = deleteRecursive(node->right, word);
        } else {
            if (node->left == nullptr) {
                Node* temp = node->right;
                delete node;
                return temp;
            } else if (node->right == nullptr) {
                Node* temp = node->left;
                delete node;
                return temp;
            }
            Node* successor = node->right;
            while (successor && successor->left) {
                successor = successor->left;
            }
            node->entry = successor->entry;
            node->right = deleteRecursive(node->right, successor->entry.word);
        }
        return node;
    }

    void inorderTraversal(Node* node) const {
        if (node == nullptr) return;
        inorderTraversal(node->left);
        cout << node->entry.word << " : " << node->entry.meaning << endl;
        inorderTraversal(node->right);
    }

    int countWordsRecursive(Node* node) const {
        if (node == nullptr) return 0;
        return 1 + countWordsRecursive(node->left) + countWordsRecursive(node->right);
    }

    void saveRecursive(Node* node, ofstream& file) {
        if (node == nullptr) return;
        saveRecursive(node->left, file);

        file << node->entry.word << "," << node->entry.meaning << ",";
        for (size_t i = 0; i < node->entry.synonyms.size(); ++i) {
            file << node->entry.synonyms[i];
            if (i != node->entry.synonyms.size() - 1) file << '|';
        }
        file << ",";
        for (size_t i = 0; i < node->entry.antonyms.size(); ++i) {
            file << node->entry.antonyms[i];
            if (i != node->entry.antonyms.size() - 1) file << '|';
        }
        file << endl;

        saveRecursive(node->right, file);
    }

    void findWordsStartingWithRecursive(Node* node, char c) const {
        if (node == nullptr) return;
        findWordsStartingWithRecursive(node->left, c);
        if (!node->entry.word.empty() && tolower(node->entry.word[0]) == c) {
            cout << node->entry.word << endl;
        }
        findWordsStartingWithRecursive(node->right, c);
    }

public:
    DictionaryTree() : root(nullptr) {}

    void insert(DictionaryEntry entry) {
        root = insertRecursive(root, entry);
    }

    DictionaryEntry* search(const string& word) const {
        return searchRecursive(root, word);
    }

    void deleteWord(const string& word) {
        root = deleteRecursive(root, word);
    }

    void insertFromFile(const string& filename) {
        ifstream file(filename);
        if (!file) {
            cerr << "Error opening file: " << filename << endl;
            return;
        }

        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            string word, meaning, synonymsStr, antonymsStr;
            vector<string> synonyms, antonyms;

            getline(ss, word, ',');
            getline(ss, meaning, ',');
            getline(ss, synonymsStr, ',');
            getline(ss, antonymsStr, ',');

            stringstream synStream(synonymsStr);
            stringstream antStream(antonymsStr);
            string item;

            while (getline(synStream, item, '|')) {
                synonyms.push_back(item);
            }
            while (getline(antStream, item, '|')) {
                antonyms.push_back(item);
            }

            insert(DictionaryEntry(word, meaning, synonyms, antonyms));
        }
    }

    void addEntryManually(const string& filename) {
        string word, meaning, synonymsStr, antonymsStr;
        vector<string> synonyms, antonyms;

        cout << "Enter word: ";
        cin.ignore();
        getline(cin, word);
        cout << "Enter meaning: ";
        getline(cin, meaning);
        cout << "Enter synonyms (separate by '|'): ";
        getline(cin, synonymsStr);
        cout << "Enter antonyms (separate by '|'): ";
        getline(cin, antonymsStr);

        stringstream synStream(synonymsStr);
        stringstream antStream(antonymsStr);
        string item;

        while (getline(synStream, item, '|')) {
            synonyms.push_back(item);
        }
        while (getline(antStream, item, '|')) {
            antonyms.push_back(item);
        }

        DictionaryEntry newEntry(word, meaning, synonyms, antonyms);
        insert(newEntry);

        ofstream file(filename, ios::app);
        if (!file) {
            cerr << "Error opening file for writing: " << filename << endl;
            return;
        }

        file << word << "," << meaning << ",";
        for (size_t i = 0; i < synonyms.size(); ++i) {
            file << synonyms[i];
            if (i != synonyms.size() - 1) file << '|';
        }
        file << ",";
        for (size_t i = 0; i < antonyms.size(); ++i) {
            file << antonyms[i];
            if (i != antonyms.size() - 1) file << '|';
        }
        file << endl;

        cout << "Entry added and saved to file successfully!" << endl;
    }

    void displayAllWords() const {
        cout << "Dictionary Entries (Alphabetical Order):\n";
        inorderTraversal(root);
    }

    void updateWord(const string& word) {
        DictionaryEntry* entry = search(word);
        if (!entry) {
            cout << "Word not found!" << endl;
            return;
        }

        cout << "Editing word: " << entry->word << endl;
        cout << "Current meaning: " << entry->meaning << endl;
        cout << "Enter new meaning (leave blank to keep same): ";
        cin.ignore();
        string newMeaning;
        getline(cin, newMeaning);
        if (!newMeaning.empty()) entry->meaning = newMeaning;

        cout << "Update synonyms (separate by '|', leave blank to keep same): ";
        string synonymsStr;
        getline(cin, synonymsStr);
        if (!synonymsStr.empty()) {
            entry->synonyms.clear();
            stringstream synStream(synonymsStr);
            string item;
            while (getline(synStream, item, '|')) {
                entry->synonyms.push_back(item);
            }
        }

        cout << "Update antonyms (separate by '|', leave blank to keep same): ";
        string antonymsStr;
        getline(cin, antonymsStr);
        if (!antonymsStr.empty()) {
            entry->antonyms.clear();
            stringstream antStream(antonymsStr);
            string item;
            while (getline(antStream, item, '|')) {
                entry->antonyms.push_back(item);
            }
        }

        cout << "Word updated successfully!" << endl;
    }

    void showSynonyms(const string& word) const {
        DictionaryEntry* entry = search(word);
        if (entry) {
            if (!entry->synonyms.empty()) {
                cout << "Synonyms of " << word << ": ";
                for (const auto& syn : entry->synonyms) cout << syn << " ";
                cout << endl;
            } else {
                cout << "No synonyms found." << endl;
            }
        } else {
            cout << "Word not found!" << endl;
        }
    }

    void showAntonyms(const string& word) const {
        DictionaryEntry* entry = search(word);
        if (entry) {
            if (!entry->antonyms.empty()) {
                cout << "Antonyms of " << word << ": ";
                for (const auto& ant : entry->antonyms) cout << ant << " ";
                cout << endl;
            } else {
                cout << "No antonyms found." << endl;
            }
        } else {
            cout << "Word not found!" << endl;
        }
    }

    int countWords() const {
        return countWordsRecursive(root);
    }

    void saveToFile(const string& filename) {
        ofstream file(filename);
        if (!file) {
            cerr << "Error opening file for writing: " << filename << endl;
            return;
        }
        saveRecursive(root, file);
        cout << "Dictionary saved successfully to " << filename << "!" << endl;
    }

    void findWordsStartingWith(char c) const {
        cout << "Words starting with '" << c << "':\n";
        findWordsStartingWithRecursive(root, tolower(c));
    }
};

int main() {
    DictionaryTree dictionary;

    dictionary.insertFromFile("dictionary.txt");

    int choice;
    do {
        cout << "\n--- Dictionary Management ---\n";
        cout << "1. Add new entry\n";
        cout << "2. Search a word\n";
        cout << "3. Delete a word\n";
        cout << "4. Update a word\n";
        cout << "5. Display all words\n";
        cout << "6. Show synonyms\n";
        cout << "7. Show antonyms\n";
        cout << "8. Find words starting with a letter\n";
        cout << "9. Count total words\n";
        cout << "10. Save dictionary to file\n";
        cout << "0. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        string word;
        char letter;
        switch (choice) {
            case 1:
                dictionary.addEntryManually("dictionary.txt");
                break;
            case 2:
                cout << "Enter word to search: ";
                cin >> word;
                if (dictionary.search(word)) {
                    cout << "Word found!" << endl;
                } else {
                    cout << "Word not found!" << endl;
                }
                break;
            case 3:
                cout << "Enter word to delete: ";
                cin >> word;
                dictionary.deleteWord(word);
                cout << "Word deleted successfully!" << endl;
                break;
            case 4:
                cout << "Enter word to update: ";
                cin >> word;
                dictionary.updateWord(word);
                break;
            case 5:
                dictionary.displayAllWords();
                break;
            case 6:
                cout << "Enter word to view synonyms: ";
                cin >> word;
                dictionary.showSynonyms(word);
                break;
            case 7:
                cout << "Enter word to view antonyms: ";
                cin >> word;
                dictionary.showAntonyms(word);
                break;
            case 8:
                cout << "Enter starting letter: ";
                cin >> letter;
                dictionary.findWordsStartingWith(letter);
                break;
            case 9:
                cout << "Total number of words: " << dictionary.countWords() << endl;
                break;
            case 10:
                dictionary.saveToFile("dictionary.txt");
                break;
            case 0:
                cout << "Exiting program. Goodbye!" << endl;
                break;
            default:
                cout << "Invalid choice! Try again." << endl;
        }

    } while (choice != 0);

    return 0;
}

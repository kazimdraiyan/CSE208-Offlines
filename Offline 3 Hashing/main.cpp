#include <bits/stdc++.h>
#include "chaining_hash_table.cpp"
#include "open_addressing_hash_table.cpp"

const int WORD_LENGTH = 10;
const int WORD_COUNT = 10000;
const int SERACH_COUNT = 1000;

using namespace std;

struct HashFunctions
{
    // djb2
    static unsigned long hash1(const string &key)
    {
        unsigned long hash = 5381; // seed
        for (int i = 0; key[i]; i++)
        {
            hash = ((hash << 5) + hash) + key[i]; // hash * 33 + char
        }
        return hash;
    }

    // FNV-1a
    static unsigned long hash2(const string &key)
    {
        unsigned long long hash = 14695981039346656037ULL; // FNV offset basis
        for (int i = 0; key[i]; i++)
        {
            hash ^= (unsigned char)key[i]; // XOR in the byte first
            hash *= 1099511628211ULL;      // multiply by FNV prime
        }
        return hash;
    }
};

class WordGenerator
{
    int length;
    unordered_set<string> seen;

public:
    WordGenerator(int len) : length(len) {}

    string next()
    {
        string s;
        do
        {
            s = "";
            for (int i = 0; i < length; i++)
            {
                s += rand() % 26 + 'a';
            }
        } while (seen.find(s) != seen.end()); // while s exists in seen
        seen.insert(s);
        return s;
    }
};

double computeAverageHits(HashTable<string, int> &table, vector<string> &words)
{
    long long totalHits = 0;
    int dummyValue;

    for (int i = 0; i < SERACH_COUNT; i++)
    {
        int randomIndex = rand() % words.size();
        int hits = 0;
        table.search(words[randomIndex], dummyValue, hits);
        totalHits += hits;
    }

    return totalHits / (double)SERACH_COUNT;
};

int main()
{
    WordGenerator wordGenerator(WORD_LENGTH);

    vector<string> words;
    for (int i = 0; i < WORD_COUNT; i++)
    {
        words.push_back(wordGenerator.next());
    }

    ChainingHashTable<string, int> chainHash1(HashFunctions::hash1);
    ChainingHashTable<string, int> chainHash2(HashFunctions::hash2);
    DoubleHashTable<string, int> doubleHash1(HashFunctions::hash1);
    DoubleHashTable<string, int> doubleHash2(HashFunctions::hash2);
    CustomProbeHashTable<string, int> customHash1(HashFunctions::hash1);
    CustomProbeHashTable<string, int> customHash2(HashFunctions::hash2);

    for (int i = 0; i < (int)words.size(); i++)
    {
        chainHash1.insert(words[i], i + 1);
        chainHash2.insert(words[i], i + 1);
        doubleHash1.insert(words[i], i + 1);
        doubleHash2.insert(words[i], i + 1);
        customHash1.insert(words[i], i + 1);
        customHash2.insert(words[i], i + 1);
    }

    double avgHitsChain1 = computeAverageHits(chainHash1, words);
    double avgHitsChain2 = computeAverageHits(chainHash2, words);
    double avgHitsDouble1 = computeAverageHits(doubleHash1, words);
    double avgHitsDouble2 = computeAverageHits(doubleHash2, words);
    double avgHitsCustom1 = computeAverageHits(customHash1, words);
    double avgHitsCustom2 = computeAverageHits(customHash2, words);

    cout << "Total words: " << WORD_COUNT << endl;
    cout << "Total searched: " << SERACH_COUNT << endl;

    cout << fixed << setprecision(3);
    cout << left << setw(16) << "Technique"
         << setw(20) << "Hash1 Collisions" << setw(16) << "Hash1 Avg Hits"
         << setw(20) << "Hash2 Collisions" << setw(16) << "Hash2 Avg Hits" << "\n";

    cout << left << setw(16) << "Chaining"
         << setw(20) << chainHash1.getCollisionCount() << setw(16) << avgHitsChain1
         << setw(20) << chainHash2.getCollisionCount() << setw(16) << avgHitsChain2 << "\n";

    cout << left << setw(16) << "Double Hashing"
         << setw(20) << doubleHash1.getCollisionCount() << setw(16) << avgHitsDouble1
         << setw(20) << doubleHash2.getCollisionCount() << setw(16) << avgHitsDouble2 << "\n";

    cout << left << setw(16) << "Custom Probing"
         << setw(20) << customHash1.getCollisionCount() << setw(16) << avgHitsCustom1
         << setw(20) << customHash2.getCollisionCount() << setw(16) << avgHitsCustom2 << "\n";

    return 0;
}
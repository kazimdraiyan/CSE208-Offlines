#include <bits/stdc++.h>
#include "hash_table.cpp"
#include "utils.cpp"

using namespace std;

template <typename K, typename V>
struct Node
{
    K key;
    V value;
    Node *next;
    Node(K k, V v) : key(k), value(v), next(nullptr) {}
};

template <typename K, typename V>
class ChainingHashTable : public HashTable<K, V>
{
    vector<Node<K, V> *> table;
    int tableSize;
    int count;
    int insertsSinceResize;
    int deletesSinceResize;
    int collisionCount;
    function<unsigned long(const K &)> hashFn;

    void resizeIfNeeded()
    {
        double loadFactor = (double)count / tableSize;

        if (loadFactor > MAX_LOAD_FACTOR && insertsSinceResize >= count / 2)
            tableSize = nextPrimeAbove(2 * tableSize);
        else if (loadFactor < MIN_LOAD_FACTOR && deletesSinceResize >= count / 2)
        {
            int shrinkedSize = prevPrimeBelow(tableSize / 2);
            if (shrinkedSize < INITIAL_SIZE)
                return; // skip shrinking
            tableSize = shrinkedSize;
        }
        else
            return; // resizing not needed

        vector<Node<K, V> *> oldTable = table;
        table.assign(tableSize, nullptr);

        // Rehash everything
        for (Node<K, V> *currentNode : oldTable)
        {
            while (currentNode != nullptr)
            {
                Node<K, V> *next = currentNode->next;

                unsigned long index = hashFn(currentNode->key) % tableSize;
                currentNode->next = table[index];
                table[index] = currentNode;

                currentNode = next;
            }
        }

        insertsSinceResize = 0;
        deletesSinceResize = 0;
    }

public:
    ChainingHashTable(function<unsigned long(const K &)> hf)
        : tableSize(INITIAL_SIZE), count(0), insertsSinceResize(0),
          deletesSinceResize(0), collisionCount(0), hashFn(hf)
    {
        table.assign(tableSize, nullptr);
    }

    ~ChainingHashTable() override
    {
        for (Node<K, V> *current : table)
        {
            while (current != nullptr)
            {
                Node<K, V> *next = current->next;
                delete current;
                current = next;
            }
        }
    }

    bool insert(const K &key, const V &value) override
    {
        int dummyValue;
        int dummyHits = 0;
        if (search(key, dummyValue, dummyHits))
        {
            return false; // duplicate
        }

        unsigned long index = hashFn(key) % tableSize;

        if (table[index] != nullptr)
            collisionCount++;

        Node<K, V> *node = new Node(key, value);
        node->next = table[index];
        table[index] = node;

        count++;
        insertsSinceResize++;

        resizeIfNeeded();

        return true;
    }

    bool search(const K &key, V &outValue, int &hits) override
    {
        unsigned long index = hashFn(key) % tableSize;
        Node<K, V> *currentNode = table[index];
        while (currentNode != nullptr)
        {
            hits++;
            if (currentNode->key == key)
            {
                outValue = currentNode->value;
                return true;
            }
            currentNode = currentNode->next;
        }
        return false;
    }

    bool remove(const K &key) override
    {
        unsigned long index = hashFn(key) % tableSize;
        Node<K, V> *current = table[index];
        Node<K, V> *parent = nullptr;

        while (current != nullptr)
        {
            if (current->key == key)
            {
                if (parent == nullptr)
                    table[index] = current->next; // removing head
                else
                    parent->next = current->next; // removing mid-chain / tail

                delete current;

                count--;
                deletesSinceResize++;
                resizeIfNeeded();
                return true;
            }
            parent = current;
            current = current->next;
        }

        return false; // not found
    }

    int getCollisionCount() const override {
        return collisionCount;
    }

    int size() const override {
        return count;
    }

    void print() const override
    {
        for (int i = 0; i < tableSize; i++)
        {
            cout << i << ": ";
            for (Node<K, V> *cur = table[i]; cur; cur = cur->next)
                cout << "(" << cur->key << "," << cur->value << ") ";
            cout << "\n";
        }
    }
};

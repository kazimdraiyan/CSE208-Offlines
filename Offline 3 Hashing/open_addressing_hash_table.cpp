#include <bits/stdc++.h>
#include "hash_table.cpp"
#include "utils.cpp"

using namespace std;

template <typename K, typename V>
struct Slot
{
    K key;
    V value;
    bool occupied = false;
    bool deleted = false; // tombstone, needed for open addressing deletes
};

template <typename K, typename V>
class OpenAddressingHashTable : public HashTable<K, V>
{
protected:
    vector<Slot<K, V>> table;
    int tableSize;
    int count;
    int insertsSinceResize;
    int deletesSinceResize;
    int collisionCount;
    function<unsigned long(const K &)> hashFn;

    // Derived classes define their own probe sequence
    virtual int probe(const K &key, int i, int size) const = 0;

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

        vector<Slot<K, V>> oldTable = table;
        table.assign(tableSize, Slot<K, V>{});

        // Rehash everything
        count = 0;
        for (Slot<K, V> &currentSlot : oldTable)
        {
            if (currentSlot.occupied && !currentSlot.deleted)
            {
                insert_helper(currentSlot.key, currentSlot.value, true);
            }
        }

        insertsSinceResize = 0;
        deletesSinceResize = 0;
    }

    bool insert_helper(const K &key, const V &value, bool isRehashInsert = false)
    {
        int dummyValue;
        int dummyHits = 0;
        if (!isRehashInsert && search(key, dummyValue, dummyHits))
        { // Rehash insert cannot be duplicate
            return false; // duplicate
        }

        // Probe until empty/tombstone slot or duplicate found,
        int i = 0;
        while (true)
        {
            int index = probe(key, i, tableSize);

            // Count collision for initial probe only
            if (i == 0 && table[index].occupied && !table[index].deleted && !isRehashInsert)
                collisionCount++;

            if (!table[index].occupied)
            {
                table[index].key = key;
                table[index].value = value;
                table[index].occupied = true;
                table[index].deleted = false;
                count++;
                insertsSinceResize++;
                if (!isRehashInsert)
                    resizeIfNeeded();
                return true;
            }
            i++;
        }
    }

public:
    OpenAddressingHashTable(function<unsigned long(const K &)> hf)
        : tableSize(INITIAL_SIZE), count(0), insertsSinceResize(0),
          deletesSinceResize(0), collisionCount(0), hashFn(hf)
    {
        table.assign(tableSize, Slot<K, V>{});
    }

    bool insert(const K &key, const V &value) override {
        return insert_helper(key, value, false);
    }

    bool search(const K &key, V &outValue, int &hits) override
    {
        // Probe until found or empty (non-deleted) slot hit
        int i = 0;
        while (true)
        {
            int index = probe(key, i, tableSize);
            hits++;
            i++;

            if (table[index].deleted)
                continue; // Tombstone
            if (!table[index].occupied)
                break;

            if (table[index].key == key)
            {
                outValue = table[index].value;
                return true;
            }
        }
        return false;
    }

    bool remove(const K &key) override
    {
        int i = 0;
        while (true)
        {
            int index = probe(key, i, tableSize);
            i++;

            if (table[index].deleted)
                continue; // Tombstone
            if (!table[index].occupied)
                break;

            if (table[index].key == key)
            {
                table[index].occupied = false;
                table[index].deleted = true; // Tombstoning
                count--;
                deletesSinceResize++;
                resizeIfNeeded();
                return true;
            }
        }
        return false;
    }

    int getCollisionCount() const override
    {
        return collisionCount;
    }

    int size() const override
    {
        return count;
    }

    void print() const override
    {
        for (int i = 0; i < tableSize; i++)
        {
            if (table[i].occupied && !table[i].deleted)
                cout << i << ": (" << table[i].key << "," << table[i].value << ")\n";
        }
    }
};

template <typename K, typename V>
class DoubleHashTable : public OpenAddressingHashTable<K, V>
{
    using super = OpenAddressingHashTable<K, V>;

    int probe(const K &key, int i, int size) const override
    {
        unsigned long primaryHash = super::hashFn(key);
        return (primaryHash + i * auxHash(primaryHash, size)) % size;
    }

public:
    DoubleHashTable(function<unsigned long(const K &)> hf)
        : OpenAddressingHashTable<K, V>(hf) {}
};

template <typename K, typename V>
class CustomProbeHashTable : public OpenAddressingHashTable<K, V>
{
    using super = OpenAddressingHashTable<K, V>;

    int probe(const K &key, int i, int size) const override
    {
        unsigned long primaryHash = super::hashFn(key);
        return (primaryHash + C1 * i * auxHash(primaryHash, size) + C2 * i * i) % size;
    }

public:
    CustomProbeHashTable(function<unsigned long(const K &)> hf)
        : OpenAddressingHashTable<K, V>(hf) {}
};

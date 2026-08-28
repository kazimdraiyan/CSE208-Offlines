#pragma once

template <typename K, typename V>
class HashTable
{
public:
    virtual ~HashTable() {}

    virtual bool insert(const K &key, const V &value) = 0;
    virtual bool search(const K &key, V &outValue, int &hits) = 0;
    virtual bool remove(const K &key) = 0;

    virtual int getCollisionCount() const = 0;
    virtual int size() const = 0;

    virtual void print() const = 0;
};
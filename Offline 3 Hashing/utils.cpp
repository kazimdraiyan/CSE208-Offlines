#pragma once

const int INITIAL_SIZE = 13;
const double MAX_LOAD_FACTOR = 0.5;
const double MIN_LOAD_FACTOR = 0.25;

// Custom probing constants
const int C1 = 1;
const int C2 = 3;

bool isPrime(int n)
{
    if (n <= 1)
        return false;

    for (int i = 2; i * i <= n; i++)
    {
        if (n % i == 0)
            return false;
    }
    return true;
}

int nextPrimeAbove(int n)
{
    while (true)
    {
        n++;
        if (isPrime(n))
            return n;
    }
}

int prevPrimeBelow(int n)
{
    while (true)
    {
        n--;
        if (isPrime(n))
            return n;
    }
}

unsigned long auxHash(unsigned long primaryHash, int tableSize)
{
    return 1 + (primaryHash % (tableSize - 1));
}

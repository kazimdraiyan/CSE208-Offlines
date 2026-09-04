#include <bits/stdc++.h>

using namespace std;

class Node
{
public:
    int key;
    int degree;
    Node *parent;
    Node *child; // leftmost child
    Node *next;  // next sibling

    Node(int k) : key(k), degree(0), parent(nullptr), child(nullptr), next(nullptr) {}
};

class BinomialHeap
{
private:
    Node *head;                        // head of root list, sorted by ascending degree
    unordered_map<int, Node *> locate; // key -> node

    // Merge two root lists sorted by ascending degree.
    // Assume the two root lists are sorted.
    // The merge is stable.
    // Corrupts the root list headY.
    Node *mergeRootLists(Node *headX, Node *headY)
    {
        Node *headMerged = nullptr;

        Node *currentX = headX;
        Node *currentY = headY;
        Node *currentMerged = headMerged;

        while (currentX && currentY)
        {
            if (currentX->degree <= currentY->degree)
            {
                if (!currentMerged)
                {
                    currentMerged = currentX;
                    headMerged = currentMerged;
                }
                else
                {
                    currentMerged->next = currentX;
                    currentMerged = currentMerged->next;
                }
                currentX = currentX->next;
            }
            else
            {
                if (!currentMerged)
                {
                    currentMerged = currentY;
                    headMerged = currentMerged;
                }
                else
                {
                    currentMerged->next = currentY;
                    currentMerged = currentMerged->next;
                }
                currentY = currentY->next;
            }
        }
        if (currentX)
        {
            if (!currentMerged)
                headMerged = currentX;
            else
                currentMerged->next = currentX;
        }
        else if (currentY)
        {
            if (!currentMerged)
                headMerged = currentY;
            else
                currentMerged->next = currentY;
        }
        return headMerged;
    }

    // Make y, x's first child. Assume y has no parent.
    void linkTrees(Node *x, Node *y)
    {
        if (!x || !y)
            throw invalid_argument("Cannot link null pointers.");

        y->next = x->child;
        x->child = y;
        y->parent = x;
        x->degree++;
    }

    void consolidate()
    {
        Node *previous = nullptr;
        Node *current = head;
        while (current && current->next)
        {
            Node *next = current->next;
            Node *secondNext = next->next;

            bool threeConsecutiveSameDegree = secondNext && current->degree == secondNext->degree;

            if (!threeConsecutiveSameDegree && current->degree == next->degree)
            {
                if (current->key <= next->key)
                {
                    linkTrees(current, next);
                    current->next = secondNext; // Delete next from root list
                }
                else
                {
                    linkTrees(next, current);
                    if (previous)
                        previous->next = next; // Delete current from root list
                    else
                        head = next;
                    current = next;
                }
            }
            else
            {
                previous = current;
                current = next;
            }
        }
    }

    // Node *findMinNode() const
    // {
    //     if (!head)
    //         return nullptr;

    //     Node *current = head;
    //     Node *minNode = current;
    //     int minKey = current->key;
    //     while (current->next)
    //     {
    //         if (current->next->key < minKey)
    //         {
    //             minKey = current->next->key;
    //             minNode = current->next;
    //         }
    //         current = current->next;
    //     }
    //     return minNode;
    // }

    // Nodes are not swapped. Only keys are swapped.
    void bubbleUp(Node *node)
    {
        if (!node)
            return;

        while (node->parent && node->key < node->parent->key)
        {
            swap(locate[node->key], locate[node->parent->key]);
            swap(node->key, node->parent->key);
            node = node->parent;
        }
    }

    // Reverse the children list and return the new head
    Node *reverseAndClearParent(Node *listHead)
    {
        if (!listHead)
            return nullptr;

        if (!listHead->next)
        {
            listHead->parent = nullptr;
            return listHead;
        }

        listHead->parent = nullptr;

        Node *newHead = reverseAndClearParent(listHead->next);
        listHead->next->next = listHead;
        listHead->next = nullptr;
        return newHead;
    }

    // Recursively delete a node and its subtrees
    void deleteTree(Node *node)
    {
        if (!node)
            return;

        Node *child = node->child;
        while (child)
        {
            Node *nextChild = child->next;
            deleteTree(child);
            child = nextChild;
        }

        delete node;
    }

    // * Part B helper methods
    // Example tree block format for B3:
    // 5 --- 55
    //   --- 15 --- 50
    //   --- 10 --- 40
    //          --- 20 --- 70
    vector<string> treeBlock(Node *node) const
    {
        string keyStr = to_string(node->key);

        if (!node->child)
            return {keyStr};

        // Collect children left to right
        vector<Node *> children;
        for (Node *c = node->child; c; c = c->next)
            children.push_back(c);
        reverse(children.begin(), children.end()); // Sort children in ascending degree

        vector<string> result;

        // First child on the same line as node
        vector<string> firstChildBlock = treeBlock(children[0]);
        result.push_back(keyStr + " --- " + firstChildBlock[0]);

        // Indent remaining children of the first child
        string firstChildIndent(keyStr.size() + 5, ' '); // "Key --- "
        for (int i = 1; i < firstChildBlock.size(); i++)
            result.push_back(firstChildIndent + firstChildBlock[i]);

        // Indent siblings of the first child
        string siblingIndent(keyStr.size() + 1, ' ');
        for (int i = 1; i < children.size(); i++)
        {
            vector<string> childBlock = treeBlock(children[i]);
            result.push_back(siblingIndent + "--- " + childBlock[0]);

            string nestedIndent = siblingIndent + string(4, ' '); // "--- "
            for (int j = 1; j < childBlock.size(); j++)
                result.push_back(nestedIndent + childBlock[j]);
        }

        return result;
    }

    void drawTree(Node *root, stringstream &sout) const
    {
        vector<string> treeBlockLines = treeBlock(root);
        for (int i = 0; i < treeBlockLines.size(); i++)
        {
            sout << treeBlockLines[i];
            if (i < treeBlockLines.size() - 1)
                sout << endl;
        }
    }

public:
    BinomialHeap() : head(nullptr) {}

    ~BinomialHeap()
    {
        Node *current = head;
        while (current)
        {
            Node *next = current->next;
            deleteTree(current);
            current = next;
        }
        head = nullptr;
        locate.clear();
    }

    // Create single node heap, union with this heap.
    void insert(int key)
    {
        if (locate.count(key))
            throw invalid_argument("Key " + to_string(key) + " already exists.");

        if (!head)
        {
            head = new Node(key);
            locate[key] = head;
            return;
        }

        BinomialHeap newHeap;
        newHeap.insert(key);
        unionWith(newHeap);
    }

    int findMin() const
    {
        Node *current = head;
        if (!current)
            throw underflow_error("Heap is empty.");

        int minKey = current->key;
        while (current->next)
        {
            minKey = min(minKey, current->next->key);
            current = current->next;
        }
        return minKey;
    }

    int extractMin()
    {
        Node *current = head;
        if (!current)
            throw underflow_error("Heap is empty.");

        // Find min node
        Node *minNode = current;
        Node *minNodePrev = nullptr;
        int minKey = current->key;
        while (current->next)
        {
            if (current->next->key < minKey)
            {
                minKey = current->next->key;
                minNode = current->next;
                minNodePrev = current;
            }
            current = current->next;
        }

        Node *sortedChildrenHead = reverseAndClearParent(minNode->child); // Rverese the children list into ascending degree

        // Remove minNode from head
        if (!minNodePrev)
            head = minNode->next; // minNode was head
        else
            minNodePrev->next = minNode->next;

        locate.erase(minKey);
        delete minNode;

        // Add min node's children to the root list
        head = mergeRootLists(head, sortedChildrenHead);
        consolidate();

        return minKey;
    }

    // Assume keys are unique accross both heaps
    void unionWith(BinomialHeap &other)
    {
        // Merge other.locate into this
        for (const pair<int, Node *> &entry : other.locate)
        {
            locate[entry.first] = entry.second;
        }

        head = mergeRootLists(head, other.head); // Corrupts other's root list
        consolidate();

        // Clear other heap
        other.head = nullptr;
        other.locate.clear();
    };

    void decreaseKey(int currentKey, int decreasedKey)
    {
        Node *node = locate[currentKey];
        node->key = decreasedKey;
        locate[decreasedKey] = node;
        locate.erase(currentKey);
        bubbleUp(node);
    }

    void removeKey(int key)
    {
        decreaseKey(key, INT_MIN);
        extractMin();
    }

    void print(int heapId, stringstream &sout) const
    {
        sout << "Heap size: " << size() << endl;

        if (!head)
        {
            sout << "Heap H" << heapId << " is empty.";
            return;
        }

        Node *currentRoot = head;
        // Traverse roots
        while (currentRoot)
        {
            if (currentRoot != head)
                sout << endl;

            sout << "Binomial Tree, B" << currentRoot->degree << endl;
            sout << "Level 0: " << currentRoot->key;

            vector<Node *> thisLevelFirstChildren;
            thisLevelFirstChildren.push_back(currentRoot->child);

            // Traverse each level of the root
            for (int level = 1; level <= currentRoot->degree; level++)
            {
                sout << endl
                     << "Level " << level << ": ";

                vector<int> keysCurrentLevel;
                vector<Node *> nextLevelFirstChildren;
                // To traverse the siblings of each first child of the level
                for (Node *currentFirstChild : thisLevelFirstChildren)
                {
                    Node *currentChild = currentFirstChild;
                    // Traverse siblings of the current first child
                    while (currentChild)
                    {
                        keysCurrentLevel.push_back(currentChild->key);
                        nextLevelFirstChildren.push_back(currentChild->child);
                        currentChild = currentChild->next;
                    }
                }

                // Print the sorted keys in the current level
                sort(keysCurrentLevel.begin(), keysCurrentLevel.end());
                for (int i = 0; i < keysCurrentLevel.size(); i++)
                {
                    int key = keysCurrentLevel[i];
                    sout << key;
                    if (i < keysCurrentLevel.size() - 1)
                        sout << " ";
                }

                // Move to next level
                thisLevelFirstChildren = nextLevelFirstChildren;
            }
            currentRoot = currentRoot->next;
        }
    }

    // TODO: Takes O(logn), should I keep and maintain a size field instead?
    int size() const
    {
        int totalSize = 0;
        Node *current = head;
        while (current)
        {
            totalSize += pow(2, current->degree);
            current = current->next;
        }
        return totalSize;
    }

    // * Part B methods:
    // Feature 1:
    void visualizeHeap(int heapId, stringstream &sout) const
    {
        sout << "Visualizing Binomial Heap H" << heapId << endl;

        sout << "Heap size: " << size() << endl;

        if (!head)
        {
            sout << "Heap H" << heapId << " is empty.";
            return;
        }

        Node *currentRoot = head;
        while (currentRoot)
        {
            sout << "Binomial Tree B" << currentRoot->degree << ":" << endl;

            drawTree(currentRoot, sout);

            currentRoot = currentRoot->next;
            if (currentRoot)
                sout << endl;
        }
    }

    // Feature 3:
    void visualizeBinaryRepresentation(int heapId, stringstream &sout) const
    {
        int n = size();
        sout << "Binary notation of H" << heapId << ":";
        sout << " n = " << n;
        sout << " (";

        // Print n in binary
        if (n == 0)
            sout << "0";
        else
        {
            // 32 bits is plenty since n <= 10^5 per the handout's constraints.
            string bits = bitset<32>(n).to_string();
            size_t firstOne = bits.find('1');
            sout << bits.substr(firstOne);
        }
        sout << ")" << endl;

        // Traverse root list
        vector<int> degrees;
        Node *current = head;
        while (current)
        {
            degrees.push_back(current->degree);
            current = current->next;
        }

        // Print the degrees of the binomial trees
        sout << "Binomial trees in H2: ";
        if (degrees.empty())
        {
            sout << "None";
        }
        for (int i = degrees.size() - 1; i >= 0; i--)
        {
            sout << "B" << degrees[i];
            if (i > 0)
                sout << " ";
        }
    }
};

class IOManager
{
private:
    BinomialHeap h1, h2;
    ofstream outFile;

    // id = 1 or 2
    BinomialHeap &heapById(int id)
    {
        return (id == 1) ? h1 : h2;
    }

    // Write one line to both console and output.txt
    void output(const string &line)
    {
        cout << line << endl;
        outFile << line << endl;
    }

public:
    IOManager(const string &outPath)
    {
        outFile.open(outPath);
    }

    ~IOManager()
    {
        if (outFile.is_open())
            outFile.close();
    }

    // Commands: I h x, F h, E h, D h x y, R h x, U h1 h2, P h
    // Only F, E, P produce output
    void run(const string &inPath)
    {
        ifstream inFile(inPath);
        if (!inFile.is_open())
        {
            cout << "Input file not found" << endl;
            return;
        }

        string line;
        while (getline(inFile, line))
        {
            if (line.empty())
                continue; // Skip blank line

            stringstream ss(line);
            string cmd;
            ss >> cmd;

            if (cmd == "I")
            {
                int h, x;
                ss >> h >> x;
                heapById(h).insert(x);
            }
            else if (cmd == "F")
            {
                int h;
                ss >> h;
                int minKey = heapById(h).findMin();
                output("Find Min returned: " + to_string(minKey));
            }
            else if (cmd == "E")
            {
                int h;
                ss >> h;
                int minKey = heapById(h).extractMin();
                output("Extract Min returned: " + to_string(minKey));
            }
            else if (cmd == "D")
            {
                int h, x, y;
                ss >> h >> x >> y;
                heapById(h).decreaseKey(x, y);
            }
            else if (cmd == "R")
            {
                int h, x;
                ss >> h >> x;
                heapById(h).removeKey(x);
            }
            else if (cmd == "U")
            {
                int h1id, h2id;
                ss >> h1id >> h2id;
                heapById(h1id).unionWith(heapById(h2id));
            }
            else if (cmd == "P")
            {
                int h;
                ss >> h;

                output("Printing Binomial Heap H" + to_string(h));
                stringstream sout;
                heapById(h).print(h, sout);

                output(sout.str());
            }
            else if (cmd == "VH")
            {
                int h;
                ss >> h;
                stringstream sout;
                heapById(h).visualizeHeap(h, sout);
                output(sout.str());
            }
            else if (cmd == "VB")
            {
                int h;
                ss >> h;
                stringstream sout;
                heapById(h).visualizeBinaryRepresentation(h, sout);
                output(sout.str());
            }
            else
            {
                output("Invalid input.");
            }
        }

        inFile.close();
    }
};

int main()
{
    // Generate output for the 10 testcases
    for (int test = 3; test <= 3; test++)
    {
        string test_id = "";
        if (test < 10)
            test_id += "0";
        test_id += to_string(test);

        string outputFilePath = "output_" + test_id + ".txt";
        string inputFilePath = "test_" + test_id + "/input.txt";
        IOManager manager(outputFilePath);
        manager.run(inputFilePath);
    }

    return 0;
}
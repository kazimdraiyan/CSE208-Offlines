// Feature 3
void printBinaryRepresentation(int heapId, stringstream &sout) const
{
    int n = size();
    sout << "Binary notation of H" << heapId << ":" << endl;
    sout << "n = " << n << endl;
    sout << "Binary = ";

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
    sout << endl;

    // Traverse root list
    vector<int> degrees;
    Node *current = head;
    while (current)
    {
        degrees.push_back(current->degree);
        current = current->next;
    }

    // Print the degrees of the binomial trees
    for (int i = degrees.size() - 1; i >= 0; i++)
    {
        sout << "B" << degrees[i];
        if (i > 0)
            sout << " ";
    }
}

else if (cmd == "VD")
{
    int h;
    ss >> h;
    stringstream sout;
    heapById(h).printBinaryRepresentation(h, sout);
    output(sout.str());
}

// * Part B helper methods
// Called by consolidate() right before each linkTrees(x, y), so Union
// visualization can trace links in the exact order they happen without
// duplicating consolidate()'s logic. nullptr when not visualizing.
function<void(Node *, Node *)> onLink = nullptr;

// --- Layout for a single tree, rooted at `root` ---
// TODO: compute an (x, y) position per node so parent-child structure is
// unambiguous. Suggested approach (matches how you already traverse
// levels in print()): reuse a level-by-level walk (root->child->next chain
// per level), but instead of sorting keys, keep each node paired with an
// x coordinate derived from its position under its parent, e.g.:
//   - assign each LEAF an increasing x (0, 1, 2, ...) left to right
//   - assign each internal node's x = average of its children's x
// y = level (you already compute this in print()).
// Store results in a map<Node*, pair<int,int>> or similar, then blit onto
// a 2D char canvas (vector<vector<char>>) using box-drawing chars for
// parent-child edges.
void computeLayout(Node *root /* TODO: output params */) const;

// Draws ONE Bk tree using computeLayout's positions. Appends to sout.
void drawTree(Node *root, stringstream &sout) const;

// * Part B public methods
// ... existing insert/findMin/extractMin/unionWith/decreaseKey/removeKey/
//     print/size unchanged ...

// --- Requirement 1: Visualize a heap ---
// For each Bk in the root list (same traversal as print()), draw it with
// drawTree() instead of the flat level-order dump. Label each tree with
// "Binomial Tree B<k>, root = <key>" so order/root are explicit per the
// handout's requirement.
void visualizeHeap(int heapId, stringstream &sout) const
{
    sout << "Visualizing Binomial Heap H" << heapId << endl;

    if (!head)
    {
        sout << "Heap H" << heapId << " is empty." << endl;
        return;
    }

    Node *currentRoot = head;
    while (currentRoot)
    {
        sout << "Binomial Tree B" << currentRoot->degree
             << ", root = " << currentRoot->key << endl;

        // TODO: drawTree(currentRoot, sout);

        currentRoot = currentRoot->next;
        if (currentRoot)
            sout << endl;
    }

    // Requirement 3: degree-profile bar for this heap
    printBinaryRepresentation(heapId, sout);
}

// --- Requirement 2: Visualize Union ---
// Shows: this BEFORE, other BEFORE, every Bk+Bk->Bk+1 link IN ORDER
// (via onLink hook into consolidate), then the final merged heap.
// NOTE: call this INSTEAD OF unionWith() when you want the trace -
// it performs the actual union (reusing mergeRootLists/consolidate),
// it doesn't just describe one.
void visualizeUnion(BinomialHeap &other, int idA, int idB, stringstream &sout)
{
    sout << "Before Union:" << endl;
    visualizeHeap(idA, sout);
    sout << endl;
    other.visualizeHeap(idB, sout);
    sout << endl;

    sout << "Linking steps:" << endl;
    onLink = [&sout](Node *parent, Node *child)
    {
        sout << "Linked B" << (parent->degree - 1)
             << " (root " << child->key << ") under B" << (parent->degree - 1)
             << " (root " << parent->key << ") -> new B" << parent->degree
             << " (root " << parent->key << ")" << endl;
        // TODO: double check degree bookkeeping - linkTrees increments
        // parent->degree BEFORE or AFTER this hook fires; adjust the
        // logged degree numbers to match whichever order you wired in.
    };

    // TODO: merge other's locate map into this (same as unionWith)
    // TODO: head = mergeRootLists(head, other.head);
    // TODO: consolidate();   // <-- onLink fires during this call
    // TODO: other.head = nullptr; other.locate.clear();

    onLink = nullptr; // stop tracing once union is done

    sout << endl
         << "After Union:" << endl;
    visualizeHeap(idA, sout);
}
#include <bits/stdc++.h>

using namespace std;
using namespace chrono;

struct Node
{
    int key;
    Node *left;
    Node *right;
    int height;

    Node(int key) : key(key), left(nullptr), right(nullptr), height(1) {}
};

class AVLTree
{
    Node *root;
    int node_count;

    void destroy(Node *n)
    {
        if (n != nullptr)
        {
            destroy(n->left);
            destroy(n->right);
            delete n;
        }
    }

    int height(Node *n)
    {
        return n == nullptr ? 0 : n->height;
    }

    void update_height(Node *n)
    {
        if (n == nullptr)
            return;
        n->height = 1 + max(height(n->left), height(n->right));
    }

    int balance_factor(Node *n)
    {
        return height(n->left) - height(n->right);
    }

    // returns the replacement of x
    Node *rotate_left(Node *x)
    {
        Node *y = x->right;
        x->right = y->left;
        y->left = x;
        update_height(x);
        update_height(y);
        return y;
    }

    // returns the replacement of x
    Node *rotate_right(Node *x)
    {
        Node *y = x->left;
        x->left = y->right;
        y->right = x;
        update_height(x);
        update_height(y);
        return y;
    }

    // returns the root of the rebalanced subtree
    Node *rebalance(Node *node)
    {
        update_height(node);
        int bf = balance_factor(node);

        if (bf > 1)
        {                                             // left side heavier
            if (balance_factor(node->left) < 0)       // left subtree is right-heavy
                node->left = rotate_left(node->left); // fix left subtree first (LR case)
            return rotate_right(node);                // LR, LL case
        }
        if (bf < -1)
        {                                                // right side heavier
            if (balance_factor(node->right) > 0)         // right subtree is left-heavy
                node->right = rotate_right(node->right); // fix right subtree first (RL case)
            return rotate_left(node);                    // RL, RR case
        }
        return node; // already balanced
    }

    Node *successor(Node *node)
    {
        if (node == nullptr)
            return nullptr;

        while (node->left != nullptr)
        {
            node = node->left;
        }
        return node;
    }

    // inserts, rebalances, returns subtree pointer
    Node *insert_recur(Node *node, int key)
    {
        if (node == nullptr)
        {
            node_count++;
            return new Node(key);
        }

        if (key < node->key)
            node->left = insert_recur(node->left, key);
        else if (key > node->key)
            node->right = insert_recur(node->right, key);
        else
            return node; // duplicate

        return rebalance(node); // return rebalanced subtree
    }

    // deletes, rebalances, returns subtree pointer
    Node *erase_recur(Node *node, int key)
    {
        if (node == nullptr)
            return nullptr; // key not found

        if (key < node->key)
            node->left = erase_recur(node->left, key);
        else if (key > node->key)
            node->right = erase_recur(node->right, key);
        else
        {
            // found the node to delete
            if (node->left == nullptr || node->right == nullptr)
            {
                node_count--;
                Node *child = (node->left != nullptr) ? node->left : node->right;
                delete node;
                return child; // sends the child (may be nullptr if node was leaf) as replacement
            }

            // two subtree case
            Node *succ = successor(node->right);
            node->key = succ->key;
            node->right = erase_recur(node->right, succ->key);
        }

        return rebalance(node); // return rebalanced subtree
    }

    void traverse_recur(Node *root, vector<int> &result)
    {
        if (root != nullptr)
        {
            traverse_recur(root->left, result);
            result.push_back(root->key);
            traverse_recur(root->right, result);
        }
    }

    void print_nested_parentheses_recur(Node *root, ostream &out) const
    {
        if (root != nullptr)
        {
            out << root->key;
            // out << "[" << root->height << "]";
            if (root->left != nullptr || root->right != nullptr)
            {
                out << "(";
                print_nested_parentheses_recur(root->left, out);
                out << ",";
                print_nested_parentheses_recur(root->right, out);
                out << ")";
            }
        }
    }

public:
    AVLTree()
    {
        root = nullptr;
        node_count = 0;
    }

    ~AVLTree() {
        destroy(root);
    }

    bool find(int key)
    {
        Node *current = root;
        while (current != nullptr)
        {
            if (key < current->key)
                current = current->left;
            else if (key > current->key)
                current = current->right;
            else
                return true;
        }
        return false;
    }

    bool insert(int key)
    {
        int previous_node_count = node_count;
        root = insert_recur(root, key);
        return node_count > previous_node_count;
    }

    bool erase(int key)
    {
        int previous_node_count = node_count;
        root = erase_recur(root, key);
        return node_count < previous_node_count;
    }

    vector<int> traverse()
    {
        vector<int> result;
        traverse_recur(root, result);
        return result;
    }

    void print_nested_parentheses(ostream &out)
    {
        print_nested_parentheses_recur(root, out);
        out << endl;
    }
};

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        cerr << "Usage: " << argv[0] << " <input-file> <output-file>" << endl;
        return 1;
    }

    ifstream fin(argv[1]);
    if (!fin)
    {
        cerr << "Error: could not open input file " << argv[1] << endl;
        return 1;
    }

    ofstream fout(argv[2]);
    if (!fout)
    {
        cerr << "Error: could not open output file " << argv[2] << endl;
        return 1;
    }

    AVLTree tree;
    string cmd;

    long long total_ns = 0;
    long long insert_ns = 0, insert_count = 0;
    long long delete_ns = 0, delete_count = 0;
    long long find_ns = 0, find_count = 0;
    long long traverse_ns = 0, traverse_count = 0;

    while (fin >> cmd)
    {
        if (cmd == "I")
        {
            int x;
            fin >> x;

            auto start = steady_clock::now();
            bool success = tree.insert(x);
            auto end = steady_clock::now();
            long long duration = duration_cast<nanoseconds>(end - start).count();
            insert_ns += duration;
            total_ns += duration;
            insert_count++;

            if (success)
                tree.print_nested_parentheses(fout);
            else
                fout << "duplicate" << endl;
        }
        else if (cmd == "D")
        {
            int x;
            fin >> x;

            auto start = steady_clock::now();
            bool success = tree.erase(x);
            auto end = steady_clock::now();
            long long duration = duration_cast<nanoseconds>(end - start).count();
            delete_ns += duration;
            total_ns += duration;
            delete_count++;

            if (success)
                tree.print_nested_parentheses(fout);
            else
                fout << "not found" << endl;
        }
        else if (cmd == "F")
        {
            int x;
            fin >> x;

            auto start = steady_clock::now();
            bool success = tree.find(x);
            auto end = steady_clock::now();
            long long duration = duration_cast<nanoseconds>(end - start).count();
            find_ns += duration;
            total_ns += duration;
            find_count++;

            fout << (success ? "found" : "not found") << endl;
        }
        else if (cmd == "T")
        {
            auto start = steady_clock::now();
            vector<int> keys = tree.traverse();
            auto end = steady_clock::now();
            long long duration = duration_cast<nanoseconds>(end - start).count();
            traverse_ns += duration;
            total_ns += duration;
            traverse_count++;

            for (int i = 0; i < keys.size(); i++)
            {
                if (i > 0)
                    fout << " ";
                fout << keys[i];
            }
            fout << endl;
        }
    }

    fin.close();
    fout.close();

    cout << "AVL\n";
    cout << "operation,count,total_ns,average_ns\n";
    cout << "insert," << insert_count << "," << insert_ns << "," << (insert_count ? to_string(insert_ns / insert_count) : "N/A") << "\n";
    cout << "delete," << delete_count << "," << delete_ns << "," << (delete_count ? to_string(delete_ns / delete_count) : "N/A") << "\n";
    cout << "find," << find_count << "," << find_ns << "," << (find_count ? to_string(find_ns / find_count) : "N/A") << "\n";
    cout << "traverse," << traverse_count << "," << traverse_ns << "," << (traverse_count ? to_string(traverse_ns / traverse_count) : "N/A") << "\n";

    return 0;
}

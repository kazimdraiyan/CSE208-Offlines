#include <bits/stdc++.h>

using namespace std;
using namespace chrono;

struct Event
{
    int id;
    int s, e;
    Event *left = nullptr, *right = nullptr;
    int height = 1;
    int max_end; // largest e anywhere in this node's subtree

    Event(int id, int s, int e) : id(id), s(s), e(e), max_end(e) {}
};

class IntervalAVLTree
{
    Event *root = nullptr;
    int next_id = 1;

    unordered_map<int, pair<int, int>> id_to_interval; // id -> (s, e), current events only

    void destroy(Event *n)
    {
        if (n != nullptr)
        {
            destroy(n->left);
            destroy(n->right);
            delete n;
        }
    }

    int height(Event *n)
    {
        return n == nullptr ? 0 : n->height;
    }

    int end_of(Event *n)
    {
        return n == nullptr ? INT_MIN : n->max_end;
    }

    // update max_end along with height
    void recompute(Event *n)
    {
        n->height = 1 + max(height(n->left), height(n->right));
        n->max_end = max({n->e, end_of(n->left), end_of(n->right)});
    }

    int balance_factor(Event *n)
    {
        return height(n->left) - height(n->right);
    }

    // compare function
    bool less_key(int s1, int id1, int s2, int id2)
    {
        if (s1 != s2)
            return s1 < s2;
        return id1 < id2;
    }

    Event *rotate_left(Event *x)
    {
        Event *y = x->right;
        x->right = y->left;
        y->left = x;
        recompute(x);
        recompute(y);
        return y;
    }

    Event *rotate_right(Event *x)
    {
        Event *y = x->left;
        x->left = y->right;
        y->right = x;
        recompute(x);
        recompute(y);
        return y;
    }

    Event *rebalance(Event *n)
    {
        recompute(n);
        int bf = balance_factor(n);
        if (bf > 1)
        {
            if (balance_factor(n->left) < 0)
                n->left = rotate_left(n->left);
            return rotate_right(n);
        }
        if (bf < -1)
        {
            if (balance_factor(n->right) > 0)
                n->right = rotate_right(n->right);
            return rotate_left(n);
        }
        return n;
    }

    Event *insert_recur(Event *n, int id, int s, int e)
    {
        if (n == nullptr)
            return new Event(id, s, e);
        if (less_key(s, id, n->s, n->id))
            n->left = insert_recur(n->left, id, s, e);
        else
            n->right = insert_recur(n->right, id, s, e); // keys cannot be duplicate since id is unique
        return rebalance(n);
    }

    Event *find_min(Event *n)
    {
        while (n->left != nullptr)
            n = n->left;
        return n;
    }

    Event *erase_recur(Event *n, int s, int id)
    {
        if (n == nullptr)
            return nullptr;

        if (less_key(s, id, n->s, n->id))
            n->left = erase_recur(n->left, s, id);
        else if (less_key(n->s, n->id, s, id))
            n->right = erase_recur(n->right, s, id);
        else
        {
            if (n->left == nullptr || n->right == nullptr)
            {
                Event *child = (n->left != nullptr) ? n->left : n->right;
                delete n;
                return child;
            }
            Event *succ = find_min(n->right);
            int succ_s = succ->s, succ_e = succ->e, succ_id = succ->id;
            n->right = erase_recur(n->right, succ_s, succ_id);
            n->id = succ_id;
            n->s = succ_s;
            n->e = succ_e;
        }
        return rebalance(n);
    }

    bool overlaps(int as, int ae, int bs, int be)
    {
        return as < be && bs < ae;
    }

    Event *find_any_conflict_recur(Event *n, int query_s, int query_e)
    {
        if (n == nullptr || end_of(n) <= query_s)
            return nullptr; // every left subtree of n has e <= n.s, so can't overlap, thus skipped
        Event *left_conflicted_event = find_any_conflict_recur(n->left, query_s, query_e);
        if (left_conflicted_event != nullptr)
            return left_conflicted_event;

        if (overlaps(n->s, n->e, query_s, query_e))
            return n;

        if (n->s >= query_e)
            return nullptr; // every right subtree of n has s >= s.e, so can't overlap, thus skipped
        return find_any_conflict_recur(n->right, query_s, query_e);
    }

    void report_all_recur(Event *n, int query_s, int query_e, vector<Event *> &result)
    {
        if (n == nullptr || end_of(n) <= query_s)
            return; // prune left subtree
        report_all_recur(n->left, query_s, query_e, result);

        if (n->s >= query_e)
            return; // prune self and right subtree

        if (overlaps(n->s, n->e, query_s, query_e))
            result.push_back(n);
        report_all_recur(n->right, query_s, query_e, result);
    }

    Event *next_event_recur(Event *n, int t)
    {
        if (n == nullptr)
            return nullptr;
        if (n->s < t)
            return next_event_recur(n->right, t);
        Event *left_conflicted_event = next_event_recur(n->left, t);
        return left_conflicted_event != nullptr ? left_conflicted_event : n;
    }

    void print_nested_recur(Event *n, ostream &out) const
    {
        if (n == nullptr)
            return;
        out << n->id;
        if (n->left != nullptr || n->right != nullptr)
        {
            out << "(";
            print_nested_recur(n->left, out);
            out << ",";
            print_nested_recur(n->right, out);
            out << ")";
        }
    }

public:
    ~IntervalAVLTree()
    {
        destroy(root);
    }

    void print_nested(ostream &out = cout)
    {
        print_nested_recur(root, out);
        out << endl;
    }

    int add(int s, int e)
    {
        int id = next_id++;
        root = insert_recur(root, id, s, e);
        id_to_interval[id] = {s, e};
        return id;
    }

    bool remove(int id)
    {
        auto it = id_to_interval.find(id);
        if (it == id_to_interval.end()) // id not found
            return false;

        int s = it->second.first;
        root = erase_recur(root, s, id);
        id_to_interval.erase(it);
        return true;
    }

    // delete and insert
    bool update(int id, int new_s, int new_e)
    {
        auto it = id_to_interval.find(id);
        if (it == id_to_interval.end())
            return false;

        int old_s = it->second.first;
        root = erase_recur(root, old_s, id);
        root = insert_recur(root, id, new_s, new_e);
        it->second = {new_s, new_e};
        return true;
    }

    bool conflict(int query_s, int query_e)
    {
        return find_any_conflict_recur(root, query_s, query_e) != nullptr;
    }

    vector<Event *> overlaps_all(int query_s, int query_e)
    {
        vector<Event *> result;
        report_all_recur(root, query_s, query_e, result);
        return result;
    }

    vector<Event *> events_at(int t)
    {
        return overlaps_all(t, t + 1); // if s <= t < e the event overlaps [t, t+1)
    }

    Event *next_event(int t)
    {
        return next_event_recur(root, t);
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

    IntervalAVLTree tree;
    string cmd;

    long long total_ns = 0;
    long long add_ns = 0, add_count = 0;
    long long remove_ns = 0, remove_count = 0;
    long long update_ns = 0, update_count = 0;
    long long conflict_ns = 0, conflict_count = 0;
    long long overlaps_ns = 0, overlaps_count = 0;
    long long at_ns = 0, at_count = 0;
    long long next_ns = 0, next_count = 0;

    while (fin >> cmd)
    {
        if (cmd == "ADD")
        {
            int s, e;
            fin >> s >> e;

            auto start = steady_clock::now();
            tree.add(s, e);
            auto end = steady_clock::now();
            long long duration = duration_cast<nanoseconds>(end - start).count();
            add_ns += duration;
            total_ns += duration;
            add_count++;

            tree.print_nested(fout);
        }
        else if (cmd == "REMOVE")
        {
            int id;
            fin >> id;

            auto start = steady_clock::now();
            bool success = tree.remove(id);
            auto end = steady_clock::now();
            long long duration = duration_cast<nanoseconds>(end - start).count();
            remove_ns += duration;
            total_ns += duration;
            remove_count++;

            if (success)
                tree.print_nested(fout);
            else
                fout << "not found" << endl;
        }
        else if (cmd == "UPDATE")
        {
            int id, s, e;
            fin >> id >> s >> e;

            auto start = steady_clock::now();
            bool success = tree.update(id, s, e);
            auto end = steady_clock::now();
            long long duration = duration_cast<nanoseconds>(end - start).count();
            update_ns += duration;
            total_ns += duration;
            update_count++;

            if (success)
                tree.print_nested(fout);
            else
                fout << "not found" << endl;
        }
        else if (cmd == "CONFLICT")
        {
            int s, e;
            fin >> s >> e;

            auto start = steady_clock::now();
            bool success = tree.conflict(s, e);
            auto end = steady_clock::now();
            long long duration = duration_cast<nanoseconds>(end - start).count();
            conflict_ns += duration;
            total_ns += duration;
            conflict_count++;

            fout << (success ? "yes" : "no") << endl;
        }
        else if (cmd == "OVERLAPS")
        {
            int s, e;
            fin >> s >> e;

            auto start = steady_clock::now();
            vector<Event *> hits = tree.overlaps_all(s, e);
            auto end = steady_clock::now();
            long long duration = duration_cast<nanoseconds>(end - start).count();
            overlaps_ns += duration;
            total_ns += duration;
            overlaps_count++;

            if (hits.empty())
                fout << "none" << endl;
            else
            {
                for (int i = 0; i < hits.size(); i++)
                {
                    if (i > 0)
                        fout << " ";
                    fout << hits[i]->id;
                }
                fout << endl;
            }
        }
        else if (cmd == "AT")
        {
            int t;
            fin >> t;

            auto start = steady_clock::now();
            vector<Event *> hits = tree.events_at(t);
            auto end = steady_clock::now();
            long long duration = duration_cast<nanoseconds>(end - start).count();
            at_ns += duration;
            total_ns += duration;
            at_count++;

            if (hits.empty())
                fout << "none" << endl;
            else
            {
                for (int i = 0; i < hits.size(); i++)
                {
                    if (i > 0)
                        fout << " ";
                    fout << hits[i]->id;
                }
                fout << endl;
            }
        }
        else if (cmd == "NEXT")
        {
            int t;
            fin >> t;

            auto start = steady_clock::now();
            Event *ev = tree.next_event(t);
            auto end = steady_clock::now();
            long long duration = duration_cast<nanoseconds>(end - start).count();
            next_ns += duration;
            total_ns += duration;
            next_count++;

            if (ev == nullptr)
                fout << "none" << endl;
            else
                fout << ev->id << " " << ev->s << " " << ev->e << endl;
        }
    }

    fin.close();
    fout.close();

    long long all_count = add_count + remove_count + update_count + conflict_count + overlaps_count + at_count + next_count;

    cout << "Interval Scheduler\n";
    cout << "operation,count,total_ns,average_ns\n";
    cout << "add," << add_count << "," << add_ns << "," << (add_count ? to_string(add_ns / add_count) : "N/A") << "\n";
    cout << "remove," << remove_count << "," << remove_ns << "," << (remove_count ? to_string(remove_ns / remove_count) : "N/A") << "\n";
    cout << "update," << update_count << "," << update_ns << "," << (update_count ? to_string(update_ns / update_count) : "N/A") << "\n";
    cout << "conflict," << conflict_count << "," << conflict_ns << "," << (conflict_count ? to_string(conflict_ns / conflict_count) : "N/A") << "\n";
    cout << "overlaps," << overlaps_count << "," << overlaps_ns << "," << (overlaps_count ? to_string(overlaps_ns / overlaps_count) : "N/A") << "\n";
    cout << "at," << at_count << "," << at_ns << "," << (at_count ? to_string(at_ns / at_count) : "N/A") << "\n";
    cout << "next," << next_count << "," << next_ns << "," << (next_count ? to_string(next_ns / next_count) : "N/A") << "\n";

    return 0;
}
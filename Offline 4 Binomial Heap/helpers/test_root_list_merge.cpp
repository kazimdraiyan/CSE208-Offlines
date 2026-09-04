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

    Node(int d, int k)
        : key(k), degree(d), parent(nullptr), child(nullptr), next(nullptr) {}

    void printNexts()
    {
        Node *current = this;
        cout << degree << "(" << key << ")";
        while (current->next)
        {
            cout << "->" << current->next->degree << "(" << current->next->key << ")";
            current = current->next;
        }
        cout << endl;
    }
};

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

void linkTrees(Node *x, Node *y)
{
    // TODO: y.prev.next = y.next?

    y->next = x->child;
    x->child = y;
    y->parent = x;
    x->degree++;
}

Node *buildList(vector<int> &degrees, int k)
{
    Node *head = new Node(degrees[0], k);
    Node *current = head;
    for (int i = 1; i < degrees.size(); i++)
    {
        current->next = new Node(degrees[i], k);
        current = current->next;
    }
    return head;
}

void consolidate(Node *&head)
{
    cout << "Function started" << endl;
    if (!head)
        return; // TODO: Is this necessary?

    cout << "Root list not empty" << endl;
    Node *previous = nullptr;
    Node *current = head;
    while (current && current->next)
    {
        Node *next = current->next;
        Node *secondNext = next->next;
        cout << "Loop ran: " << current->degree << ", " << next->degree << endl;

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

Node *reverseAndClearParent(Node *head)
{
    if (!head || !head->next)
        return head;

    head->parent = nullptr;

    Node *newHead = reverseAndClearParent(head->next);
    head->next->next = head;
    head->next = nullptr;
    return newHead;
}

int main()
{
    vector<int> degrees1 = {0, 1, 4};
    Node *head1 = buildList(degrees1, 1);

    head1->printNexts();
    head1 = reverseAndClearParent(head1);

    head1->printNexts();

    // vector<int> degrees2 = {0, 1, 2};
    // BinomialNode *head2 = buildList(degrees2, 2);

    // head1->printNexts();
    // head2->printNexts();

    // BinomialNode *mergedHead = mergeRootLists(head1, head2);
    // mergedHead->printNexts();

    // consolidate(mergedHead);
    // mergedHead->printNexts();
    // head1->printNexts();
    // head2->printNexts();
}
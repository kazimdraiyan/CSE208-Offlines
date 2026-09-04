### **BANGLADESH UNIVERSITY OF ENGINEERING AND TECHNOLOGY DEPARTMENT OF COMPUTER SCIENCE AND ENGINEERING CSE 208 – Data Structures and Algorithms II Sessional** 

# **Offline on Advanced Heaps: Binomial Heap** 

**Deadline: 04 September 2026, 11:30 PM** 

**Two parts. Part A** is the prescribed Binomial Min Heap implementation. **Part B** is a creative implementation task: design an automatic visualizer for the _actual trees_ created by Part A. **Use OOP concepts throughout the assignment.** Your code should be modular, reusable, and easy to explain during evaluation. 

# **1 Part A: Binomial Heap Implementation** 

Implement a **Binomial Min Heap** . Maintain two heaps, _H_ 1 and _H_ 2, both initially empty. Every tree must be heap-ordered, and there must be at most one Binomial Tree of each order _k_ . 

For `U h1 h2` , store Union( _Hh_ 1 _, Hh_ 2) in _Hh_ 1 and make _Hh_ 2 empty. Keys are integers and are unique across the two heaps at any instant. 

## **1.1 Required Commands** 

|**Operation**|Command|Meaning|Time|
|---|---|---|---|
|**Insert**|`I h x`|Insert _x_ into _Hh_.|_O_(log_n_)|
|**Find Min**|`F h`|Return the minimum key of _Hh_.|_O_(log_n_)|
|**Extract Min**|`E h`|Return and remove the minimum key.|_O_(log_n_)|
|**Decrease Key**|`D h x y`|Decrease key _x_ to _y_.|_O_(log_n_)<sup>_∗_</sup>|
|**Remove Key**|`R h x`|Delete key _x_.|_O_(log_n_)<sup>_∗_</sup>|
|**Union**|`U h1 h2`|_Hh_1 _←_Union(_Hh_1_, Hh_2); empty _Hh_2.|_O_(log_n_)|
|**Print**|`P h`|Print the heap in the fxed format below.|–|



> _∗_ For `Decrease Key` and `Remove Key` , the stated bound applies after the node containing _x_ has been located, matching the handle/pointer assumption used in class. 

**Decrease Key:** exchange _key values_ with parents as needed; do not physically swap tree nodes. **Remove Key:** decrease the key to a value **strictly smaller than** _−_ 10<sup>9</sup> (the minimum valid input key), then perform `Extract Min` . Do not use _−_ 10<sup>9</sup> itself as the sentinel, because _−_ 10<sup>9</sup> may already be present in the heap. 

## **1.2 Union Convention** 

Use the following rules so that provided output is reproducible: 

1. Keep root lists in increasing order of degree. 

2. Consolidate left to right. With three consecutive roots of the same degree, do not link the first two; advance to the second root. Otherwise link equal-degree neighbors. 

3. The smaller root key becomes the parent; the other root becomes its **first child** . 

4. For `Insert` , create a one-node heap and perform Union with the current heap, as discussed in class. 

## **1.3 Standard Print Format** 

The `P h` command is a fixed output used for automatic checking. It is **not** the creative visualizer of Part B. 

- Print heap size. 

1 

- Print trees in order _B_ 0 _, B_ 1 _, B_ 2 _, . . ._ . 

- Print each tree level by level. 

- Within one level, print keys in increasing numerical order. 

### **Non-empty heap example:** 

```
PrintingBinomialHeapH1
Heapsize:5
BinomialTree,B0
Level0:30
BinomialTree,B2
Level0:5
Level1:720
Level2:12
```

### **Empty heap:**

```
PrintingBinomialHeapH1
Heapsize:0
HeapH1isempty.
```

For `F h` and `E h` , print exactly:

```[txt]
FindMinreturned:x
ExtractMinreturned:x
```

### **Formatting is case-sensitive. Do not print extra prompts, debug messages, or blank lines in normal Part A output.**

**What Part A Print checks:** the provided output is used to check heap size, the set of _Bk_ trees, and the level of each key. Because keys within a level are printed in sorted order, complete parent–child relationships are **not** determined by `P h` ; those relationships are shown and manually evaluated in Part B.

## **1.4 Input, Output, and Evaluation**

**Input must be read from** **`input.txt` .**

**Every required output line must be written to** **`output.txt` and also printed to the console.** The two outputs must be identical. 

`Insert` , `Decrease Key` , `Remove Key` , and `Union` produce no normal Part A output. `Find Min` , `Extract Min` , and `Print` produce output exactly as specified.

**During evaluation, you must be able to compare your** **`output.txt` with the provided output file using Diffchecker or an equivalent line-by-line comparison tool.**

## **1.5 Assumptions and Restrictions**

- At most 10<sup>5</sup> commands; valid input keys are in [ _−_ 10<sup>9</sup> _,_ 10<sup>9</sup> ].

- `I h x` : _x_ does not exist in either heap.

- `D h x y` : _x_ exists in _Hh_ , _y < x_ , and _y_ does not exist in either heap.

- `R h x` : _x_ exists in _Hh_ .

- `Find Min` / `Extract Min` will not be called on an empty heap.

- Union heap identifiers are always different; all commands are otherwise valid.

- **Do not rebuild a heap from all stored keys after an operation.**

- **Do not replace the Binomial Heap with a built-in heap/priority queue.** Ordinary containers may be used only as supporting storage, e.g., for printing, lookup, or visualization.

## **1.6 Sample Input / Output**

### **input.txt**

### **output.txt and console**

```[txt]
I120PrintingBinomialHeapH1
I15Heapsize:3
I130BinomialTree,B0
I212Level0:30
I27BinomialTree,B1
P1Level0:5
P2Level1:20
U12PrintingBinomialHeapH2
P1Heapsize:2
D1303BinomialTree,B1
F1Level0:7
E1Level1:12
P1PrintingBinomialHeapH1
Heapsize:5
BinomialTree,B0
Level0:30
BinomialTree,B2
Level0:5
Level1:720
Level2:12
FindMinreturned:3
ExtractMinreturned:3
PrintingBinomialHeapH1
Heapsize:4
BinomialTree,B2
Level0:5
Level1:720
Level2:12
```

# **– 2 Part B: Creative Implementation Binomial Heap Visualizer**

**You are NOT designing new Binomial Trees.** Your Part A code determines every node and parent–child relationship. In Part B, your job is to design **how those actual trees are automatically visualized** .

There is no prescribed visual format and no sample visualization. The layout and presentation are part of your implementation design.

## **2.1 Your Visualizer Must**

1. **Visualize a heap:** show every _Bk_ separately, identify its order and root, and make actual parent–child relationships clear. A level-order list alone is not enough.

2. **Visualize Union:** show the two heaps before Union, every link _Bk_ + _Bk → Bk_ +1 in order, and the final heap.

3. **Add one useful feature of your own design** that communicates extra structural/operational information. A purely decorative change does not count.

## **2.2 Implementation Rules for Part B**

- Reuse the **same heap and node structures** from Part A; do not create a separate fake heap for visualization.

- Temporary layout data (node pointers, coordinates, widths, etc.) is allowed.

- Text/ASCII/Unicode, terminal output, coordinates, or a GUI may be used. A GUI is not required.

- You must compute the layout yourself. Automatic tree/graph layout tools such as Graphviz are not allowed. Basic drawing libraries are allowed if your code computes all positions.

- The evaluator will construct heaps only through valid Part A commands. For Part B evaluation, there will be at most **31 elements** in total and no displayed tree larger than _B_ 4.

• Document how to invoke the heap and Union visualizations in `README.txt` .

# **3 Mark Distribution (Total: 100)**

|**Component**|**Marks**|
|---|---|
|**Part A: Binomial Heap Implementation**|**60**|
|**Part B: Creative Binomial Heap Visualizer**|**30**|
|**Evaluation / Viva**|**10**|
|**Total**|**100**|

# **4 Submission and Evaluation**

- **Use OOP concepts** and write readable, reusable, well-structured code.

- Part A and Part B must use the same Binomial Heap implementation.

- Include `README.txt` with compile/run instructions, file-I/O instructions, Part B invocation, and a brief description of your own visualization feature.

- During evaluation, be prepared to run the provided `input.txt` , generate `output.txt` , show the console output, and demonstrate the diff comparison.

- Submit source files and `README.txt` inside a directory named with your 7-digit student ID. Compress it as `.zip` and submit through Moodle.

# **5 Special Instructions**

**The submitted program must implement an actual Binomial Heap.** A built-in priority queue, rebuilding from all keys, or a separate fake visualization structure is not acceptable.

**You must be able to explain both the Binomial Heap operations and the implementation decisions used in your visualizer during the evaluation/viva.**

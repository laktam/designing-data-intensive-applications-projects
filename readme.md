# Designing data intensive applications implementations
Learning project inspired by Designing Data-Intensive Applications
## chapter 4:
1. A simple **append-only** file-based key-value store implemented in C
    - set key value: append a key-value pair to db.txt.
        - o(1)
    - get key: retrieve the value associated with a key.
        - o(n) : it may need to scan the whole file looking for the key
2. **Append-only** + in memory index
    - To make get faster we need an index, a separate data structure that help us efficently find values, but it add an overhead in writes (set method) because the index need to be updated on every write
    - the problem in this case is that it don't take existing db content in the start as it is not in the index
    - the second problem is that index data is lost when the program end as it is not persisted
    - if we ignore this, we have now o(1) write and reads, appart from the time where we resize the hashmap array when it reaches the load factor
    - and in this implementation we override old value for same key because we have the exact offset in the file
    - db running example:
        ```console
        $.\db_w_index.exe
        >get test
        value
        >set test value 2
        >get test 
        value 2
        >get nok
        >set tt a new value
        >set key verry verry long value
        >get tt
        a new value
        >get key
        verry verry long value
        >
        ```
3. **SSTable** (Sorted Strings Table)
    a file of key value pairs sorted by key
    - pairs are grouped in blocks (few kilobytes)
    - first key of each block is stored in the index (sparse index), also stored
    - the index is a B-tree, a trie or another data structure that allows queries to
    quickly look up a particular key
    - and you have to scan just between two indices to get the result or conclude that it doesn't exist in the database

**Implementation** : 
- data structures :
    - Skip list (memtable in RAM): stores key + data in sorted order.
    - SSTable (disk): stores key + data in sorted order.
    - SSTable index (disk): stores key + offset to locate data in the SSTable.
- summary of the paragraph "Constructing and merging SSTables"
    1. Write: Add key-value pairs to a sorted in-memory structure (called a memtable) (i will use a skip list).
    2. Flush: When full (couple MBs), write the memtable to disk as a sorted SSTable with an index. Create a new memtable for new writes.
        - SSTable is divesed into **blocks** of few KBs and first key of each **block** is stored on the index (sparce index)
        - for index : i will use a sorted array of each block's first key and offset. Binary search the array to find the relevant block (other options are B tree, ...).
    3. Read: Search the memtable first, then SSTables from newest to oldest.
    4. Compaction: Merge SSTables and remove overwritten or deleted values.

    
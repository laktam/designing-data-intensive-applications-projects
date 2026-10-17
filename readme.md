# Designing data intensive applications implementations
Learning project inspired by Designing Data-Intensive Applications
## chapter 4:
1. A simple file-based key-value store implemented in C
    - set key value: append a key-value pair to db.txt.
        - o(1)
    - get key: retrieve the value associated with a key.
        - o(n) : it may need to scan the whole file looking for the key
2. adding an in memory index
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

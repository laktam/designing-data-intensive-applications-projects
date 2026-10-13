# Designing data intensive applications implementations
Learning project inspired by Designing Data-Intensive Applications
## chapter 4:
1. A simple file-based key-value store implemented in C
    - set key value: append a key-value pair to db.txt.
        - o(1)
    - get key: retrieve the value associated with a key.
        - o(n) : it may need to scan the whole file looking for the key
2. Log-Structured Storage
To make get faster we need an index, a separate data structure that help us efficently find values 
but it add an overhead in writes (set method) because the index need to be updated on every write
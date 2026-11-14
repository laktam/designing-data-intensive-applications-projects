## Skip list
- https://www.youtube.com/watch?v=VctNQi7WCkE
- https://www.youtube.com/watch?v=NDGpsfwAaqo

skip list printing example
1. max level = 4
```console
 -> -> -> -> -> -> ->
 -> -> ->d-> -> -> ->
 -> -> ->d-> -> -> ->
 ->a->b->d->e->g->k->
```
2. max level = 8
```console
 -> -> -> -> -> -> -> -> -> -> -> -> -> -> ->
 -> -> -> -> -> -> -> -> -> -> -> -> -> -> ->
 -> -> -> -> -> -> -> -> -> -> -> -> -> -> ->
 -> -> -> -> -> -> -> -> -> -> -> -> -> -> ->
 -> -> -> -> -> -> -> -> -> ->o-> -> -> -> ->
 -> -> -> -> -> -> -> -> -> ->o->p-> -> -> ->
 ->a-> -> -> ->g->i->k-> -> ->o->p-> -> -> ->
 ->a->b->d->e->g->i->k->l->m->o->p->t->u->y->
 ```
3. max level = 4
```console
 -> -> -> -> -> -> -> -> -> -> ->p-> -> -> ->
 -> -> -> -> -> -> -> -> -> -> ->p-> -> -> ->
 -> -> -> -> -> -> ->k->l-> -> ->p-> -> -> ->
 ->a->b->d->e->g->i->k->l->m->o->p->t->u->y->
 ```

## SSTable
- After reaching a configurable max size in memtable (skip list), the data is writen as an sstable
- the sstable format is :
    - data (bytes):
        - |key size (4)|key data|value size (4)|value data|...
    - sparce (not all keys included) index at the end of the file :
        - |key size (4)|key data|offset (8)|
    - at the end (footer) of the file
        - |index size (nbr of elements) (4)|index offset (8)|
- To get a key 
    - we look in memory in the memtable (skip list)
    - if not found we look in the files from last to first
        - we load the index using footer data
        - then we search (binary search) the index for the largest key smaller than or equal to the target key.
            - if it exist we start from its offset and scan that block

## Merging and compacting segments (sstable files)
- can be done in a backgroud thread while reading can continue from these files
- we merge multiple input files to remove duplicate entries and keep the most recent one
- and to delete the ones marked for deletion
    - for deletion we write a spcial value called a TOMBSTONE
    - the TOMBSTONE entry is not deleted imediatly, we need to make sure it doesn't exist in the other segments that are not in the merge operation
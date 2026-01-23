# Generic Insertion Sort Implementation

I have implemented a generic insertion sort for your doubly linked list. This allows you to sort by any attribute of any struct stored in the list.

## How it works

### 1. The Generic Comparison ([compareNodes](file:///home/vincebus/vincebus-coding/dcode-ide/DEPS/SORT/SORT.C#3-26))
The core challenge in C is that `void* data` doesn't know its own structure. To solve this, I used **Byte Offsets**:
- `void *v1 = (char*)d1 + offset;`
- By casting the data pointer to `char*`, we can add a specific number of bytes (the `offset`) to reach the memory address of the attribute we want to sort by.
- We then cast that address back to the correct type (e.g., `int*`, `float*`) to perform the comparison.

### 2. The `SORT_LIST` Macro
In [SORT.H](file:///home/vincebus/vincebus-coding/dcode-ide/DEPS/SORT/SORT.H), I added a macro that makes the code feel like a higher-level language:
```c
#define SORT_LIST(list, structType, attribute, type, ascending) \
    s_sortList(list, offsetof(structType, attribute), type, ascending)
```
- `offsetof(structType, attribute)` is a standard C macro (from `stddef.h`) that calculates the number of bytes from the start of the struct to the member.
- This makes the call very clean: `SORT_LIST(myList, MyStruct, id, SORT_TYPE_INT, true);`

### 3. Pointer-Efficient Insertion Sort
The implementation in [SORT.C](file:///home/vincebus/vincebus-coding/dcode-ide/DEPS/SORT/SORT.C) is optimized for linked lists:
- **No Indexing**: Instead of calling [getNodeByIndex](file:///home/vincebus/vincebus-coding/dcode-ide/DEPS/DATA/DATA.C#132-150) (which is $O(n)$), it simply follows `next` and `prev` pointers. This brings the complexity down from $O(n^3)$ to $O(n^2)$.
- **Data Swapping**: Instead of unlinking and relinking nodes (which is error-prone), it simply swaps the `void* data` pointers. This is much faster and safer for sorting.

## How to Verify (Watcom C / DOS)

Since I cannot run commands in your environment, follow these steps:

1. **Review the Test File**: I created [TEST_SORT.C](file:///home/vincebus/vincebus-coding/dcode-ide/DEPS/SORT/TEST_SORT.C).
2. **Compile**: Use your Watcom compiler (e.g., `wcl386` or via your Makefile).
   ```bash
   wcl386 -i=DEPS/DATA -i=DEPS/SORT DEPS/SORT/TEST_SORT.C
   ```
3. **Run**: Execute `TEST_SORT.EXE` in DOSBox.
4. **Expected Output**: You should see the list printed three times, sorted by ID, then Score, then Name.

## Files Modified
- [SORT.H](file:///home/vincebus/vincebus-coding/dcode-ide/DEPS/SORT/SORT.H): Added `SortType`, `SORT_LIST` macro, and updated signature.
- [SORT.C](file:///home/vincebus/vincebus-coding/dcode-ide/DEPS/SORT/SORT.C): Implemented [compareNodes](file:///home/vincebus/vincebus-coding/dcode-ide/DEPS/SORT/SORT.C#3-26) and generic [s_sortList](file:///home/vincebus/vincebus-coding/dcode-ide/DEPS/SORT/SORT.C#27-53).
- [TEST_SORT.C](file:///home/vincebus/vincebus-coding/dcode-ide/DEPS/SORT/TEST_SORT.C): New test utility.

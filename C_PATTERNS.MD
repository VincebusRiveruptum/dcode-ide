# C Programming Patterns for UI Architecture

This document compares two common patterns for implementing polymorphism and component systems in C.

## 1. Common Header (Struct Inheritance)
This pattern emulates inheritance by ensuring that the "base" data structure is the first member of every "derived" structure.

### Implementation
```c
typedef struct Base {
    int type;
    int id;
} Base;

typedef struct Button {
    Base base;      /* MUST BE FIRST */
    char *label;
} Button;
```

### Key Characteristics
*   **Memory Layout**: A pointer to `Button` is identical to a pointer to `Base`. You can cast [(Base*)button](file:///home/vincebus/vincebus-coding/dcode-ide/DEPS/TUI/TUI.C#113-153) safely.
*   **Single Allocation**: Only one `malloc` is needed for the entire object.
*   **Performance**: Zero indirection. Accessing base fields is direct.

### Pros & Cons
| Pros | Cons |
| :--- | :--- |
| Extremely fast (no pointer chasing) | Less flexible at runtime |
| Memory efficient (contiguous) | Rigid hierarchy |
| Type-safe casting in C | Struct size varies between types |

---

## 2. Composition (Wrapper/Pointer)
This pattern uses a generic container that "points" to a specific implementation.

### Implementation
```c
typedef struct Base {
    int type;
    void *instance;  /* Pointer to Button, Label, etc. */
} Base;

typedef struct Button {
    char *label;
} Button;
```

### Key Characteristics
*   **Indirection**: You must follow a pointer (`base->instance`) to get to the specific data.
*   **Uniformity**: All "Base" objects are exactly the same size.
*   **Separation**: The "System" metadata is decoupled from the "Component" data.

### Pros & Cons
| Pros | Cons |
| :--- | :--- |
| Very flexible (can swap instance) | Double allocation (Base + Instance) |
| Easy to store in fixed-size arrays | Pointer indirection overhead (CPU cycles) |
| Cleaner separation of concerns | Manual memory management is harder |

---

## Recommendation for DOS (386/486)
For high-performance TUI development on vintage hardware, the **Common Header** pattern is usually preferred because it minimizes memory fragmentation and avoids the overhead of pointer indirection during the rendering loop.

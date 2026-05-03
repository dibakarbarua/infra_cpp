**Function Parameters**

Use `const T&` when:
- `T` is expensive to copy: `std::string`, `std::vector`, maps, custom structs/classes.
- The function only needs to read the object.

```cpp
void print_user(const User& user);
```

Use `T&` when:
- The function needs to modify the caller’s object.
- The parameter must not be null.

```cpp
void normalize(User& user);
```

Use `T*` when:
- The argument may be null.
- You want to signal optional/maybe-present mutation.

```cpp
void set_parent(Node* parent);
```

Use plain `T` when:
- `T` is cheap to copy: `int`, `double`, `char`, small enums.
- The function needs its own copy anyway.
- You plan to move from it.

```cpp
void set_name(std::string name);
```

**Loop Variables**

Use `const auto&` when:
- Iterating over objects and only reading them.
- Avoids copying each element.

```cpp
for (const auto& route : routes) {}
```

Use `auto&` when:
- You want to mutate elements in the container.

```cpp
for (auto& route : routes) {
    route.enabled = true;
}
```

Use `auto` when:
- Elements are cheap to copy.
- You intentionally want a copy.

```cpp
for (int x : nums) {}
```

**Return Values**

Return `T` when:
- Creating a new object for the caller.
- This is the normal/default choice.

```cpp
std::vector<int> make_values();
```

Return `T&` or `const T&` when:
- Returning access to an object that already exists and will outlive the call.

```cpp
const std::string& name() const;
```

Avoid returning references to local variables:

```cpp
const std::string& bad() {
    std::string s = "oops";
    return s; // wrong
}
```

**Heap vs Stack**

Reference does not mean heap.

This is stack:

```cpp
User user;
User& ref = user;
```

This is heap:

```cpp
User* user = new User;
User& ref = *user;
```

A reference is just another way to refer to an existing object. The object can live on the stack, heap, inside a vector, inside another object, etc.

**Rule Of Thumb**

Use references when:
- You want to avoid copying.
- You want to mutate an existing object.
- Null is not a valid option.
- The referenced object definitely outlives the reference.

Use pointers when:
- Null is meaningful.
- Ownership/lifetime needs to be explicit.
- You are working with dynamic structures or APIs that require pointers.
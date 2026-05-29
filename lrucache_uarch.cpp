#include <cstdint>
#include <list>
#include <algorithm>
#include <bit>
#include <cassert>

/*
std::rotate in std::vector
1. remove the existing hit entry from its old position
2. insert that same entry at the front

Example:

```cpp
A B C D
    ^
    hit
```

After rotate:

```cpp
C A B D
```

Same as “erase C, insert C at front”.

But performance-wise, `std::rotate` is better than erase-then-insert for this case.

With erase + insert:

```cpp
CacheEntry tmp = *it;
set_array.erase(it);
set_array.insert(set_array.begin(), tmp);
```

You may shift elements twice:

- `erase(it)` shifts elements after `it` left
- `insert(begin)` shifts elements before the insertion right

For the example:

```cpp
A B C D
```

erase `C`:

```cpp
A B D
```

then insert at front:

```cpp
C A B D
```

That touches more elements than necessary.

With:

```cpp
std::rotate(set_array.begin(), it, it + 1);
```

only the prefix up through the hit entry is affected:

```cpp
A B C D
```

becomes:

```cpp
C A B D
```

`D` is untouched.

So yes, same functional result, but `rotate` avoids the “remove then reinsert” container-size changes
 and avoids shifting the suffix after the hit entry. It still moves elements from `begin()` through `it`, 
 because vector storage is contiguous, but it is the cleaner and usually more efficient operation for 
 “promote one existing element to front.”
*/

struct CacheEntry {
  CacheEntry(int tag, int data) {
    tag_ = tag;
    data_ = data;
    valid_ = true;
  }
  int tag_;
  int data_;
  bool valid_;
};

class LRUCache
{
  public:
    LRUCache() {
      cache_size_ = 64 * 1024; // 64KB
      address_bits_ = 32;
      offset_bits_ = 8; // 64B
      index_bits_ = 4; // 16 ways

      // Addressing
      tag_bits_ = address_bits_ - (offset_bits_ + index_bits_);
      tag_mask_ = (uint32_t(1) << tag_bits_) - 1;
      index_mask_ = (uint32_t(1) << index_bits_) - 1;
      offset_mask_ = (uint32_t(1) << offset_bits_) - 1;

      // Sizing
      block_size_ = 1 << offset_bits_;
      num_ways_ = 1 << index_bits_;
      num_sets_ = cache_size_ / (num_ways_ * block_size_);
    }

  LRUCache(int cache_size, int offset_bits, int index_bits) : 
    cache_size_(cache_size), offset_bits_(offset_bits), index_bits_(index_bits) {
      address_bits_ = 32;
      
      // Addressing
      tag_bits_ = address_bits_ - (offset_bits_ + index_bits_);
      tag_mask_ = (uint32_t(1) << tag_bits_) - 1;
      index_mask_ = (uint32_t(1) << index_bits_) - 1;
      offset_mask_ = (uint32_t(1) << offset_bits_) - 1;

      // Sizing
      block_size_ = 1 << offset_bits;
      num_ways_ = 1 << index_bits_;
      num_sets_ = cache_size_ / (num_ways_ * block_size_);
    }
    
  // To be implemented
  virtual bool read_data(uint32_t address, int& data) = 0;
  virtual bool tag_check(uint32_t address) = 0;
  virtual bool write_data(uint32_t address, const int data) = 0;

protected:
  // Addressing
  int address_bits_;
  int tag_bits_;
  int offset_bits_;
  int index_bits_;
  int offset_mask_;
  int index_mask_;
  int tag_mask_;

  // Sizing
  int cache_size_;
  int block_size_;
  int num_sets_;
  int num_ways_;

  // Occupancy Tracking
  std::unordered_map<int /*set_index*/, int /*ways occupied*/> set_occupancy_;

  // Helpers for address decoding
  int set_idx(uint32_t address) const {
    return (address >> offset_bits_) & index_mask_;
  }

  int tag_idx(uint32_t address) const {
    return (address >> (offset_bits_ + index_bits_)) & tag_mask_;
  }

};

class LRUCacheOnList : public LRUCache {
  public:
    bool read_data(uint32_t address, int& data) final {
    }

    bool tag_check(uint32_t address) final {
    }

    bool write_data(uint32_t address, const int data) final {
    }

  private:
};

class LRUCacheOnVector : public LRUCache {
  public:
    bool read_data(uint32_t address, int& data) final {
      int tag = tag_idx(address);
      int set = set_idx(address);

      auto set_it = cache_array_.find(set);
      if (set_it == cache_array_.end()) {
        data = -1;
        return false;
      }

      auto& set_array = set_it->second;

      for(auto it = set_array.begin(); it != set_array.end(); it++) {
        if (it->valid_ && (it->tag_ == tag)) {
          // hit in the cache
          data = it->data_;
          // move to MRU
          std::rotate(set_array.begin(), it, it+1);
          return true;
        }
      }
      data = -1;
      return false;
    }

    bool tag_check(uint32_t address) final {
      int tag = tag_idx(address);
      int set = set_idx(address);

      auto& set_array = cache_array_[set];

      for(auto entry : set_array) {
        if (entry.valid_ && (entry.tag_ == tag)) {
          return true;
        }
      }
      return false;
    }

    bool write_data(uint32_t address, const int data) final {
      int tag = tag_idx(address);
      int set = set_idx(address);

      // First ever access to this set, add it to tag-map
      if (cache_array_.find(set) == cache_array_.end()) {
        initialize_set_array(set);
      }

      // Get set array ptr
      auto& set_array = cache_array_[set];

      // check if it is already cached
      for (auto it = set_array.begin(); it != set_array.end(); it++) {
        if (it->valid_ && (it->tag_ == tag)) {
          // move entry to MRU and update data
          it->data_ = data;
          std::rotate(set_array.begin(), it, it + 1);
          return true;
        }
      }

      // No Eviction:
      // If there are empty ways, push to the front of the vector
      if (get_occupancy(set) < num_ways_) {
        set_array.emplace(set_array.begin(), tag, data);
        add_occupancy(set);
        return true;
      }

      // Eviction:
      set_array.pop_back();
      set_array.emplace(set_array.begin(), tag, data);
      return false;
    }

  private:
    std::unordered_map<int/*set_index*/, std::vector<CacheEntry>> cache_array_;
    void initialize_set_array(int set) {
      auto& set_vec = cache_array_[set];
      set_vec.reserve(num_ways_);
      for(auto entry : set_vec) {
        entry.valid_ = false;
        entry.data_ = -1;
        entry.tag_ = -1;
      }
      set_occupancy_[set] = 1; 
    }

    void add_occupancy(int set) {
      set_occupancy_[set]++;
    }

    int get_occupancy(int set) {
      return set_occupancy_[set];
    }
};

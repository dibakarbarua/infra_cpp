#include <algorithm>
#include <functional>
#include <list>
#include <optional>
#include <unordered_map>
#include <cassert>

/*
std::list() as a linked-list in Modern C++
The std::list::splice member function in C++ transfers nodes from one std::list to another (or within the same list) 
by rearranging the internal pointers of the linked list. This is a highly efficient operation because no elements 
are copied or moved in memory, and existing iterators or references to the elements remain valid.

dest.splice(pos, source);              // move all nodes
dest.splice(pos, source, it);          // move one node
dest.splice(pos, source, first, last); // move range [first, last)
*/

template <typename Key, typename Value>
class LruCache {
public:
    using EvictionCallback = std::function<void(const Key&, const Value&)>;

    explicit LruCache(std::size_t capacity)
        : capacity_(capacity)
    {
        assert(capacity != 0);
    }

    void set_eviction_callback(EvictionCallback callback) {
        on_evict_ = callback;
    }

    void put(Key key, Value value) {
        auto existing_map_entry = index_.find(key);
        
        // key does not exist, add entry
        if (existing_map_entry == index_.end()) {
            // evict if needed
            if (entries_.size() == capacity_) 
            {
                // delete least recently used entry
                auto deleted_entry = entries_.begin();
                auto deleted_key = deleted_entry->key;
                entries_.erase(deleted_entry);
                index_.erase(deleted_key);
            }
            entries_.emplace_back(Entry{key, value});
            index_[key] = std::prev(entries_.end());
        }
        // key exists, make most recently used
        else {
            entries_.splice(entries_.end(), entries_, index_[key]);
        }
    }

    std::optional<Value> get(const Key& key) {
        if (index_.contains(key)) {
            auto existing_entry = index_[key];
            auto value = existing_entry->value;
            entries_.splice(entries_.end(), entries_, existing_entry);
            return value;
        }
        else 
        {
            return std::nullopt;
        }
    }

    bool contains(const Key& key) const {
        return index_.contains(key);
    }

    std::size_t size() const {
        return entries_.size();
    }

private:
    struct Entry {
        Key key;
        Value value;
    };

    std::size_t capacity_ = 0;
    std::list<Entry> entries_;
    std::unordered_map<Key, typename std::list<Entry>::iterator> index_;
    EvictionCallback on_evict_;
};

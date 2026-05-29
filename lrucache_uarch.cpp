#include <cstdint>
#include <list>
#include <algorithm>
#include <bit>

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
      tag_mask_ = (uint32_t(1) << tag_bits) - 1;
      index_mask_ = (uint32_t(1) << index_bits) - 1;
      offset_mask_ = (uint32_t(1) << offset_bits_) - 1;

      // Sizing
      block_size_ = 1 << offset_bits;
      num_ways_ = 1 << index_bits;
      num_sets_ = cache_size_ / (num_ways * block_size);
    }
    
  // To be implemented
  virtual bool read_data(uint32_t address, int& data) override = 0;
  virtual bool tag_check(uint32_t address) override = 0;
  virtual bool write_data(uint32_t address, const int data) override = 0;
      
private:
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
  int set_idx(uint32_t address) {
    return (address >> offset_bits_) & index_mask_;

  int tag_idx(uint32_t address) {
    return (address >> (offset_bits_ + index_bits_)) & tag_mask_;
  }    
};

class LRUCacheOnList : public LRUCache {
  public:
    bool read_data(uint32_t address, int& data) final {
    }

    bool tag_check(uint32_t address) final {
    }

    bool write_data(uint32_t address, int& data) final {
    }

  private:
};

class LRUCacheOnVector : public LRUCache {
  public:
    bool read_data(uint32_t address, int& data) final {
    }

    bool tag_check(uint32_t address) final {
      int tag = tag_idx(address);
      int set = set_idx(address);

      auto& set_array = cache_array_[set];

      for(auto way : set_array) {
        if (way
      }
    }

    bool write_data(uint32_t address, const int data) final {
      int tag = tag_idx(address);
      int set = set_idx(address);

      std::pair<int,int> new_entry = std::make_pair<int,int>(tag,data);

      // First ever access to this set, add it to tag-map
      if (cache_array_.find(set) == cache_array_.end()) {
        cache_array_[set] = std::vector<int,int>(&new_entry);
        set_occupancy_[set] = 1;
      }

      // Get set array ptr
      auto& set_array = cache_array_[set];
      int& set_occupancy = set_occupancy_[set];

      // If there are empty ways, push to the front of the vector
      if (set_occupancy < num_ways_) {
        set_array.emplace_front(
      }
    }

  private:
    std::unordered_map<int/*set_index*/, std::vector<std::pair<int, int>/*tag, data*/>> cache_array_;
};

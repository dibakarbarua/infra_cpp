#include <cstdint>
#include <list>
#include <algorithm>
#include <bit>

class LRUCache
{
  public:
    LRUCache() {
      cache_size = 64 * 1024; // 64KB
      address_bits = 32;
      offset_bits = 8; // 64B
      index_bits = 4; // 16 ways

      // Addressing
      tag_bits = address_bits - (offset_bits + index_bits);
      tag_mask = (uint32_t(1) << tag_bits) - 1;
      index_mask = (uint32_t(1) << index_bits) - 1;
      offset_mask = (uint32_t(1) << offset_bits) - 1;

      // Sizing
      block_size = 1 << offset_bits;
      num_ways = 1 << index_bits;
      num_sets = cache_size / (num_ways * block_size);
    }

  LRUCache(int cache_size, int offset_bits, int index_bits) : 
    cache_size(cache_size), offset_bits(offset_bits), index_bits(index_bits) {
      address+bits = 32;
      
      // Addressing
      tag_bits = address_bits - (offset_bits + index_bits);
      tag_mask = (uint32_t(1) << tag_bits) - 1;
      index_mask = (uint32_t(1) << index_bits) - 1;
      offset_mask = (uint32_t(1) << offset_bits) - 1;

      // Sizing
      block_size = 1 << offset_bits;
      num_ways = 1 << index_bits;
      num_sets = cache_size / (num_ways * block_size);
    }
      
  private:
    // Addressing
    int address_bits;
    int tag_bits;
    int offset_bits;
    int index_bits;
    int offset_mask;
    int index_mask;
    int tag_mask;

    // Sizing
    int cache_size;
    int block_size;
    int num_sets;
    int num_ways;

    // To be implemented
    virtual bool read_data(uint32_t address, int& data) = 0;
    virtual bool tag_check(uint32_t address) = 0;
    virtual bool write_data(uint32_t address, const int data) = 0;

    // Helpers for address decoding
    int set_idx(uint32_t address) {
      return (address >> offset_bits) & index_mask;

    int tag_idx(uint32_t address) {
      return (address >> (offset_bits + index_bits)) & tag_mask;
    }
    
};

class LRUCacheOnList : public LRUCache {
};

class LRUCacheOnVector : public LRUCache {
};

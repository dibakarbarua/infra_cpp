#include <algorithm>
#include <optional>
#include <functional>
#include <optional>
#include <vector>
#include <queue>
#include <utility>

/*
Given a list of meeting time intervals, determine the minimum number of conference rooms required so that no meetings overlap.

Example:
Input:
intervals = [[0,30], [5,10], [15,20]]
Output:
2
Constraints:
1 <= intervals.length <= 10^4
0 <= start_i < end_i <= 10^6
Each interval represents a valid meeting time

Refer to Priority Queue notes on Notes.app for details....
*/

namespace utils {

template <typename T>
class PriorityQueue {
public:
    enum class HeapType {
        min = 0,
        max = 1,
    };
    PriorityQueue(HeapType type = HeapType::min) {
        type_ = type;
    }
    int size() const {
        return heap_store_.size();
    }
    void push(T entry) {
        heap_store_.emplace_back(entry);
        int curr_idx = heap_store_.size() - 1;
        while(curr_idx >= 1) {
            int parent_idx = (curr_idx - 1)/2;
            if (has_higher_priority(heap_store_[curr_idx], heap_store_[parent_idx])) {
                std::swap(heap_store_[curr_idx], heap_store_[parent_idx]);
                curr_idx = parent_idx;
            }
            else {
                // adjustments complete
                break;
            }
        }
    }
    std::optional<T> peek() const {
        if (heap_store_.size() == 0) {
            return std::nullopt;
        }
        return heap_store_[0];
    }
    std::optional<T> pop() {
        if (heap_store_.size() == 0) {
            return std::nullopt;
        }
        T ret_val = std::move(heap_store_[0]);
        heap_store_[0] = heap_store_[heap_store_.size() - 1];
        heap_store_.pop_back();
        int heap_size = heap_store_.size();
        if (heap_size == 0) {
            return ret_val;
        }

        int curr_idx = 0;
        while(curr_idx < heap_size) {
            T curr_val = heap_store_[curr_idx];
            int left_child_idx = 2 * curr_idx + 1;
            int right_child_idx = left_child_idx + 1;
            bool left_child_valid = left_child_idx < heap_size;
            bool right_child_valid = right_child_idx < heap_size;
            if (!left_child_valid) {
                return ret_val;
            }

            T left_child_val = heap_store_[left_child_idx];
            T compare_val = left_child_val;
            int compare_idx = left_child_idx;

            if (right_child_valid && has_higher_priority(heap_store_[right_child_idx], left_child_val)) {
                compare_val = heap_store_[right_child_idx];
                compare_idx = right_child_idx;
            }

            if (has_higher_priority(compare_val, curr_val)) {
                heap_store_[compare_idx] = curr_val;
                heap_store_[curr_idx] = compare_val;
                curr_idx = compare_idx;
            }
            else {
                return ret_val;
            }
        }
        return ret_val;
    }
private:
    bool has_higher_priority(const T& a, const T& b) const {
        if (type_ == HeapType::min) {
            return a < b;
        }
        return a > b;
    }

    HeapType type_;
    std::vector<T> heap_store_;
};

}

int min_queue_slots(std::vector<std::pair<int, int>>& events) {
    // sort events by start time
    std::sort(events.begin(), events.end(), 
        [](const std::pair<int, int> a, const std::pair<int, int> b){
            return a.first < b.first;
        });
    
    utils::PriorityQueue<int> min_heap(utils::PriorityQueue<int>::HeapType::min);
    for (const auto& event : events) {
        if (min_heap.size() == 0) {
            min_heap.push(event.second);
        }
        else if (auto earliest_end = min_heap.peek(); earliest_end.has_value() && earliest_end.value() <= event.first) {
            min_heap.pop();
            min_heap.push(event.second);
        }
        else {
            min_heap.push(event.second);
        }
    }
    return min_heap.size();
}

int std_pri_queue_min_queue_slots(std::vector<std::pair<int, int>>& events) {
    // sort events by start time
    std::sort(events.begin(), events.end(), 
        [](const std::pair<int, int> a, const std::pair<int, int> b){
            return a.first < b.first;
        });
    
    std::priority_queue<int, std::vector<int>, std::greater<int>> min_heap;
    for (const auto& event : events) {
        if (min_heap.size() == 0) {
            min_heap.push(event.second);
        }
        else if (min_heap.top() <= event.first) {
            min_heap.pop();
            min_heap.push(event.second);
        }
        else {
            min_heap.push(event.second);
        }
    }
    return min_heap.size();
}

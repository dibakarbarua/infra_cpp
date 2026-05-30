#include <array>
#include <queue>
#include <vector>

class TaskSchedulerSolution {
public:
    int leastInterval(const std::vector<char>& tasks, int n) {
        std::array<int, 26> frequencies{};
        for (char task : tasks) {
            ++frequencies[task - 'A'];
        }

        std::priority_queue<Task, std::vector<Task>, CompareTasks> available;
        for (int idx = 0; idx < static_cast<int>(frequencies.size()); ++idx) {
            if (frequencies[idx] > 0) {
                /* GoodC++: always construct in place inside container using emplace */
                available.emplace(static_cast<char>('A' + idx), frequencies[idx]);
            }
        }

        std::queue<Task> cooling_down;
        int cycle = 0;

        while (!available.empty() || !cooling_down.empty()) {
            ++cycle;

            if (!cooling_down.empty() && cooling_down.front().ready_at <= cycle) {
                /* GoodC++: move from the source element before removing it. */
                available.push(std::move(cooling_down.front()));
                cooling_down.pop();
            }

            if (!available.empty()) {
                Task current = available.top();
                available.pop();

                --current.remaining;
                if (current.remaining > 0) {
                    current.ready_at = cycle + n + 1;
                    cooling_down.push(std::move(current));
                }
            }
        }

        return cycle;
    }

private:
    struct Task {
        Task(char type, int remaining_count)
            : type(type), remaining(remaining_count), ready_at(0) {}

        char type;
        int remaining;
        int ready_at;
    };

    struct CompareTasks {
        bool operator()(const Task& lhs, const Task& rhs) const {
            return lhs.remaining < rhs.remaining;
        }
    };
};

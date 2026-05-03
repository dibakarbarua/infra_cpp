#include <algorithm>
#include <functional>
#include <string>
#include <vector>

enum class Severity {
    Debug,
    Info,
    Warning,
    Error
};

struct LogMessage {
    Severity severity;
    std::string subsystem;
    std::string text;
};

using Predicate = std::function<bool(const LogMessage&)>;

class LogRouter {
public:
    void add_route(std::string name, Predicate predicate) {
        routes_.emplace_back(Route{name, predicate});
    }

    /*
    We cannot use copy-if here since we are copying messages not routes.
    If we were copying routes it would look like:
        ```
        std::copy_if(
            routes_.begin(),
            routes_.end(),
            std::back_inserter(result),
            // Note the [&] and Route&
            // This makes sure we use the reference to the object and message ...
            // ... instead of making copies just for comparing
            // This would be common practice in any <>_if() style std functions
            [&](const Route& route) {
                return route.predicate(message);
            }
        );
        ```
    */
    std::vector<std::string> matching_routes(const LogMessage& message) const {
        std::vector<std::string> matching_routes;
        for (const auto& route : routes_) {
            if (route.predicate(message)) {
                matching_routes.push_back(route.name);
            }
        }
        return matching_routes;
    }

private:
    struct Route {
        std::string name;
        Predicate predicate;
    };

    std::vector<Route> routes_;
};

Predicate min_severity(Severity severity) {
    // If instead we used 
    //      return [&](const LogMessage& msg){ return msg.severity == severity;};
    // It is possible severity would be out-of-scope
    // The lambda function needs to copy severity as a local variable of the predicate function
    return [severity](const LogMessage& msg){ return msg.severity == severity;};
}

Predicate from_subsystem(std::string subsystem) {
    return [subsystem](const LogMessage& msg){ return msg.subsystem == subsystem;};
}

Predicate text_contains(std::string needle) {
    return [needle](const LogMessage& msg){ return msg.text.find(needle) != std::string::npos;};
}

Predicate all_of(std::vector<Predicate> predicates) {
    return [predicates](const LogMessage& msg){
        return std::all_of(
            predicates.begin(), 
            predicates.end(), 
            [&](const Predicate& p) {
                return p(msg);
            }
        );
    };
}

Predicate any_of(std::vector<Predicate> predicates) {
    // implement me using lambda
    return {};
}

Predicate none_of(Predicate predicate) {
    // implement me using lambda
    return {};
}

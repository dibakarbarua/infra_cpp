#include <iostream>
#include <memory>
#include <string>
#include <vector>

/*
    CPU is unhealthy if temperature is greater than 85.0.
    Disk is unhealthy if used space is greater than 90.0.
    DeviceRegistry owns all devices.
    Returned pointers are non-owning.
    Avoid object slicing.
*/

enum class DeviceType {
    Cpu,
    Disk,
    DeviceTypes
};

class Device {
public:
    virtual ~Device() = default;

    virtual std::string name() const = 0;
    virtual DeviceType type() const = 0;
    virtual bool healthy() const = 0;
};

class CpuDevice : public Device {
public:
    CpuDevice(std::string id, double temperature_celsius) :
        id_(id), temperature_celsius_(temperature_celsius)
    {}

    std::string name() const override {
        return id_;
    }

    DeviceType type() const override {
        return DeviceType::Cpu;
    }

    bool healthy() const override {
        return (temperature_celsius_ <= 85);
    }

private:
    std::string id_;
    double temperature_celsius_ = 0.0;
};

class DiskDevice : public Device {
public:
    DiskDevice(std::string mount_point, double used_percent) :
        mount_point_(mount_point), used_percent_(used_percent)
    {}

    std::string name() const override {
        return mount_point_;
    }

    DeviceType type() const override {
        return DeviceType::Disk;
    }

    bool healthy() const override {
        return used_percent_ <= 90;
    }

private:
    std::string mount_point_;
    double used_percent_ = 0.0;
};

class DeviceRegistry {
public:
    void add(std::unique_ptr<Device> device) {
        devices_.emplace_back(std::move(device));
    }

    /*
    <> Key Learnings:
    ```
    Use a reference based loop for unique_ptr objects
        for (const auto& ptr : things) // inspect objects
        for (auto& ptr : things)       // modify objects or maybe reset/move pointers
        for (auto ptr : things) // error: tries to copy unique_ptr
    
    Difference between raw ptr and shared ptr
        std::vector<Thing*>              // borrowed pointers, no lifetime extension
        std::vector<std::shared_ptr<Thing>> // shared ownership, keeps Things alive
    
    Derefencing a borrowed unique_ptr
        1. Iterator is always by reference
            decltype(it) = std::vector<unique_ptr>::iterator&
        2. If we dereference iterator we get a reference to unique_ptr
            std::unique_ptr<Thing>& ptr = *it;
        3. If we dereference the unique_ptr reference we get a reference to object
            Thing& found = *ptr;
        NOTE: Because this is a reference to object, ownership is not transferred.

    ```
    */

    std::vector<const Device*> unhealthy_devices() const {
        std::vector<const Device*> unhealthy_devices;
        // decltype(device) = std::unique_ptr<Device>& (reference to unique_ptr)
        /*
            We could have used std::find_if() if the vector did not contain unique_ptr
                ```
                std::vector<Thing*> result;
                std::copy_if(things.begin(), things.end(), std::back_inserter(result),
                    [](const std::unique_ptr<Thing>& ptr) {
                        return ptr && ptr->isReady();
                    });
                ```
            Here, we will try to insert unique_ptr into std::vector<Thing*> which is incorrect
            std::vector<Thing&> is INVALID in C++!

        */
        for (const auto& device : devices_) {
            if (!device->healthy()) { // upcasting
                // Note that here we are copying the pointer not the object!
                unhealthy_devices.emplace_back(device.get());
            }
        }
        return unhealthy_devices;
    }

    std::vector<const Device*> devices_by_type(DeviceType type) const {
        std::vector<const Device*> devices_of_type;
        for (const auto& device : devices_) {
            if (device->type() == type) {
                devices_of_type.emplace_back(device.get());
            }
        }
        return devices_of_type;
    }

private:
    std::vector<std::unique_ptr<Device>> devices_;
};

#include <string>
#include <optional>

int fake_open(const std::string& path);
void fake_close(int fd);
bool fake_write(int fd, const std::string& data);
bool fake_is_open(int fd);

/*
    - Destructor closes the descriptor if open.
    - Move transfers ownership.
    - Moved-from handle should have fd_ == -1.
    - close() should be safe to call more than once.
    - write() should return false if closed.
*/

class FileHandle {
public:
    FileHandle() = default;

    explicit FileHandle(std::string path) {
        // implement me
        // 1. open a handle to the file at this path
        // 2. assert if path is invalid
        // 3. store handle in fd_
        fd_ = fake_open(path);
        if (fd_ == -1) {
            throw std::runtime_error("Failed to open file: " + path);
        }

    }

    // copy constructor is invalid (race condition)
    FileHandle(const FileHandle&) = delete;
    
    // equals operator is also invalid (race condition)
    FileHandle& operator=(const FileHandle&) = delete;

    // pointer migration from other to this
    FileHandle(FileHandle&& other) noexcept {
        fd_ = other.fd();
        other.close();
    }

    FileHandle& operator=(FileHandle&& other) noexcept {
        close();
        fd_ = other.fd();
        other.close();
        return *this;
    }

    ~FileHandle() {
        fake_close(fd);
    }

    bool write(const std::string& data) {
        assert(is_open());
        fake_write(fd_, data);
    }

    bool is_open() const {
        return fd_ != -1;
    }

    int fd() const {
        return fd_;
    }

    void close() {
        fake_close(fd);
        fd = -1;
    }

    static std::optional<FileHandle> open(std::string path) {
        assert (fd_ != -1;)
        fake_open(fd_, path);
    }

private:
    int fd_ = -1;
};

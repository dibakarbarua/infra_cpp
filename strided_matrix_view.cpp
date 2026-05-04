#include <cstddef>
#include <span>

/*
    - `stride` is the number of elements between starts of adjacent rows.
    - `submatrix()` must not copy data.
    - Validate that the backing span is large enough.
    - Throw on invalid indexing or invalid construction.
*/

/* 
---- std::span ----
std::span is a lightweight, non-owning view over a contiguous sequence of objects.

Think: “a pointer plus a length, packaged safely.”

It does not own memory. It does not allocate. 
It just refers to existing data such as an array, std::vector, std::array, or part of one.

When It Is Useful

Use std::span when a function needs to look at or modify a sequence of contiguous elements, 
but should not own that sequence.

std::vector<int> values = {10, 20, 30, 40, 50};

std::span<int> view(values);

auto first_three = view.first(3);
auto middle = view.subspan(1, 3); // 20, 30, 40
auto last_two = view.last(2);

*/

template <typename T>
class MatrixView {
public:
    MatrixView(std::span<T> data,
               std::size_t rows,
               std::size_t cols,
               std::size_t stride)
        : data_(data), rows_(rows), cols_(cols), stride_(stride)
    {}

    T& operator()(std::size_t row, std::size_t col) {
        // protect against illegal access
        return data_[row * stride_ + col];
    }

    const T& operator()(std::size_t row, std::size_t col) const {
        return data_[row * stride_ + col];
    }

    std::size_t rows() const {
        return rows_;
    }

    std::size_t cols() const {
        return cols_;
    }

    std::size_t stride() const {
        return stride_;
    }

    MatrixView<T> submatrix(std::size_t row,
                            std::size_t col,
                            std::size_t rows,
                            std::size_t cols) const {
        // Note:
        // This is a neat trick to return a non contiguous view of a matrix
        return MatrixView<T>{
            data_.subspan(
                row * stride_ + col,
                // note that this span is actually larger than what the sub-matrix view can see
                (rows - 1) * stride_ + col,
            ), 
            rows, 
            cols, 
            stride_};
    }

private:
    std::span<T> data_;
    std::size_t rows_ = 0;
    std::size_t cols_ = 0;
    std::size_t stride_ = 0;
};
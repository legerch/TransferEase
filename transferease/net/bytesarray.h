#ifndef TEASE_NET_BYTESARRAY_H
#define TEASE_NET_BYTESARRAY_H

#include <memory>
#include <vector>

namespace tease
{

class BytesArray final
{

public:
    using Byte = uint8_t;

private:
    using Container = std::vector<Byte>; /* As long as this type is not changed, ABI will be preserved */

public:
    using iterator = typename Container::iterator;
    using const_iterator = typename Container::const_iterator;
    using reverse_iterator = typename Container::reverse_iterator;
    using const_reverse_iterator = typename Container::const_reverse_iterator;

public:
    BytesArray();
    explicit BytesArray(size_t size);
    explicit BytesArray(size_t size, Byte value);
    explicit BytesArray(const std::initializer_list<Byte> &args);

    explicit BytesArray(const BytesArray &other);
    explicit BytesArray(BytesArray &&other) noexcept;

    ~BytesArray();

public:
    bool isEmpty() const;
    std::size_t getSize() const;
    std::size_t getMaxSize() const;

    const Byte& at(size_t index) const;

public:
    void resize(size_t size);

    void pushBack(Byte value);
    void popBack();

    Byte* data();
    const Byte* dataConst() const;

public:
    void clear();

public:
    iterator begin();
    const_iterator cbegin() const;
    reverse_iterator rbegin();
    const_reverse_iterator crbegin() const;
    iterator end();
    const_iterator cend() const;
    reverse_iterator rend();
    const_reverse_iterator crend() const;

public:
    Byte& operator[](size_t index);
    const Byte& operator[](size_t index) const;

private:
    class Impl;
    std::unique_ptr<Impl> d_ptr;
};

} // namespace tease

#endif // TEASE_NET_BYTESARRAY_H

#include "bytesarray.h"

/*****************************/
/* Class documentations      */
/*****************************/

/*!
 * \class tease::BytesArray
 * \brief To complete
 */

/*****************************/
/* Macro definitions         */
/*****************************/

/*****************************/
/* Start namespace           */
/*****************************/

namespace tease
{

/*****************************/
/* Functions definitions     */
/*      Private Class        */
/*****************************/

class BytesArray::Impl final
{
public:
    Impl() = default;
    explicit Impl(size_t size);
    explicit Impl(size_t size, Byte value);
    explicit Impl(const std::initializer_list<Byte> &args);

    explicit Impl(const Impl &other);

public:
    std::vector<uint8_t> m_buffer;
};

/*****************************/
/* Functions implementation  */
/*      Private Class        */
/*****************************/

BytesArray::Impl::Impl(size_t size)
    : m_buffer(size){}

BytesArray::Impl::Impl(size_t size, Byte value)
    : m_buffer(size, value){}

BytesArray::Impl::Impl(const std::initializer_list<Byte> &args)
    : m_buffer(args){}

BytesArray::Impl::Impl(const Impl &other)
    : m_buffer(other.m_buffer){}

/*****************************/
/* Functions implementation  */
/*      Public Class         */
/*****************************/

BytesArray::BytesArray()
    : d_ptr(std::make_unique<Impl>()){}

BytesArray::BytesArray(size_t size)
    : d_ptr(std::make_unique<Impl>(size)){}

BytesArray::BytesArray(size_t size, Byte value)
    : d_ptr(std::make_unique<Impl>(size, value)){}

BytesArray::BytesArray(const std::initializer_list<Byte> &args)
    : d_ptr(std::make_unique<Impl>(args)){}

BytesArray::BytesArray(const BytesArray &other)
    : d_ptr(other.d_ptr ? std::make_unique<Impl>(*other.d_ptr) : nullptr){}

BytesArray::BytesArray(BytesArray &&other) noexcept
    : d_ptr(std::move(other.d_ptr)){}

bool BytesArray::isEmpty() const
{
    return d_ptr->m_buffer.empty();
}

std::size_t BytesArray::getSize() const
{
    return d_ptr->m_buffer.size();
}

std::size_t BytesArray::getMaxSize() const
{
    return d_ptr->m_buffer.max_size();
}

BytesArray::iterator BytesArray::begin()
{
    return d_ptr->m_buffer.begin();
}

BytesArray::const_iterator BytesArray::cbegin() const
{
    return d_ptr->m_buffer.cbegin();
}

BytesArray::reverse_iterator BytesArray::rbegin()
{
    return d_ptr->m_buffer.rbegin();
}

BytesArray::const_reverse_iterator BytesArray::crbegin() const
{
    return d_ptr->m_buffer.crbegin();
}

BytesArray::iterator BytesArray::end()
{
    return d_ptr->m_buffer.end();
}

BytesArray::const_iterator BytesArray::cend() const
{
    return d_ptr->m_buffer.cend();
}

BytesArray::reverse_iterator BytesArray::rend()
{
    return d_ptr->m_buffer.rend();
}

BytesArray::const_reverse_iterator BytesArray::crend() const
{
    return d_ptr->m_buffer.crend();
}

/*****************************/
/* Constants definitions     */
/*****************************/

/*****************************/
/* End namespace             */
/*****************************/

} // namespace tease

/*****************************/
/* End file                  */
/*****************************/

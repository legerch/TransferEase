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
    Container m_buffer;
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

BytesArray::~BytesArray() = default;

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

const BytesArray::Byte &BytesArray::at(size_t index) const
{
    return d_ptr->m_buffer.at(index);
}

std::string BytesArray::toString() const
{
    return std::string(d_ptr->m_buffer.cbegin(), d_ptr->m_buffer.cend());
}

/*!
 * \brief Increase capacity of bytes array.
 *
 * \param[in] size
 * Total number of elements that the bytes array
 * can hold without requiring reallocation. \n
 * If \c size is greater than \c getMaxSize(), new storage
 * is allocated, otherwise the function does nothing (so
 * no shrinking).
 *
 * \sa resize()
 * \sa getMaxSize()
 */
void BytesArray::reserve(size_t size)
{
    d_ptr->m_buffer.reserve(size);
}

void BytesArray::resize(size_t size, Byte value)
{
    d_ptr->m_buffer.resize(size, value);
}

/*!
 * \brief Append a value to the back of bytes array
 *
 * \param[in] value
 * Value to add
 */
void BytesArray::pushBack(Byte value)
{
    d_ptr->m_buffer.push_back(value);
}

/*!
 * \overload
 * \param[in] strView
 * Append string to bytes array.
 *
 * \sa setFromString()
 */
void BytesArray::pushBack(std::string_view strView)
{
    /* Limit allocations by pre-allocating it */
    reserve(d_ptr->m_buffer.size() + strView.size());

    /* Copy string characters to buffer */
    d_ptr->m_buffer.insert(d_ptr->m_buffer.end(), strView.cbegin(), strView.cend());
}

void BytesArray::popBack()
{
    d_ptr->m_buffer.pop_back();
}

/*!
 * \brief Allow to set bytes array from a string
 * \details
 * This method will simply clear() any previous data
 * and append string bytes via pushBack() method.
 *
 * \param[in] strView
 * String to use to set bytes array
 *
 * \sa clear(), pushBack()
 */
void BytesArray::setFromString(std::string_view strView)
{
    clear();
    pushBack(strView);
}

BytesArray::Byte *BytesArray::data()
{
    return d_ptr->m_buffer.data();
}

const BytesArray::Byte *BytesArray::dataConst() const
{
    return d_ptr->m_buffer.data();
}

void BytesArray::clear()
{
    d_ptr->m_buffer.clear();
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

BytesArray::Byte& BytesArray::operator[](size_t index)
{
    return d_ptr->m_buffer[index];
}

const BytesArray::Byte& BytesArray::operator[](size_t index) const
{
    return d_ptr->m_buffer[index];
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

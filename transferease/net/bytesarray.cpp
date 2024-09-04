#include "bytesarray.h"

#include "logs/logmanager.h"
#include "tools/filesystemhelper.h"
#include "tools/stringhelper.h"

#include <fstream>

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
    : d_ptr(std::make_unique<Impl>(*other.d_ptr)){}

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
 * \brief Allow to export bytes array data to a file
 *
 * \param[in] pathFile
 * Path to file to create. \n
 * If file doesn't exists, method will automatically create
 * it (and build needed folders). \n
 * If file already exists, it will be truncated before
 * writing to it.
 *
 * \return
 * Returns \c true if succeed.
 *
 * \sa setFromFile()
 * \sa toString()
 */
bool BytesArray::toFile(const std::string &pathFile)
{
    /* Prepare output file */
    FileSystemHelper::createDirectories(FileSystemHelper::getFilePathDir(pathFile));
    std::ofstream outFile(pathFile, std::ios::out | std::ios::trunc | std::ios::binary);
    if(!outFile){
        const std::string err = StringHelper::format("Failed to create file [path: %s]", pathFile.c_str());
        TEASE_LOG_ERROR(err);
        return false;
    }

    /* Write bytes to file */
    outFile.write(reinterpret_cast<const char*>(d_ptr->m_buffer.data()), d_ptr->m_buffer.size());
    if(!outFile){
        const std::string err = StringHelper::format("Failed to write to file [path: %s]", pathFile.c_str());
        TEASE_LOG_ERROR(err);
        return false;
    }

    return true;
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
 * \sa setFromFile()
 */
void BytesArray::setFromString(std::string_view strView)
{
    clear();
    pushBack(strView);
}

/*!
 * \brief Allow to set bytes array from file content
 * \details
 * This method will clear any previous data and load
 * file content data into the bytes array.
 *
 * \param[in] pathFile
 * Path to file to load. \n
 * If file doesn't exist, method will returns \c false
 *
 * \warning
 * Be careful with loaded file, current implementation
 * will allocate same size memory than the loaded file. \n
 * Just to prevent any issues, 1Gb limit is currently in place.
 *
 * \return
 * Returns \c true if succeed.
 *
 * \sa toFile()
 * \sa setFromString()
 */
bool BytesArray::setFromFile(const std::string &pathFile)
{
    static constexpr size_t sizeLimit = 1024 * 1024 * 1024; // Represent 1Gb

    /* Clear any previous data */
    clear();

    /* Open file */
    std::ifstream inFile(pathFile, std::ios::in | std::ios::binary);
    if(!inFile){
        const std::string err = StringHelper::format("Failed to load file [path: %s]", pathFile.c_str());
        TEASE_LOG_ERROR(err);
        return false;
    }

    /* Determine file size */
    inFile.seekg(0, std::ios::end);
    const std::streamsize size = inFile.tellg();
    inFile.seekg(0, std::ios::beg);

    /* Resize the internal buffer to hold file datas */
    if(size > sizeLimit){
        const std::string err = StringHelper::format("Read file is superior to current limit, no loading performed [path: %s, size-file: %zu, size-max: %zu]", pathFile.c_str(), size, sizeLimit);
        TEASE_LOG_ERROR(err);
        return false;
    }
    d_ptr->m_buffer.resize(size);

    /* Read file datas into the buffer */
    inFile.read(reinterpret_cast<char*>(d_ptr->m_buffer.data()), size);
    if(!inFile){
        const std::string err = StringHelper::format("Failed to read data from file [path: %s, nb-bytes-read: %zd]", pathFile.c_str(), inFile.gcount());
        TEASE_LOG_ERROR(err);
        return false;
    }

    return true;
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

BytesArray& BytesArray::operator=(const BytesArray &other)
{
    /* Verify that value actually differs */
    if(this == &other){
        return *this;
    }

    /* Perform copy assignment */
    d_ptr = std::make_unique<Impl>(*other.d_ptr);
    return *this;
}

BytesArray& BytesArray::operator=(BytesArray &&other) noexcept
{
    d_ptr = std::move(other.d_ptr);
    return *this;
}

bool operator==(const BytesArray &left, const BytesArray &right)
{
    return left.d_ptr->m_buffer == right.d_ptr->m_buffer;
}

bool operator!=(const BytesArray &left, const BytesArray &right)
{
    return !(left == right);
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

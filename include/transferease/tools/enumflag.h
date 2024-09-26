#ifndef TEASE_TOOLS_ENUMFLAG_H
#define TEASE_TOOLS_ENUMFLAG_H

#include <unordered_map>
#include <sstream>
#include <string>
#include <type_traits>

template<typename E>
struct enable_bitmask_operators
{
    static constexpr bool enable = false; /* Define SFINAE */
};

template<typename E>
typename std::enable_if<enable_bitmask_operators<E>::enable,E>::type
operator|(E lhs,E rhs)
{
    typedef typename std::underlying_type<E>::type underlying;
    return static_cast<E>(
        static_cast<underlying>(lhs) | static_cast<underlying>(rhs));
}

template<typename E>
typename std::enable_if<enable_bitmask_operators<E>::enable,bool>::type
operator&(E lhs,E rhs)
{
    typedef typename std::underlying_type<E>::type underlying;
    return static_cast<bool>(
        static_cast<underlying>(lhs) & static_cast<underlying>(rhs));
}

template<typename E>
typename std::enable_if<enable_bitmask_operators<E>::enable,E>::type
operator^(E lhs,E rhs)
{
    typedef typename std::underlying_type<E>::type underlying;
    return static_cast<E>(
        static_cast<underlying>(lhs) ^ static_cast<underlying>(rhs));
}

template<typename E>
typename std::enable_if<enable_bitmask_operators<E>::enable,E>::type
operator~(E lhs)
{
    typedef typename std::underlying_type<E>::type underlying;
    return static_cast<E>(
        ~static_cast<underlying>(lhs));
}

template<typename E>
typename std::enable_if<enable_bitmask_operators<E>::enable,E&>::type
operator|=(E& lhs,E rhs)
{
    typedef typename std::underlying_type<E>::type underlying;
    lhs=static_cast<E>(
        static_cast<underlying>(lhs) | static_cast<underlying>(rhs));
    return lhs;
}

template<typename E>
typename std::enable_if<enable_bitmask_operators<E>::enable,E&>::type
operator&=(E& lhs,E rhs)
{
    typedef typename std::underlying_type<E>::type underlying;
    lhs=static_cast<E>(
        static_cast<underlying>(lhs) & static_cast<underlying>(rhs));
    return lhs;
}

template<typename E>
typename std::enable_if<enable_bitmask_operators<E>::enable,E&>::type
operator^=(E& lhs,E rhs)
{
    typedef typename std::underlying_type<E>::type underlying;
    lhs=static_cast<E>(
        static_cast<underlying>(lhs) ^ static_cast<underlying>(rhs));
    return lhs;
}

template<typename E>
typename std::enable_if<enable_bitmask_operators<E>::enable,std::string>::type
flagEnumToString(E enumFlag, const std::unordered_map<E, std::string> &mapFlagToStr, const char separator = '|')
{
    typedef typename std::underlying_type<E>::type underlying;
    E noFlag = static_cast<E>(0);

    /* Verify than flag is set */
    if(enumFlag == noFlag){
        return mapFlagToStr.at(noFlag);
    }

    /* Add flags to string */
    std::ostringstream oss;
    underlying primaryFlag = static_cast<underlying>(enumFlag);
    underlying indexFlag = 0x1;
    while(primaryFlag){
        E singleFlag = static_cast<E>(primaryFlag & indexFlag);
        if(singleFlag != noFlag){
            oss << mapFlagToStr.at(static_cast<E>(primaryFlag & indexFlag)) << separator; // String related to current index flag
        }

        primaryFlag &= ~indexFlag; // Remove already treated flag
        indexFlag = indexFlag << 1; // Go to next flag
    }

    /* Remove uneeded last separator character */
    std::string str = oss.str();
    if(str.back() == separator){
        str.pop_back();
    }

    return str;
}

#endif // TEASE_TOOLS_ENUMFLAG_H

#ifndef TEASE_GLOBAL_H
#define TEASE_GLOBAL_H

#include "config.h"

/**********************************
 * Library management
 *********************************/
#ifdef __GNUC__
#   if defined(TEASE_LIBRARY_BUILD)
#       define TEASE_EXPORT __attribute__((visibility("default")))
#   else
#       define TEASE_EXPORT
#   endif
#else
#   if defined(TEASE_LIBRARY_BUILD)
#       define TEASE_EXPORT __declspec(dllexport)
#   else
#       define TEASE_EXPORT __declspec(dllimport)
#   endif
#endif

/**********************************
 * Deprecations warnings
 *********************************/
#define TEASE_DEPREC              [[deprecated]]          /**< Use to mark a method as deprecated. \n\code{.cpp}TEASE_DEPREC void myOldFct(); \endcode */
#define TEASE_DEPREC_X(reason)    [[deprecated(reason)]] 	/**< Use to mark a method as deprecated and specify a reason. \n\code{.cpp}TEASE_DEPREC_X("Use myNewFct() instead") void myOldFunc(); \endcode */

/**********************************
 * Custom macros used to detect custom
 * built-in functions
 * Sources:
 * - MSVC: No equivalent
 * - GCC: https://gcc.gnu.org/onlinedocs/gcc-13.2.0/cpp/_005f_005fhas_005fbuiltin.html
 * - Clang: https://clang.llvm.org/docs/LanguageExtensions.html#has-builtin
 *********************************/
#if defined(__GNUC__) || defined(__clang__)
#   define TEASE_BUILTIN(x)  __has_builtin(x)
#else
#   define TEASE_BUILTIN(x)  0
#endif

/**********************************
 * Custom macros in order to
 * not trigger warning on expected
 * behaviour
 *********************************/
#define TEASE_FALLTHROUGH  [[fallthrough]]    /**< Indicates that the fall through from the previous case label is intentional and should not be diagnosed by a compiler that warns on fallthrough */

/**********************************
 * Context informations
 *********************************/
#define TEASE_FILE            __FILE__
#define TEASE_LINE            __LINE__
#define TEASE_FCTNAME         __func__

#if defined(_MSC_VER)
#define TEASE_FCTSIG          __FUNCSIG__
#else
#define TEASE_FCTSIG          __PRETTY_FUNCTION__
#endif

/**********************************
 * Classes behaviours
 *********************************/
#define TEASE_DISABLE_COPY(Class) \
    Class(const Class &) = delete;\
    Class &operator=(const Class &) = delete;

#define TEASE_DISABLE_MOVE(Class) \
    Class(Class &&) = delete; \
    Class &operator=(Class &&) = delete;

#define TEASE_DISABLE_COPY_MOVE(Class) \
    TEASE_DISABLE_COPY(Class) \
    TEASE_DISABLE_MOVE(Class)

#endif // TEASE_GLOBAL_H
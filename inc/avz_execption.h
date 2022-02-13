/*
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2021-11-15 11:06:42
 * @Description:
 */

#ifndef __AVZ_EXCEPTION_H__
#define __AVZ_EXCEPTION_H__
#include <exception>
#include <string>
namespace AvZ {
class Exception : public std::exception { // copy from https://www.itranslater.com/qa/details/2582506091615618048
public:
    /** Constructor (C strings).
     *  @param message C-style string error message.
     *                 The string contents are copied upon construction.
     *                 Hence, responsibility for deleting the char* lies
     *                 with the caller.
     */
    explicit Exception(const char* message)
        : msg_(message)
    {
    }

    /** Constructor (C++ STL strings).
     *  @param message The error message.
     */
    explicit Exception(const std::string& message)
        : msg_(message)
    {
    }

    /** Destructor.
     * Virtual to allow for subclassing.
     */
    virtual ~Exception() throw() { }

    /** Returns a pointer to the (constant) error description.
     *  @return A pointer to a const char*. The underlying memory
     *          is in posession of the Exception object. Callers must
     *          not attempt to free the memory.
     */
    virtual const char* what() const throw()
    {
        return msg_.c_str();
    }

protected:
    /** Error message.
     */
    std::string msg_;
};
}

#endif
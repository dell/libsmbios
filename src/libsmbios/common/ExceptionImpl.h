// vim:expandtab:autoindent:tabstop=4:shiftwidth=4:filetype=c:
/*
 * Copyright (C) 2005 Dell Inc.
 *  by Michael Brown <Michael_E_Brown@dell.com>
 * Licensed under the Open Software License version 2.1
 *
 * Alternatively, you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License,
 * or (at your option) any later version.

 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 */


#ifndef EXCEPTIONIMPL_H
#define EXCEPTIONIMPL_H



// compat header should always be first header
#include "smbios/compat.h"

#include <map>
#include <string>
#include <sstream>
#include <exception>

// types.h should be first user-defined header.
#include "smbios/types.h"
#include "smbios/IException.h"

// abi_prefix should be last header included before declarations
#include "smbios/config/abi_prefix.hpp"

#define DEFINE_EXCEPTION_EX( excName, ns, superclass )   \
    class excName : public smbios::Exception< ns :: superclass >  \
    {                                       \
    public:                                 \
        ~excName() throw() {};  \
        excName( const std::string initMessage ) : smbios::Exception< ns :: superclass >(initMessage) {} ;\
        excName( const excName &src ) : smbios::Exception< ns :: superclass >(src) {} ;\
        excName( ) : smbios::Exception< ns :: superclass >() {} ;\
    }

    /*commented out fragment above is valid code but hits a bug in MSVC++ and emits C2437 diagnostic erroneously*/

// Macro for convenient exception throwing
// (includes message, file, line #)
#define THROW(Type, txt) \
    throw Type ( std::string( __FILE__ ## ":Line " ## LIBSMBIOS_STRINGIZE(__LINE__)) + txt)

namespace smbios
{
    // TODO: all this needs to be hidden to preserve ABI.
    template <class S>
    class Exception : public S
    {
    public:
        // destructor
        virtual ~Exception() throw() {};
        // constructors
        Exception( const std::string initMessage ) : S(), messageStr( initMessage ), outputStr("") {};
        Exception( ) : S(), messageStr( "" ), outputStr("")  {};
        Exception( const Exception<S> &source );
        // overloaded assignment
        Exception<S> &operator =( const Exception<S> &source );

        virtual const char *what() const throw() ;
        virtual std::string getParameterString( const std::string &name ) const;
        virtual u32 getParameterNumber( const std::string &name ) const;

        virtual void setMessageString( const std::string &newMsgString );
        virtual void setParameter( const std::string &name, const std::string &value );
        virtual void setParameter( const std::string &name, const u32 value );
    private:
        std::string messageStr;
        mutable std::string outputStr;

        std::map< std::string, std::string > r_ptrStrMap;
        std::map< std::string, u32 > r_ptrNumMap;
    };

    // copy constructor
    template <class S>
    Exception<S>::Exception( const Exception<S> &source )
            : S(), messageStr( source.messageStr ), outputStr("")
    {
        // copy parameters over from source
        std::map< std::string, u32 >::const_iterator iter = source.r_ptrNumMap.begin();
        while ( iter != source.r_ptrNumMap.end() )
        {
            setParameter( iter->first, iter->second );
            ++iter;
        }

        std::map< std::string, std::string >::const_iterator iterStr = source.r_ptrStrMap.begin();
        while ( iterStr != source.r_ptrStrMap.end() )
        {
            setParameter( iterStr->first, iterStr->second );
            ++iterStr;
        }
    }

    // overloaded assignment
    template <class S>
    Exception<S> &Exception<S>::operator =( const Exception<S> &source )
    {
        if (this == &source)
            return *this;

        messageStr = source.messageStr;
        outputStr = "";

        // copy parameters over from source
        std::map< std::string, u32 >::const_iterator iter = source.r_ptrNumMap.begin();
        while ( iter != source.r_ptrNumMap.end() )
        {
            setParameter( iter->first, iter->second );
            ++iter;
        }

        std::map< std::string, std::string >::const_iterator iterStr = source.r_ptrStrMap.begin();
        while ( iterStr != source.r_ptrStrMap.end() )
        {
            setParameter( iterStr->first, iterStr->second );
            ++iterStr;
        }

        return *this;
    }

    template <class S>
    const char * Exception<S>::what() const throw()
    {
        outputStr = messageStr;

        size_t strLen = outputStr.length();
        size_t pos = 0;
        while(pos < strLen)
        {
            std::string varName = "";
            size_t replaceLen = 0;
            char varType;
            size_t endVar = 0;
            std::ostringstream rep;

            pos = outputStr.find( "%", pos );
            if( pos >= strLen )  // no more occurences
                break;

            // handle '%' as last character
            if( pos == strLen-1 )
                break;  // this is an error. maybe throw?

            // handle %%
            if( outputStr[pos+1] == '%' )
            {
                outputStr.replace(pos, 2, "%");
                goto next_pos;
            }

            if( outputStr[pos+1] != '(' )
            {
                // somebody lost their mind. Ignore them
                //   Only legal sequences with '%' are: "%%" and "%("
                goto next_pos;  // this is an error. maybe throw?
            }

            endVar = outputStr.find( ")", pos );
            if( endVar >= strLen )
            {
                // again with the crazy people.
                goto next_pos;  // this is an error. maybe throw?
            }

            // handle "%(xXx)" at end with no var type.
            if( endVar == strLen-1 )
                break;  // this is an error. maybe throw?

            varType = outputStr[endVar + 1];

            replaceLen = endVar - pos + 2;
            varName = outputStr.substr( pos+2, replaceLen - 4 );

            // limit vars to 32 chars (limit accidental runaway vars.)
            if( replaceLen > 32 )
                goto next_pos;

            switch( varType )
            {
            case 'i':
                rep <<  getParameterNumber(varName);
                outputStr.replace( pos, replaceLen, rep.str() );
                goto loop_end; // go back to start of while(), lets us recursively substitute
            case 's':
                outputStr.replace( pos, replaceLen, getParameterString(varName));
                goto loop_end; // go back to start of while(), lets us recursively substitute
            }

next_pos:
            ++pos;

loop_end:
            strLen = outputStr.length(); // in case string changed
        }

        return outputStr.c_str();
    }

    template <class S>
    void Exception<S>::setMessageString( const std::string &newStr )
    {
        messageStr = newStr;
    }


    template <class S>
    void Exception<S>::setParameter( const std::string &name, const std::string &value)
    {
        r_ptrStrMap[ name ] = value;
    }

    template <class S>
    void Exception<S>::setParameter( const std::string &name, const u32 value)
    {
        r_ptrNumMap[ name ] = value;
    }


    template <class S>
    u32 Exception<S>::getParameterNumber( const std::string &name ) const
    {
        std::map< std::string, u32 >::const_iterator iterStr = r_ptrNumMap.find(name);
        return iterStr->second;
    }

    template <class S>
    std::string Exception<S>::getParameterString( const std::string &name ) const
    {
        std::map< std::string, std::string >::const_iterator iterStr = r_ptrStrMap.find(name);
        return iterStr->second;
    }



    // some standard exceptions

    /** Raised when some class does not implement part of the public interface
      Used mainly in classes where there are optional parts of the interface
      defined that require extra external functionality, such as XML, for example.
      */
    DEFINE_EXCEPTION_EX( NotImplementedImpl, smbios, NotImplemented );

    /** Used in cases where something that "cannot happen" happens. Raised in
     * instances usually caused by some internal class state becoming
     * corrupted.
     */
    DEFINE_EXCEPTION_EX( InternalErrorImpl, smbios, InternalError );

    /** Used in cases where operating system privleges prevent an action. */
    DEFINE_EXCEPTION_EX( PermissionExceptionImpl, smbios, PermissionException );
}

// always should be last thing in header file
#include "smbios/config/abi_suffix.hpp"

#endif /* EXCEPTIONIMPL_H */

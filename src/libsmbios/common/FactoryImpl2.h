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


#ifndef FACTORY2_H_
#define FACTORY2_H_

#define LIBSMBIOS_SOURCE
#include "smbios/compat.h"

#include <map>

#include "smbios/types.h"

// abi_prefix should be last header included before declarations
#include "smbios/config/abi_prefix.hpp"

namespace factory
{

    template <class S>
    class TFactory : public S
    {
    public:
        virtual ~TFactory() throw()
        {
            if( _instance )
            {
                TFactory<S> *savedInstance = _instance;
                _instance = 0;
                delete savedInstance;
            }
            _instance = 0;
        };

        // default parameter foo=0 to workaround vc6 ICE when trying
        // to compile an explicit template function initialization
        template <class R>
        static TFactory<S> *getFactory(R *foo= 0)
        {
            UNREFERENCED_PARAMETER(foo);
            if( _instance == 0 )
            {
                _instance = new R();
            }
            return _instance;
        }

        virtual void reset()
        {
            if( _instance )
            {
                TFactory<S> *savedInstance = _instance;
                _instance = 0;
                delete savedInstance;
            }
            _instance = 0;
        };

        // Set Methods
        virtual void setParameter( const std::string name, const std::string value) { strParamMap[ name ] = value; };
        virtual void setParameter( const std::string name, const u32 value) { numParamMap[ name ] = value; };
        virtual void setMode( const int newMode ) { mode = newMode; };

        // CONST Methods (get)
        virtual std::string getParameterString( const std::string name ) const { return strParamMap[ name ]; };
        virtual u32 getParameterNum( const std::string name ) const {return numParamMap[ name ];};
        virtual int getMode( ) const { return mode ; };

    protected:
        TFactory() : S(), mode(0) {};
        TFactory( const TFactory<S> &src );
        TFactory<S> &operator =( const TFactory<S> &src );

        int mode;
        mutable std::map< std::string, std::string > strParamMap;
        mutable std::map< std::string, u32 > numParamMap;
        static TFactory<S> * _instance;
    };

    template<class S> TFactory<S> *TFactory<S>::_instance = 0;
}

// always should be last thing in header file
#include "smbios/config/abi_suffix.hpp"

#endif /* FACTORY2_H_ */

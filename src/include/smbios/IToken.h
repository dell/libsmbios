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


#ifndef TOKEN_H
#define TOKEN_H

// compat header should always be first header
#include "smbios/compat.h"

#include <string>

// types.h should be first user-defined header.
#include "smbios/types.h"

#include "smbios/ICmosRW.h"
#include "smbios/ISmbios.h"

// abi_prefix should be last header included before declarations
#include "smbios/config/abi_prefix.hpp"

namespace smbios
{
    // Exceptions
    DECLARE_EXCEPTION( TokenException );
    DECLARE_EXCEPTION_EX( InvalidTokenTableMode, smbios, TokenException );
    DECLARE_EXCEPTION_EX( InvalidAccessMode, smbios, TokenException );
    DECLARE_EXCEPTION_EX( DerefNullPointer, smbios, TokenException );
    DECLARE_EXCEPTION_EX( ParameterError, smbios, TokenException );
    DECLARE_EXCEPTION_EX( InvalidChecksum, smbios, TokenException );
    DECLARE_EXCEPTION_EX( NeedAuthentication, smbios, TokenException );

    // forward declarations
    class ITokenTable;
    class TokenTableIterator;
    class ConstTokenTableIterator;

    class TokenTableFactory : public virtual factory::IFactory
    {
    public:
        static TokenTableFactory *getFactory();
        virtual ~TokenTableFactory() throw();
        virtual ITokenTable *getSingleton(const smbios::ISmbiosTable *table = 0) = 0;
        virtual ITokenTable *makeNew(const smbios::ISmbiosTable *table) = 0;
    protected:
        TokenTableFactory();
    };


    //! Table interface to obtain individual  Tokens
    class ITokenTable
    {
    public:
        typedef TokenTableIterator iterator;
        typedef ConstTokenTableIterator const_iterator;

        virtual ~ITokenTable();

        // ITERATORS
        virtual iterator begin () = 0;
        virtual const_iterator begin () const = 0;

        virtual iterator end () = 0;
        virtual const_iterator end () const = 0;

        virtual iterator operator[]( const int ) = 0;
        virtual const_iterator operator[]( const int ) const = 0;

        virtual iterator operator[]( const std::string & ) = 0;
        virtual const_iterator operator[]( const std::string & ) const = 0;

        virtual std::ostream & streamify( std::ostream & cout ) const = 0;

    protected:
        // No-arg constructor not legal for this class for outside parties
        ITokenTable();
    };


    //! Interface to inspect or manipulate individual  tokens
    class IToken
    {
    public:
        virtual ~IToken();

        virtual std::string getTokenClass() const = 0;

        //! Returns the Token Type, per Dell SMBIOS Spec.
        virtual u32 getType() const = 0;

        //! Returns true if bitfield-type token is set
        virtual bool isActive() const = 0;
        //! Sets the bitmask for bitfield-type tokens
        virtual void activate() const = 0;
        //! Returns true for string-type tokens
        virtual bool isString() const = 0;
        //! Returns true for bool-type tokens
        virtual bool isBool() const = 0;
        //! Returns length for string-type tokens
        virtual unsigned int getStringLength() const = 0;
        //! returns std:string and raw value in first parameter.
        //  \warning byteArray must be at least <b> getStringLength()+1 </b> bytes or NULL!
        /** \param byteArray If Non-NULL, getString() will store the raw string here.
         * \param size This must be the length of the space allocated for byteArray. getString() will not overflow this length.
         * \return Returns a C++-style std::string initialized with the contents of byteArray. Only useful for strings that hold C-style zero-terminated strings.
         */
        virtual const std::string getString( u8 *byteArray = 0, unsigned int size = 0 ) const = 0;
        virtual void setString( const u8 *byteArray, size_t size ) const = 0;

        virtual const ISmbiosItem &getItemRef() const = 0; // use judiciously!

        virtual std::ostream & streamify( std::ostream & cout ) const = 0;
    protected:
        IToken() ;

    private:
        IToken( const IToken & ); //no copying
        IToken & operator = (const IToken & source);//no assignment
    };

    class IProtectedToken
    {
    public:
        virtual ~IProtectedToken() throw() {};
        virtual bool tryPassword(std::string pw) const = 0;
        virtual u32 getValueFormat() const = 0;
    protected:
        IProtectedToken();
        IProtectedToken( const IProtectedToken & );
        IProtectedToken &operator = (const IProtectedToken &);
    };

    class ICmosToken
    {
    public:
        //! returns details about CMOS index/data ports and cmos location.
        //  should be used judiciously, as this circumvents object layering.
        //  The main purpose for this is to implement special-case code
        //  that needs to access raw cmos.
        virtual void getCMOSDetails( u16 *indexPort, u16 *dataPort, u8 *location ) const = 0;
        virtual ~ICmosToken() throw() {};
    protected:
        ICmosToken();
        ICmosToken( const ICmosToken & );
        ICmosToken &operator = (const ICmosToken &);
    };

    class ISmiToken
    {
    public:
        //! returns details about Smi location and value
        //  should be used judiciously, as this circumvents object layering.
        //  The main purpose for this is to implement special-case code
        //  that needs to access raw smi.
        virtual void getSmiDetails( u16 *cmdIOAddress, u8 *cmdIOCode, u8 *location ) const = 0;
        virtual ~ISmiToken() throw() {};
    protected:
        ISmiToken();
        ISmiToken( const ISmiToken & );
        ISmiToken &operator = (const ISmiToken &);
    };


    //! Base class for the TokenTableIterator subclassess
    /**
    */
    class TokenTableIteratorBase
        : public std::iterator < std::forward_iterator_tag, IToken >
    {
    public:
        typedef std::forward_iterator_tag iterator_category;
        typedef std::ptrdiff_t difference_type;

        virtual ~TokenTableIteratorBase() throw() {};
        explicit TokenTableIteratorBase(const ITokenTable *initialTable, int typeToMatch);
        bool operator == (const TokenTableIteratorBase other) const { return current == other.current; };
        bool operator != (const TokenTableIteratorBase other) const { return current != other.current; };
        const IToken * dereference () const;
        IToken * dereference ();
        void incrementIterator();

        void reset();
        bool eof();

    protected:
        int matchType;
        const ITokenTable *table;
        int current;
    };

    //! Iterator for TokenTable objects
    /**
    */
    class TokenTableIterator
        :public TokenTableIteratorBase
    {
    public:
        // Make sure you define these, otherwise you can't use
        // iterators in stl algorithms
        typedef IToken value_type;
        typedef value_type& reference;
        typedef value_type* pointer;

        virtual ~TokenTableIterator() throw() {};
        explicit TokenTableIterator (const ITokenTable *initialTable = 0, int typeToMatch = -1 );
        reference operator * () const;
        pointer operator -> () const;
        TokenTableIterator & operator ++ ();	      // ++Prefix
        const TokenTableIterator operator ++ (int);  //Postfix++
    };

    //! Iterator for const TokenTable objects
    /***
    */
    class ConstTokenTableIterator
        :public TokenTableIteratorBase
    {
    public:
        // Make sure you define these, otherwise you can't use
        // iterators in stl algorithms
        typedef const IToken value_type;
        typedef value_type& reference;
        typedef value_type* pointer;

        virtual ~ConstTokenTableIterator() throw() {};
        explicit ConstTokenTableIterator (const ITokenTable * initialTable = 0, int typeToMatch = -1 );
        reference operator * () const;
        pointer   operator -> () const;
        ConstTokenTableIterator & operator ++ ();	      // ++Prefix
        const ConstTokenTableIterator operator ++ (int);  //Postfix++
    };


    std::ostream & operator << (std::ostream & cout, const ITokenTable & item);
    std::ostream & operator << (std::ostream & cout, const IToken & item);

    // helper functions

    bool isTokenActive(int tokenNum);
    void activateToken(int tokenNum, std::string password = "");
}

// always should be last thing in header file
#include "smbios/config/abi_suffix.hpp"

#endif  /* TOKEN_H */

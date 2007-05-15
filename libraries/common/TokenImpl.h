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

#ifndef TOKENIMPL_H
#define TOKENIMPL_H

// compat header should always be first header
#include "smbios/compat.h"

#include <vector>

// types.h should be first user-defined header.
#include "smbios/types.h"

// Include I*.h headers first
#include "smbios/IToken.h"
#include "smbios/ICmosRW.h"
#include "smbios/ISmbios.h"
#include "smbios/IObserver.h"
#include "ExceptionImpl.h"

#include "TokenLowLevel.h"


namespace smbios
{
    DEFINE_EXCEPTION_EX( InvalidTokenTableModeImpl, smbios, InvalidTokenTableMode );
    DEFINE_EXCEPTION_EX( InvalidAccessModeImpl, smbios, InvalidAccessMode );
    DEFINE_EXCEPTION_EX( DerefNullPointerImpl, smbios, DerefNullPointer );
    DEFINE_EXCEPTION_EX( ParameterErrorImpl, smbios, ParameterError );
    DEFINE_EXCEPTION_EX( InvalidChecksumImpl, smbios, InvalidChecksum );
    DEFINE_EXCEPTION_EX( NeedAuthenticationImpl, smbios, NeedAuthentication );

    // forward declarations
    class CmosTokenD4;
    class TokenTableIterator;
    class TokenTableConstIterator;
    class CmosRWChecksumObserver;

    class TokenTable: public ITokenTable
    {
    public:
        TokenTable( const smbios::ISmbiosTable & );
        virtual ~TokenTable();

        // ITERATORS
        virtual iterator begin ();
        virtual const_iterator begin () const;

        virtual iterator end ();
        virtual const_iterator end () const;

        virtual iterator operator[]( const int );
        virtual const_iterator operator[]( const int ) const;

        virtual iterator operator[]( const std::string & );
        virtual const_iterator operator[]( const std::string & ) const;

        virtual std::ostream & streamify( std::ostream & cout ) const;

        friend class TokenTableIteratorBase;

    protected:
        void addD4Structures(const smbios::ISmbiosTable & table);
        void addD5Structures(const smbios::ISmbiosTable & table);
        void addD6Structures(const smbios::ISmbiosTable & table);
        void addDAStructures(const smbios::ISmbiosTable & table);
        void getD4TokensFromStruct(const smbios::ISmbiosTable::const_iterator &item, const u8 *ptr, size_t size);
        void getDATokensFromStruct(const smbios::ISmbiosTable::const_iterator &item, const u8 *ptr, size_t size);
        void addChecksumObserverForD4Struct(const smbios::ISmbiosTable::const_iterator &item, const u8 *ptr, size_t size);


        std::vector< IToken *> tokenList;
        std::vector< CmosRWChecksumObserver > checksumList;

    private:
        // No-arg constructor not legal for this class for outside parties
        TokenTable();

    };

    class CmosTokenD4 : public IToken, public ICmosToken
    {
    public:
        CmosTokenD4( const smbios::ISmbiosItem &item, const indexed_io_token *token );
        virtual ~CmosTokenD4() throw();

        u32 getType() const;

        virtual bool isActive() const;
        virtual void activate() const;
        virtual bool isString() const;
        virtual bool isBool() const;
        virtual unsigned int getStringLength() const;
        virtual const std::string getString( u8 *byteArray = 0, unsigned int size = 0 ) const;
        virtual void setString( const u8 *byteArray, size_t size ) const;

        virtual const ISmbiosItem &getItemRef() const;
        virtual void getCMOSDetails( u16 *indexPort, u16 *dataPort, u8 *location ) const;

        virtual std::string getTokenClass() const;

        virtual std::ostream & streamify( std::ostream & cout ) const;

    protected:
        CmosTokenD4();
        CmosTokenD4(const CmosTokenD4 &);
        void operator = (const CmosTokenD4 &);

    private:
        std::auto_ptr<const smbios::ISmbiosItem> item;
        indexed_io_access_structure structure;
        indexed_io_token token;
        cmos::ICmosRW  *cmos;  // NEVER DELETE THIS!
        //lifetime is managed by the cmosrw factory
    };

    class CmosTokenD5 : public IToken, public ICmosToken, public IProtectedToken
    {
    public:
        CmosTokenD5( const smbios::ISmbiosItem &item, std::vector< CmosRWChecksumObserver > &initChecksumList);
        virtual ~CmosTokenD5() throw();

        u32 getType() const;

        virtual bool isActive() const;
        virtual void activate() const;
        virtual bool isString() const;
        virtual bool isBool() const;
        virtual unsigned int getStringLength() const;
        virtual const std::string getString( u8 *byteArray = 0, unsigned int size = 0 ) const;
        virtual void setString( const u8 *byteArray, size_t size ) const;

        virtual const ISmbiosItem &getItemRef() const;
        virtual void getCMOSDetails( u16 *indexPort, u16 *dataPort, u8 *location ) const;
        virtual bool tryPassword(std::string pw) const;
        virtual std::string getTokenClass() const;
        virtual u32 getValueFormat() const;

        virtual std::ostream & streamify( std::ostream & cout ) const;

    protected:
        CmosTokenD5();
        CmosTokenD5(const CmosTokenD5 &);
        void operator = (const CmosTokenD5 &);

        virtual void addChecksumObserver() const;

        dell_protected_value_1_structure structure;
        std::auto_ptr<const smbios::ISmbiosItem> item;
        cmos::ICmosRW  *cmos;  // NEVER DELETE THIS!
        //lifetime is managed by the cmosrw factory
        std::string validationKey;
        std::vector< CmosRWChecksumObserver > &checksumList;
    };

    class CmosTokenD6 : public CmosTokenD5
    {
    public:
        CmosTokenD6( const smbios::ISmbiosItem &item, std::vector< CmosRWChecksumObserver > &initChecksumList);
        virtual std::string getTokenClass() const;

    protected:
        CmosTokenD6();
        CmosTokenD6(const CmosTokenD6 &);
        void operator = (const CmosTokenD6 &);

        virtual void addChecksumObserver() const;

    private:
        dell_protected_value_2_structure structure;
    };

    class SmiTokenDA : public IToken, public ISmiToken, public IProtectedToken
    {
    public:
        SmiTokenDA( const smbios::ISmbiosItem &item, const calling_interface_token *token );
        virtual ~SmiTokenDA() throw();

        u32 getType() const;

        virtual bool isActive() const;
        virtual void activate() const;
        virtual bool isString() const;
        virtual bool isBool() const;
        virtual unsigned int getStringLength() const;
        virtual const std::string getString( u8 *byteArray = 0, unsigned int size = 0 ) const;
        virtual void setString( const u8 *byteArray, size_t size ) const;
        virtual const ISmbiosItem &getItemRef() const;
        virtual std::string getTokenClass() const;
        virtual u32 getValueFormat() const;
        virtual bool tryPassword(std::string pw) const;
        virtual std::ostream & streamify( std::ostream & cout ) const;
        virtual void getSmiDetails(  u16 *cmdIOAddress, u8 *cmdIOCode, u8 *location ) const;

    protected:
        SmiTokenDA();
        SmiTokenDA(const SmiTokenDA &);
        void operator = (const SmiTokenDA &);

    private:
        std::auto_ptr<const smbios::ISmbiosItem> item;
        calling_interface_structure structure;
        calling_interface_token token;
        mutable std::string password;
    };

    // fixes up checksums on a cmos object
    // when the cmos object is written to
    class CmosRWChecksumObserver : public observer::IObserver
    {
    public:
        CmosRWChecksumObserver( std::string description, cmos::ICmosRW *cmos, int checkType, u32 indexPort, u32 dataPort, u32 start, u32 end, u32 checksumLocation  );
        CmosRWChecksumObserver( const CmosRWChecksumObserver &source );
        // called when the observed cmos changes
        // doUpdate should be bool*.  not very safe, but hey, this is C.
        virtual void update( const observer::IObservable *whatChanged, void *doUpdate );
        virtual ~CmosRWChecksumObserver();
    private:
        std::string description;
        cmos::ICmosRW *cmos;
        int checkType;
        u32 indexPort;
        u32 dataPort;
        u32 start;
        u32 end;
        u32 checksumLocation;
    };

    enum
    {
        CHECK_TYPE_WORD_CHECKSUM   = 0x00, //simple running sum in word
        CHECK_TYPE_BYTE_CHECKSUM   = 0x01, //simple running sum in byte
        CHECK_TYPE_WORD_CRC        = 0x02, // crc
        CHECK_TYPE_WORD_CHECKSUM_N = 0x03, //simple runnign sum in word, then (~result + 1)
    };



}

#endif  /* TOKEN_H */

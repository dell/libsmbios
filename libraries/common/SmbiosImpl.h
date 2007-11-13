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


#ifndef SMBIOSIMPL_H
#define SMBIOSIMPL_H

// compat header should always be first header if including system headers
#include "smbios/compat.h"

#include <vector>

#include "smbios/ISmbios.h"
#include "smbios/SmbiosLowLevel.h"
#include "SmbiosWorkaroundImpl.h"
#include "FactoryImpl2.h"
#include "ExceptionImpl.h"

namespace smbios
{
    DEFINE_EXCEPTION_EX( ParameterExceptionImpl, smbios, ParameterException );
    DEFINE_EXCEPTION_EX( ParseExceptionImpl, smbios, ParseException );
    DEFINE_EXCEPTION_EX( StringUnavailableImpl, smbios, StringUnavailable );
    DEFINE_EXCEPTION_EX( DataOutOfBoundsImpl, smbios, DataOutOfBounds );
    DEFINE_EXCEPTION_EX( ItemNotFoundImpl, smbios, ItemNotFound );

    class SmbiosFactoryImpl : public factory::TFactory<SmbiosFactory>
    {
    public:
        SmbiosFactoryImpl();
        virtual ~SmbiosFactoryImpl() throw();
        virtual ISmbiosTable *getSingleton( ); // returns singleton
        virtual ISmbiosTable *makeNew( ); // not for use
    protected:
        static ISmbiosTable *_tableInstance;
    };

    class SmbiosStrategy
    {
    public:
        SmbiosStrategy() {};
        virtual ~SmbiosStrategy() {};

        virtual bool getSmbiosTable(const u8 **, smbiosLowlevel::smbios_table_entry_point *, bool ) = 0;
    };

    class SmbiosMemoryStrategy : public SmbiosStrategy
    {
    public:
        virtual ~SmbiosMemoryStrategy() throw() {};
        SmbiosMemoryStrategy(unsigned long initOffset) :SmbiosStrategy(), offset(initOffset) {};
        SmbiosMemoryStrategy(const SmbiosMemoryStrategy &src) : SmbiosStrategy(), offset(src.offset) {};
        virtual bool getSmbiosTable(const u8 **, smbiosLowlevel::smbios_table_entry_point *, bool );
    protected:
        // popular mem locations we use in scanning code.
        enum {
            E_BLOCK_START = 0xE0000UL,
            F_BLOCK_START = 0xF0000UL,
            F_BLOCK_END = 0xFFFFFUL
        };

        virtual void getSmbiosTableHeader(smbiosLowlevel::smbios_table_entry_point *, bool);
        virtual void getSmbiosTableBuf(const u8 **, smbiosLowlevel::smbios_table_entry_point);
        unsigned long offset;
    };

    class SmbiosLinuxEFIStrategy : public SmbiosMemoryStrategy
    {
    public:
        virtual ~SmbiosLinuxEFIStrategy() throw() {};
        SmbiosLinuxEFIStrategy() :SmbiosMemoryStrategy(0) {};
        SmbiosLinuxEFIStrategy(const SmbiosLinuxEFIStrategy &src) : SmbiosMemoryStrategy(0) {};

    protected:
        virtual void getSmbiosTableHeader(smbiosLowlevel::smbios_table_entry_point *, bool);
    };

    class SmbiosWinWMIStrategy : public SmbiosStrategy
    {
    public:
        virtual ~SmbiosWinWMIStrategy() throw() {};
        SmbiosWinWMIStrategy() {};
        virtual bool getSmbiosTable(const u8 **, smbiosLowlevel::smbios_table_entry_point *, bool );
    };

    class SmbiosWinGetFirmwareTableStrategy : public SmbiosStrategy
    {
    public:
        virtual ~SmbiosWinGetFirmwareTableStrategy() throw() {};
        SmbiosWinGetFirmwareTableStrategy() {};
        virtual bool getSmbiosTable(const u8 **, smbiosLowlevel::smbios_table_entry_point *, bool );
    };

    class SmbiosTable : public virtual ISmbiosTable
    {
    public:
        // CONSTRUCTORS, DESTRUCTOR, and ASSIGNMENT
        explicit SmbiosTable(std::vector<SmbiosStrategy *> initStrategyList, bool strictValidation = 0);

        // CONSTRUCTORS, DESTRUCTOR, and ASSIGNMENT
        //SmbiosTable (const SmbiosTable & source);
        //virtual SmbiosTable& operator = (const SmbiosTable & source);
        virtual ~SmbiosTable ();

        // ITERATORS
        virtual iterator begin ();
        virtual const_iterator begin () const;

        virtual iterator end ();
        virtual const_iterator end () const;

        virtual iterator operator[]( const int );
        virtual const_iterator operator[]( const int ) const;

        virtual iterator operator[]( const std::string & );
        virtual const_iterator operator[]( const std::string & ) const;


        // MEMBERS
        virtual void rawMode(bool m) const;
        virtual int getNumberOfEntries () const;  // used by unit-test code

        // Used by the validateBios.cpp
        virtual smbiosLowlevel::smbios_table_entry_point getTableEPS() const;

        virtual std::ostream & streamify(std::ostream & cout ) const;

        // used by factory only.
        virtual void initializeWorkaround() const;
        // restricting the header checking
        virtual void setStrictValidationMode(bool mode) const;
        virtual bool getStrictValidationMode() const;

        virtual ISmbiosItem *getCachedItem( const void * ) const;
        virtual void cacheItem( const void *, ISmbiosItem &newitem ) const;
        virtual void clearItemCache() const;
        ISmbiosItem & getSmbiosItem (const u8 *);
        const ISmbiosItem & getSmbiosItem (const u8 *) const;
        const u8 * nextSmbiosStruct ( const u8 * current = 0) const;

    protected:
        // No-arg constructor not legal for this class for regular users
        SmbiosTable ();

        // used by the iterator
        virtual ISmbiosItem &makeItem(
            const void *header = 0) const;

        mutable std::map< const void *, ISmbiosItem *> itemList;
        mutable bool initializing;
        mutable bool strictValidationMode;
        mutable std::auto_ptr<SmbiosWorkaroundTable> workaround;
        const u8 * smbiosBuffer;
        smbiosLowlevel::smbios_table_entry_point table_header;

    private:
        SmbiosTable (const SmbiosTable &source);

        virtual void reReadTable();
        mutable unsigned long offset;
        std::vector<SmbiosStrategy *> strategyList;
    };




    class SmbiosItem : public ISmbiosItem
    {
    public:
        SmbiosItem (const SmbiosItem & source);
        explicit SmbiosItem (const smbiosLowlevel::smbios_structure_header *header = 0);
        virtual ~SmbiosItem ();

        virtual std::auto_ptr<const ISmbiosItem> clone() const;
        virtual std::auto_ptr<ISmbiosItem> clone();
        virtual std::ostream & streamify( std::ostream & cout ) const;

        u8 getType() const;
        u8 getLength() const;
        u16 getHandle() const;

        virtual void getData( unsigned int offset, u8 *out, size_t size ) const;

        virtual const u8* getBufferCopy(size_t &length) const;
        virtual size_t getBufferSize() const;

        virtual const char *getStringByStringNumber (u8) const;

        // for table only...
        virtual void fixup( const SmbiosWorkaroundTable *workaround ) const;
    protected:
        const smbiosLowlevel::smbios_structure_header * header;
        size_t header_size;

    private:
        SmbiosItem & operator = (const SmbiosItem & source);
    };

}


#endif /* SMBIOSIMPL_H */

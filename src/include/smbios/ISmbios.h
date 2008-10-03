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


#ifndef SMBIOSINTERFACE_H
#define SMBIOSINTERFACE_H

// compat header should always be first header
#include "smbios/compat.h"

#include <cstdlib>		// Provides size_t and NULL
#include <iostream>
#include <map>
#include <memory>

// types.h should be first user-defined header.
#include "smbios/types.h"
#include "smbios/IFactory.h"
#include "smbios/IException.h"
#include "smbios/SmbiosLowLevel.h"

// abi_prefix should be last header included before declarations
#include "smbios/config/abi_prefix.hpp"

namespace smbios
{
    // Exception Classes
    DECLARE_EXCEPTION( SmbiosException );
    DECLARE_EXCEPTION_EX( ParameterException, smbios, SmbiosException );
    DECLARE_EXCEPTION_EX( ParseException, smbios, SmbiosException );
    DECLARE_EXCEPTION_EX( StringUnavailable, smbios, SmbiosException );
    DECLARE_EXCEPTION_EX( DataOutOfBounds, smbios, SmbiosException );
    DECLARE_EXCEPTION_EX( ItemNotFound, smbios, SmbiosException );


    //forward declarations... defined 'for real' below...
    class ISmbiosTable;
    class ISmbiosItem;
    class SmbiosTableIterator;
    class ConstSmbiosTableIterator;

    //!AbstractFactory that produces ISmbiosTable objects.
    /** The SmbiosFactory class is based on the Factory design pattern.
    * The SmbiosFactory is the recommended method to create ISmbiosTable
    * objects.
    *
    * The getSingleton() is the recommended method to call to create
    * tables. You need not delete the pointer returned by this method, it
    * will be delete by the factory when it is reset() or destructed.
    *
    * Most users of the factory need call nothing more than getFactory()
    * and then getSingleton() on the returned factory object.
    *
    * Advanced users can call setParameter() to set up internal factory
    * variables that control creation of tables.
    */
    class SmbiosFactory : public virtual factory::IFactory
    {
    public:
        //! Create a factory object that you can use to create ISmbiosTable objects.
        /** Factory entry point: This is the method to call to get a handle
         * to the SmbiosFactory. The SmbiosFactory is the recommended method
         * to create ISmbiosTable objects.
         *
         * The getSingleton() is the recommended method to call to create
         * tables. You need not delete the pointer returned by this method, it
         * will be deleted by the factory when it is reset() or destructed.
         *
         * \returns Singleton SmbiosFactory object pointer.
         */
        static SmbiosFactory *getFactory();
        virtual ~SmbiosFactory() throw();

        //! Recommended way to get an ISmbiosTable object.
        /** getSingleton() returns a pointer to a Singleton ISmbiosTable
         * object. The user need not delete the pointer returned by this method.
         * The singleton will be deleted when the factory is destructed or
         * the reset() method is called
         * \returns (ISmbiosTable *) -- Factory manages lifetime, do not delete.
         */
        virtual ISmbiosTable *getSingleton() = 0;

        //! Create a new ISmbiosTable object that the caller must delete. (NOT RECOMMENDED)
        /** The makeNew() method returns a pointer to a newly allocated
         * ISmbiosTable object. The caller is responsible for deleting this
         * reference when it is finished with it. It is recommended that the
         * caller store the pointer in an std::auto_ptr<ISmbiosTable>.
         *
         * The getSingleton() method is preferred over this method.
         *
         * \returns (ISmbiosTable *) -- caller must delete
         */
        virtual ISmbiosTable *makeNew() = 0;
    protected:
        //! Use getFactory() to get a factory.
        SmbiosFactory();
    };

    //!Interface definition for Smbios Table operations.
    /**
     * \copydoc smbios_theory
     */
    class ISmbiosTable
    {
    public:
        // Std container typedefs. Everybody expects to
        // say 'iterator' or 'const_iterator'
        typedef SmbiosTableIterator iterator;
        typedef ConstSmbiosTableIterator const_iterator;

        // CONSTRUCTORS, DESTRUCTOR, and ASSIGNMENT
        ISmbiosTable();
        // Interface class: no default or copy constructor
        virtual ~ISmbiosTable ();

        // ITERATORS
        //
        //! Standard iterator interface. Points to first table item.
        /**
         * \returns iterator or const_iterator
         * Example Iterator Usage:
\code
    smbios::ISmbiosTable *table = smbios::SmbiosFactory::getFactory()->getSingleton();
    smbios::ISmbiosTable::iterator item = table->begin();
    while( item != table->end() )
    {
        cout << "Type of Item: " << item->getType();
        ++item;
    }
\endcode
        */
        virtual iterator begin () = 0;
        //! Standard iterator interface. Points to first table item.
        /** \copydoc begin() */
        virtual const_iterator begin () const = 0;

        //! Standard iterator interface. Points to one-past-the-last table item.
        /** \copydoc begin() */
        virtual iterator end () = 0;

        //! Standard iterator interface. Points to one-past-the-last table item.
        /** Used by const_iterator.
         * \copydoc begin() */
        virtual const_iterator end () const = 0;

        //! Standard indexed access by integer item type.
        /** The operator[] method returns an \a iterator that can be used to
         * iterator over all items in the table of the supplied \a type. So, for
         * example, if you want to perform an operation on all SMBIOS type 0x01
         * (System Information Block) structures, just index the table object
         * using the [] operator.
         * \returns iterator or const_iterator
         * Sample usage:
\code
// Integer indexing example
smbios::ISmbiosTable *table = smbios::SmbiosFactory::getFactory()->getSingleton();
smbios::ISmbiosTable::iterator item1 = (*table)[0];
cout << "The BIOS Version is: " << item1->getString(0x05) << endl;
\endcode
         * \sa operator[]( const std::string & ) const
         */
        virtual iterator operator[]( const int ) = 0;

        //! Standard indexed access by integer item type.
         /** \copydoc operator[]( const int ) */
        virtual const_iterator operator[]( const int ) const = 0;

        // MEMBERS
        //! Disables all workarounds for _new_ items created by the table.
        /** Any new item generated by the table will not have workarounds
         * applied to them. However, any previously-existing items that have had
         * workarounds applied still exist. If this is not what you want,
         * recommend calling clearItemCache() prior to calling rawMode().
         * \param m pass in a bool value to turn raw mode on or off.
         */
        virtual void rawMode(bool m = true) const = 0;

        //! Clears out any cached SmbiosItem entries in the cache
        /** This API is useful for two instances. First, you can use this to
         * reduce memory usage if you know that you do not need any
         * ISmbiosItem(s) out of the table for a while. The cached
         * ISmbiosItem(s) will be deleted and then re-populated on demand when
         * queries are made for them.
         *
         * Next, this API is used internally when reReadTable() is called to
         * clear out all old ISmbiosItems.
         *
         * \warning All previous references or pointers to ISmbiosItem objects
         * created from this table become invalid and attempts to access them
         * will cause undefined behaviour (most likely your code will crash.)
         *
         * \todo clearItemCache() needs to be made an abstract function and the
         * definition needs to be moved to the SmbiosItem class. This needs to
         * happen at the same time that itemList is moved.
         */
        virtual void clearItemCache() const = 0;

        //! Returns the number of table items, per SMBIOS table header
        virtual int getNumberOfEntries () const = 0;  // used by unit-test code
        //! Returns  the table entry point structure
        // Used by the validateBios code.
        virtual smbiosLowlevel::smbios_table_entry_point getTableEPS() const = 0;

        friend class SmbiosTableIteratorBase;
    protected:
        virtual const ISmbiosItem & getSmbiosItem (const u8 *current) const = 0;
        virtual ISmbiosItem & getSmbiosItem (const u8 *current) = 0;
        virtual const u8 * nextSmbiosStruct ( const u8 * current = 0) const = 0;

        //! Used by operator << (std::ostream & cout, const ISmbiosTable & ) to
        //output table information.
        /** Users normally would not need or want to call this API. The normal
         * operator<<() has been overloaded to call this function internally.
         */
        virtual std::ostream & streamify(std::ostream & cout ) const = 0;
        friend std::ostream & operator << (std::ostream & cout, const ISmbiosTable & item);

    private:
        explicit ISmbiosTable(const ISmbiosTable &); ///< not implemented (explicitly disallowed)
        void operator =( const ISmbiosTable & ); ///< not implemented (explicitly disallowed)
    };

    //!Interface definition for Smbios Item operations.
    /**
     * \copydoc item_theory
     */
    class ISmbiosItem
    {
    public:
        /** Destructor */
        virtual ~ISmbiosItem ();
        ISmbiosItem();

        virtual std::auto_ptr<const ISmbiosItem> clone() const = 0;
        virtual std::auto_ptr<ISmbiosItem> clone() = 0;

        /** Returns the Type field of the SMBIOS Item.
         * This field is standard for all SMBIOS tables and is defined
         * in the SMBIOS standard.
         * \returns The Type value.
         */
        virtual u8 getType() const = 0;

        /** Returns the Length field of the SMBIOS Item.
         * This field is standard for all SMBIOS tables and is defined
         * in the SMBIOS standard.
         * \returns The Length value.
         */
        virtual u8 getLength() const = 0;

        /** Returns the Handle field of the SMBIOS Item.
         * This field is standard for all SMBIOS tables and is defined
         * in the SMBIOS standard.
         * \returns The Handle value.
         */
        virtual u16 getHandle() const = 0;

        /** Set of accessor functions: getU8(), getU16(), getU32(), and getU64()
         * Returns a (byte|word|dword|qword) field from the Item.
         *
         * The \a offset specified is an int representing the a valid offset
         * within the table.  Method will return a u8/u16/u32/u64
         * (depending on function called).
         *
         * These methods all check the offset parameter for out of bounds
         * conditions. They will throw exceptions on attempts to access data
         * outside the length of the present item.
         *
         * \param offset The offset to the field within the Item.
         * \param out  void pointer to where to store output data
         * \param size  size of data to return
         *
         * \returns The (byte|word|dword|qword) at \a offset.  Throws
         * smbios::SmbiosItemDataOutOfBounds or smbios::SmbiosParseException
         * on error.
         *
         *
         * \warning These methods are unchecked access. There is no verification
         * that (for example) when you use getU8() that the location you are
         * trying to access is actually a U8.
         */
        virtual void getData( unsigned int offset, u8 *out, size_t size ) const = 0;

        virtual const u8* getBufferCopy(size_t &length) const = 0;

        //! Returns the buffer size of the item.
        // The validateBios.cpp calls this function.
        virtual size_t getBufferSize() const = 0;

        /** Not likely to be useful to regular client code. It is public
         * mainly to help in writing Unit Tests. Clients should normally
         * use getString().
         */
        virtual const char *getStringByStringNumber (u8) const = 0;

        enum {
            FIELD_LEN_BYTE=1,
            FIELD_LEN_WORD=2,
            FIELD_LEN_DWORD=4,
            FIELD_LEN_QWORD=8
        };

    protected:
        /** Used by 'std::ostream &smbios::operator <<( std::ostream &, ISmbiosItem&)'
         * to print out the item info.
         *
         * Not particularly useful for clients. Use operator<< instead.
         */
        virtual std::ostream & streamify( std::ostream & cout ) const = 0;
        friend std::ostream & operator << (std::ostream & cout, const ISmbiosItem & item);
    };

    u8 getItemType(const ISmbiosItem &item);
    u8 getItemLength(const ISmbiosItem &item);
    u16 getItemHandle(const ISmbiosItem &item);

    u8 getU8_FromItem(const ISmbiosItem &item, unsigned int offset);
    u16 getU16_FromItem(const ISmbiosItem &item, unsigned int offset);
    u32 getU32_FromItem(const ISmbiosItem &item, unsigned int offset);
    u64 getU64_FromItem(const ISmbiosItem &item, unsigned int offset);
    const char *getString_FromItem(const ISmbiosItem &item, unsigned int offset);
    void *getBits_FromItem(const ISmbiosItem &item, unsigned int offset, void *out, unsigned int lsb=0, unsigned int msb=0 );
    bool isBitSet(const ISmbiosItem *itemPtr, unsigned int offset, unsigned int bitToTest);

    template <class R>
    R &getData(const ISmbiosItem &item, unsigned int offset, R &out)
    {
        item.getData(offset, &out, sizeof(R));
        return out;
    }

    //! Iterator base class for ISmbiosTable objects.
    /** The base class for iterators over ISmbiosTable. This class has all of
     * the data items to keep track of the position. There is no good way to
     * implement this as a pure abstract base class (interface) because of the
     * way STL iterators were designed (I think.)
     *
     * This class is stable and should not be modified.
     */
    class SmbiosTableIteratorBase
    {
    public:
        typedef std::forward_iterator_tag iterator_category;
        typedef std::ptrdiff_t difference_type;


        explicit SmbiosTableIteratorBase(const ISmbiosTable * initialTable = 0, int typeToMatch = -1 );
        SmbiosTableIteratorBase &operator=(const SmbiosTableIteratorBase&);
        virtual ~SmbiosTableIteratorBase() throw();
        bool operator == (const SmbiosTableIteratorBase &other) const;
        bool operator != (const SmbiosTableIteratorBase &other) const;
        void incrementIterator ();
        const ISmbiosItem & dereference () const;
        ISmbiosItem & dereference ();

        void reset();
        bool eof();

    protected:
        int matchType;
        const ISmbiosTable * table;
        const u8 * current;
    };

    class SmbiosTableIterator:
        public SmbiosTableIteratorBase,
        public std::iterator < std::forward_iterator_tag, ISmbiosItem >
    {
    public:
        typedef ISmbiosItem value_type;
        typedef value_type& reference;
        typedef value_type* pointer;

        virtual ~SmbiosTableIterator() throw();
        explicit SmbiosTableIterator(ISmbiosTable * initialTable = 0, int typeToMatch = -1 );
        reference operator * ();
        pointer   operator -> ();
        SmbiosTableIterator & operator ++ (); // ++Prefix
        const SmbiosTableIterator operator ++ (int);  //Postfix++
    };

    class ConstSmbiosTableIterator:
        public SmbiosTableIteratorBase,
        public std::iterator < std::forward_iterator_tag, const ISmbiosItem >
    {
    public:
        typedef const ISmbiosItem value_type;
        typedef value_type& reference;
        typedef value_type* pointer;

        virtual ~ConstSmbiosTableIterator() throw();
        explicit ConstSmbiosTableIterator(const ISmbiosTable * initialTable = 0, int typeToMatch = -1 );
        ConstSmbiosTableIterator &operator=(const SmbiosTableIteratorBase&);

        reference operator * () const;
        pointer   operator -> () const;
        ConstSmbiosTableIterator & operator ++ (); // ++Prefix
        const ConstSmbiosTableIterator operator ++ (int);  //Postfix++
    };

    //
    // Non-member functions
    //
    std::ostream & operator << (std::ostream & cout, const ISmbiosTable & item);
    std::ostream & operator << (std::ostream & cout, const ISmbiosItem & item);

}


// always should be last thing in header file
#include "smbios/config/abi_suffix.hpp"

#endif  /* SMBIOSINTERFACE_H */

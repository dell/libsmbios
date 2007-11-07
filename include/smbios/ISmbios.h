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
#include "smbios/ISmbiosBase.h"

// abi_prefix should be last header included before declarations
#include "smbios/config/abi_prefix.hpp"

namespace smbios
{

    //forward declarations... defined 'for real' below...
    class ISmbiosTable;
    class SmbiosTableIterator;


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
    class ISmbiosTable : public ISmbiosTableBase
    {
    public:
        // Std container typedefs. Everybody expects to
        // say 'iterator' or 'const_iterator'
        typedef SmbiosTableIterator iterator;
        typedef SmbiosTableIterator const_iterator;

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
        virtual const_iterator begin () const = 0;

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
        virtual const_iterator operator[]( const int ) const = 0;

    private:
        explicit ISmbiosTable(const ISmbiosTable &); ///< not implemented (explicitly disallowed)
        void operator =( const ISmbiosTable & ); ///< not implemented (explicitly disallowed)
    };


    //! Iterator base class for ISmbiosTable objects.
    /** The base class for iterators over ISmbiosTable. This class has all of
     * the data items to keep track of the position. There is no good way to
     * implement this as a pure abstract base class (interface) because of the
     * way STL iterators were designed (I think.)
     *
     * This class is stable and should not be modified.
     */
    class SmbiosTableIterator: 
        public std::iterator < std::forward_iterator_tag, const ISmbiosItem >
    {
    public:
        // Make sure you define these, otherwise you can't use
        // iterators in stl algorithms
        typedef std::forward_iterator_tag iterator_category;
        typedef const ISmbiosItem value_type;
        typedef value_type& reference;
        typedef value_type* pointer;
        typedef std::ptrdiff_t difference_type;


        explicit SmbiosTableIterator(const ISmbiosTable * initialTable = 0, int typeToMatch = -1 )
            : matchType(typeToMatch), table(initialTable), current(0)
            { incrementIterator(); };
        virtual ~SmbiosTableIterator() throw() {};
        bool operator == (const SmbiosTableIterator other) const { return current == other.current; };
        bool operator != (const SmbiosTableIterator other) const { return current != other.current; };

        reference operator * () const { return dereference(); };
        pointer   operator -> () const { return &dereference(); };
        SmbiosTableIterator & operator ++ () { incrementIterator(); return *this; }; // ++Prefix
        const SmbiosTableIterator operator ++ (int)     
        {
            const SmbiosTableIterator oldValue = *this;
            ++(*this);
            return oldValue;
        };  //Postfix++
    protected:
        void incrementIterator ();
        const ISmbiosItem & dereference () const;

        int matchType;
        const ISmbiosTable * table;
        mutable const u8 * current;
    };
    
    //
    // Non-member functions
    //
    std::ostream & operator << (std::ostream & cout, const ISmbiosTable & item);
}


// always should be last thing in header file
#include "smbios/config/abi_suffix.hpp"

#endif  /* SMBIOSINTERFACE_H */

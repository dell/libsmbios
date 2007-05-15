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

//
#ifndef ISMBIOSWORKAROUND_H_
#define ISMBIOSWORKAROUND_H_

// types.h should be first user-defined header.
#include "smbios/types.h"
#include "smbios/ISmbios.h"
#include "FactoryImpl2.h"

namespace smbios
{

    // forward decls
    class SmbiosWorkaroundTable;
    struct Workaround;

    /** This class is a private class used by the SmbiosTable class
     * to handle workarounds for BIOS bugs.
     *
     * As such, there is no ABI guarantee, so this class doesn't follow the
     * XXXXFactory | XXXXFactoryImpl  pattern that the rest of the
     * Factories follow. That makes things a bit simpler in implementation.
     *
     * There is no ISmbiosWorkaroundTable for the same reason as above.
     *
     * General Theory of Operation:
     *      The SmbiosTable object contains a buffer of the raw SMBIOS table, as
     *      read from memory. As such, this is never changed and will always
     *      reflect bit-for-bit what is in memory.
     *
     *      As each SmbiosItem is created, the ->fixup() method is called, which
     *      alters the individual items. It will alter the actual item buffer
     *      that the item holds so that it has correct data.
     */
    class SmbiosWorkaroundFactory : public factory::TFactory<factory::IFactory>
    {
    public:
        //! Use getFactory() to get a factory.
        SmbiosWorkaroundFactory() {};

        //! Create a factory object that you can use to create SmbiosWorkaroundTable objects.
        /** Factory entry point: This is the method to call to get a handle
         * to the SmbiosWorkaroundFactory. The SmbiosWorkaroundFactory is the recommended method
         * to create SmbiosWorkaroundTable objects.
         *
         * \returns Singleton SmbiosWorkaroundFactory object pointer.
         */
        static factory::TFactory<smbios::SmbiosWorkaroundFactory> *getFactory();
        virtual ~SmbiosWorkaroundFactory() throw();

        //! Create a new SmbiosWorkaroundTable object that the caller must delete.
        /** The makeNew() method returns a pointer to a newly allocated
         * SmbiosWorkaroundTable object. The caller is responsible for deleting this
         * reference when it is finished with it. It is recommended that the
         * caller store the pointer in an std::auto_ptr<SmbiosWorkaroundTable>.
         *
         * \returns (SmbiosWorkaroundTable *) -- caller must delete
         */
        virtual SmbiosWorkaroundTable *makeNew( const ISmbiosTable *table );

    protected:
        static SmbiosWorkaroundTable   *_tableInstance;
    };

    class SmbiosWorkaroundTable
    {
    public:
        SmbiosWorkaroundTable( const ISmbiosTable * table, const Workaround **initWorkarounds);
        virtual ~SmbiosWorkaroundTable();
        void fixupItem( const ISmbiosItem *item, u8 *buffer, size_t bufsize ) const;

    private:
        SmbiosWorkaroundTable(); //< not implmented (or legal)
        void operator =( const SmbiosWorkaroundTable & ); //< not implmented (or legal)

        int systemId;
        std::string biosVersion;
        const Workaround **workaroundsForThisSystem;
    };

    enum { TYPE_U8=1, TYPE_U16=2, TYPE_U32=4, TYPE_U64=8 } ;

    struct SystemAffected
    {
        int             systemId;
        const char *    biosMinVersion;
        const char *    biosMaxVersion;
    };

    // give data[8] at the beginning to provide the most flexibility for
    // people trying to initialize static members.
    union datatron
    {
        u8  data[8]; // can only initialize the first member of a union (ansi)
        u64 dataU64;
        u32 dataU32;
        u16 dataU16;
        u8  dataU8;
    };

    struct WorkaroundSmbiosItem
    {
        int     type;
        unsigned int     fieldOffset;
        int     fieldDataType;
        datatron data;
    };

    struct Workaround
    {
        const char            *name;
        const WorkaroundSmbiosItem      *symptoms;
        const WorkaroundSmbiosItem      *fixups;
    };

    struct SystemWorkaround
    {
        const SystemAffected  *system;
        const Workaround      **workarounds;
    };

}

#endif /* ISMBIOSWORKAROUND_H_ */

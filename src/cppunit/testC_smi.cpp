/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
 * vim:expandtab:autoindent:tabstop=4:shiftwidth=4:filetype=c:cindent:textwidth=0:
 *
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

// compat header should always be first header if including system headers
#include "smbios_c/compat.h"

// system
#include <string.h>

#include "testC_smi.h"
#include "smbios_c/obj/memory.h"
#include "smbios_c/obj/smi.h"
#include "smbios_c/smi.h"

#include "outputctl.h"
#include "main.h"

using namespace std;

// Note:
//      Except for , there are no "using namespace XXXX;" statements
//      here... on purpose. We want to ensure that while reading this code that
//      it is extremely obvious where each function is coming from.
//
//      This leads to verbose code in some instances, but that is fine for
//      these purposes.

// Register the test
CPPUNIT_TEST_SUITE_REGISTRATION (testCsmi);

// keep these struct in sync with what is in smi_impl.h
struct UT_smi_cmd_buffer
{
    u16 smi_class;
    u16 smi_select;
    union {  /* to match BIOS docs, can use exact arg names specified in doc */
        u32      arg[4];
        struct
        {
            u32 cbARG1;
            u32 cbARG2;
            u32 cbARG3;
            u32 cbARG4;
        };
    };
    union {  /* to match BIOS docs, can use exact res names specified in doc */
        u32      res[4];
        struct
        {
            s32 cbRES1;
            s32 cbRES2;
            s32 cbRES3;
            s32 cbRES4;
        };
    };
}
LIBSMBIOS_C_PACKED_ATTR;

// keep these struct in sync with what is in smi_impl.h
struct UT_dell_smi_obj
{
    int initialized;
    u16 command_address;
    u8  command_code;
    int (*execute)(struct dell_smi_obj *);
    struct UT_smi_cmd_buffer smi_buf;
    u8 *physical_buffer[4];
    size_t physical_buffer_size[4];
};


int smi_ut_exec(struct dell_smi_obj *smi)
{
    struct UT_dell_smi_obj *ut_smi = (struct UT_dell_smi_obj *)smi;

    switch(ut_smi->smi_buf.smi_class){
    case 0x01:
        CPPUNIT_ASSERT_EQUAL( (u32)0x01, ut_smi->smi_buf.cbARG1 );
        CPPUNIT_ASSERT_EQUAL( (u32)0x01, ut_smi->smi_buf.cbARG2 );
        CPPUNIT_ASSERT_EQUAL( (u32)0x01, ut_smi->smi_buf.cbARG3 );
        CPPUNIT_ASSERT_EQUAL( (u32)0x01, ut_smi->smi_buf.cbARG4 );
        ut_smi->smi_buf.cbRES1 = 1;
        ut_smi->smi_buf.cbRES2 = 1;
        ut_smi->smi_buf.cbRES3 = 1;
        ut_smi->smi_buf.cbRES4 = 1;
        CPPUNIT_ASSERT_EQUAL( (u8*)0, ut_smi->physical_buffer[cbARG1] );
        CPPUNIT_ASSERT_EQUAL( (u8*)0, ut_smi->physical_buffer[cbARG2] );
        CPPUNIT_ASSERT_EQUAL( (u8*)0, ut_smi->physical_buffer[cbARG3] );
        CPPUNIT_ASSERT_EQUAL( (u8*)0, ut_smi->physical_buffer[cbARG4] );
        break;
    case 0x02:
        CPPUNIT_ASSERT_EQUAL( (u32)0x02, ut_smi->smi_buf.cbARG1 );
        CPPUNIT_ASSERT_EQUAL( (u32)0x02, ut_smi->smi_buf.cbARG2 );
        CPPUNIT_ASSERT_EQUAL( (u32)0x00, ut_smi->smi_buf.cbARG3 ); // phys buf ptr arg == 0
        CPPUNIT_ASSERT_EQUAL( (u32)0x02, ut_smi->smi_buf.cbARG4 );
        ut_smi->smi_buf.cbRES1 = 2;
        ut_smi->smi_buf.cbRES2 = 2;
        ut_smi->smi_buf.cbRES3 = 2;
        ut_smi->smi_buf.cbRES4 = 2;
        CPPUNIT_ASSERT_EQUAL( (u8*)0, ut_smi->physical_buffer[cbARG1] );
        CPPUNIT_ASSERT_EQUAL( (u8*)0, ut_smi->physical_buffer[cbARG2] );
        memset(ut_smi->physical_buffer[cbARG3], 'a', 32);
        CPPUNIT_ASSERT_EQUAL( (u8*)0, ut_smi->physical_buffer[cbARG4] );
        break;
    default:
        break;
    }

    return 0;
}

static int smi_ut_init_fn(struct dell_smi_obj *smi)
{
    struct UT_dell_smi_obj *ut_smi = (struct UT_dell_smi_obj *)smi;
    ut_smi->execute = smi_ut_exec;
    return 0;
}

void testCsmi::setUp()
{
    setupForUnitTesting(getTestDirectory(), getWritableDirectory());
}

void testCsmi::tearDown()
{
    reset();
}

void testCsmi::testSmiConstruct()
{
    STD_TEST_START(getTestName().c_str() << "  ");
    struct dell_smi_obj *smi = dell_smi_factory(DELL_SMI_GET_NEW | DELL_SMI_UNIT_TEST_MODE, smi_ut_init_fn);

    dell_smi_obj_set_class(smi, 0x01);
    dell_smi_obj_set_select(smi, 0x01);
    dell_smi_obj_set_arg(smi, cbARG1, 0x01);
    dell_smi_obj_set_arg(smi, cbARG2, 0x01);
    dell_smi_obj_set_arg(smi, cbARG3, 0x01);
    dell_smi_obj_set_arg(smi, cbARG4, 0x01);

    CPPUNIT_ASSERT_EQUAL( (u32)0, dell_smi_obj_get_res(smi, cbRES1));
    CPPUNIT_ASSERT_EQUAL( (u32)0, dell_smi_obj_get_res(smi, cbRES2));
    CPPUNIT_ASSERT_EQUAL( (u32)0, dell_smi_obj_get_res(smi, cbRES3));
    CPPUNIT_ASSERT_EQUAL( (u32)0, dell_smi_obj_get_res(smi, cbRES4));

    dell_smi_obj_execute(smi);

    CPPUNIT_ASSERT_EQUAL( (u32)1, dell_smi_obj_get_res(smi, cbRES1));
    CPPUNIT_ASSERT_EQUAL( (u32)1, dell_smi_obj_get_res(smi, cbRES2));
    CPPUNIT_ASSERT_EQUAL( (u32)1, dell_smi_obj_get_res(smi, cbRES3));
    CPPUNIT_ASSERT_EQUAL( (u32)1, dell_smi_obj_get_res(smi, cbRES4));

    dell_smi_obj_free(smi);
    STD_TEST_END("");
}

void testCsmi::testSmiBuffer()
{
    STD_TEST_START(getTestName().c_str() << "  ");
    struct dell_smi_obj *smi = dell_smi_factory(DELL_SMI_GET_NEW | DELL_SMI_UNIT_TEST_MODE, smi_ut_init_fn);

    dell_smi_obj_set_class(smi, 0x02);
    dell_smi_obj_set_select(smi, 0x02);
    dell_smi_obj_set_arg(smi, cbARG1, 0x02);
    dell_smi_obj_set_arg(smi, cbARG2, 0x02);
    dell_smi_obj_set_arg(smi, cbARG3, 0x02);
    dell_smi_obj_set_arg(smi, cbARG4, 0x02);

    u8 *buf = dell_smi_obj_make_buffer_frombios_auto(smi, cbARG3, 33);
    memset(buf, 'z', 32);

    CPPUNIT_ASSERT_EQUAL( (u32)0, dell_smi_obj_get_res(smi, cbRES1));
    CPPUNIT_ASSERT_EQUAL( (u32)0, dell_smi_obj_get_res(smi, cbRES2));
    CPPUNIT_ASSERT_EQUAL( (u32)0, dell_smi_obj_get_res(smi, cbRES3));
    CPPUNIT_ASSERT_EQUAL( (u32)0, dell_smi_obj_get_res(smi, cbRES4));

    dell_smi_obj_execute(smi);

    CPPUNIT_ASSERT_EQUAL( (u32)2, dell_smi_obj_get_res(smi, cbRES1));
    CPPUNIT_ASSERT_EQUAL( (u32)2, dell_smi_obj_get_res(smi, cbRES2));
    CPPUNIT_ASSERT_EQUAL( (u32)2, dell_smi_obj_get_res(smi, cbRES3));
    CPPUNIT_ASSERT_EQUAL( (u32)2, dell_smi_obj_get_res(smi, cbRES4));

    CPPUNIT_ASSERT_EQUAL( (u8)'a', buf[0] );
    CPPUNIT_ASSERT_EQUAL( (u8)'a', buf[31] );

    dell_smi_obj_free(smi);
    STD_TEST_END("");
}

// private for unit test only
extern "C" {
void set_basedir(const char *newdir);
}

void testCsmi::testLinuxSmi()
{
    STD_TEST_START(getTestName().c_str() << "  ");
#ifdef BUILD_LINUX
    struct dell_smi_obj *smi = dell_smi_factory(DELL_SMI_GET_NEW);
    CPPUNIT_ASSERT(smi);

    string d = getWritableDirectory() + "/";
    set_basedir(d.c_str());

    dell_smi_obj_set_class(smi, 0x03);
    dell_smi_obj_set_select(smi, 0x03);
    dell_smi_obj_set_arg(smi, cbARG1, 0x03);
    dell_smi_obj_set_arg(smi, cbARG2, 0x03);
    dell_smi_obj_set_arg(smi, cbARG3, 0x03);
    dell_smi_obj_set_arg(smi, cbARG4, 0x03);

    u8 *buf = dell_smi_obj_make_buffer_frombios_auto(smi, cbARG4, 32);
    memset(buf, 'z', 24);

    buf = dell_smi_obj_make_buffer_frombios_auto(smi, cbARG3, 32);
    memset(buf, 'o', 24);

    buf = dell_smi_obj_make_buffer_frombios_auto(smi, cbARG2, 32);
    memset(buf, 'm', 24);

    buf = dell_smi_obj_make_buffer_frombios_auto(smi, cbARG1, 32);
    memset(buf, 'a', 24);

    CPPUNIT_ASSERT_EQUAL( (u32)0, dell_smi_obj_get_res(smi, cbRES1));
    CPPUNIT_ASSERT_EQUAL( (u32)0, dell_smi_obj_get_res(smi, cbRES2));
    CPPUNIT_ASSERT_EQUAL( (u32)0, dell_smi_obj_get_res(smi, cbRES3));
    CPPUNIT_ASSERT_EQUAL( (u32)0, dell_smi_obj_get_res(smi, cbRES4));

    dell_smi_obj_execute(smi);

    //CPPUNIT_ASSERT_EQUAL( (u32)2, dell_smi_obj_get_res(smi, cbRES1));
    //CPPUNIT_ASSERT_EQUAL( (u32)2, dell_smi_obj_get_res(smi, cbRES2));
    //CPPUNIT_ASSERT_EQUAL( (u32)2, dell_smi_obj_get_res(smi, cbRES3));
    //CPPUNIT_ASSERT_EQUAL( (u32)2, dell_smi_obj_get_res(smi, cbRES4));

    //CPPUNIT_ASSERT_EQUAL( (u8)'a', buf[0] );
    //CPPUNIT_ASSERT_EQUAL( (u8)'a', buf[31] );

    dell_smi_obj_free(smi);
#endif
    STD_TEST_END("");
}







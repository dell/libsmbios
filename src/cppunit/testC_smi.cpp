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

// keep this struct in sync with what is in smi_impl.h
struct UT_dell_smi_obj
{
    int initialized;
    int (*execute)(struct dell_smi_obj *);
    u16 smi_class;
    u16 smi_select;
    u32 arg[4];
    u32 res[4];
    u8 *physical_buffers[4];
};

int smi_ut_exec(struct dell_smi_obj *smi)
{
    struct UT_dell_smi_obj *ut_smi = (struct UT_dell_smi_obj *)smi;

    switch(ut_smi->smi_class){
    case 0x01:
        ut_smi->res[CB_RES1] = 1;
        ut_smi->res[CB_RES2] = 2;
        ut_smi->res[CB_RES3] = 3;
        ut_smi->res[CB_RES4] = 4;
        break;
    default:
        break;
    }

    return 0;
}

int smi_ut_init_fn(struct dell_smi_obj *smi)
{
    struct UT_dell_smi_obj *ut_smi = (struct UT_dell_smi_obj *)smi;
    ut_smi->execute = smi_ut_exec;
    return 0;
}

void testCsmi::setUp()
{
    string memdumpCopyFile = setupMemoryForUnitTest(getTestDirectory(), getWritableDirectory());
    string cmosCopyFile = setupCmosForUnitTest(getTestDirectory(), getWritableDirectory());

    dell_smi_factory(DELL_SMI_GET_SINGLETON | DELL_SMI_UNIT_TEST_MODE, smi_ut_init_fn);
}

void testCsmi::tearDown()
{
}

void testCsmi::testSmiConstruct()
{
    STD_TEST_START(getTestName().c_str() << "  ");
    struct dell_smi_obj *smi =  dell_smi_factory(DELL_SMI_GET_SINGLETON);

    dell_smi_obj_set_class(smi, 0x01);
    dell_smi_obj_set_select(smi, 0x02);
    dell_smi_obj_set_arg(smi, CB_ARG1, 0x03);
    dell_smi_obj_set_arg(smi, CB_ARG2, 0x04);
    dell_smi_obj_set_arg(smi, CB_ARG3, 0x05);
    dell_smi_obj_set_arg(smi, CB_ARG4, 0x06);

    dell_smi_obj_execute(smi);

    CPPUNIT_ASSERT_EQUAL( dell_smi_obj_get_res(smi, CB_RES1), (u32)1 );
    CPPUNIT_ASSERT_EQUAL( dell_smi_obj_get_res(smi, CB_RES2), (u32)2 );
    CPPUNIT_ASSERT_EQUAL( dell_smi_obj_get_res(smi, CB_RES3), (u32)3 );
    CPPUNIT_ASSERT_EQUAL( dell_smi_obj_get_res(smi, CB_RES4), (u32)4 );

    STD_TEST_END("");
}





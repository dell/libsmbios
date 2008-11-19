/* vim:expandtab:autoindent:tabstop=4:shiftwidth=4:filetype=c:cindent: 
 */
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

/* safe to include in C headers */

#ifndef LIBSMBIOS_AUTO_LINK_H_INCLUDED
#define LIBSMBIOS_AUTO_LINK_H_INCLUDED

#ifdef __cplusplus
#  ifndef LIBSMBIOS_CONFIG_H
#     include "smbios/config.hpp"
#  endif
#elif defined(_MSC_VER) && !defined(__MWERKS__) && !defined(__EDG_VERSION__)
/*
 * C language compatability (no, honestly)
 */
#  define BOOST_MSVC _MSC_VER
#  define BOOST_STRINGIZE(X) BOOST_DO_STRINGIZE(X)
#  define BOOST_DO_STRINGIZE(X) #X
#endif

#if (defined(LIBSMBIOS_PLATFORM_WIN32) || defined(LIBSMBIOS_PLATFORM_WIN64)) && defined(_MSC_VER)
// The rest of the code in this file is used to automatically select which 
// version libsmbios library to link to.

// libsmbios-vc6        // single-thread  (DLL)     (non-debug)     XXX
// libsmbios-vc6-mt     // multi-thread   (DLL)     (non-debug)     mt-dll
// libsmbios-vc6-gd     // single-thread  (DLL)     (debug)         XXX
// libsmbios-vc6-mt-gd  // multi-thread   (DLL)     (debug)         d-mt-dll
// libsmbios-vc6-s      // single-thread  (non-DLL) (non-debug)     st
// libsmbios-vc6-mt-s   // multi-thread   (non-DLL) (non-debug)     mt
// libsmbios-vc6-sgd    // single-thread  (non-DLL) (debug)         d-st
// libsmbios-vc6-mt-sgd // multi-thread   (non-DLL) (debug)         d-mt



#if !defined(LIBSMBIOS_ALL_NO_LIB) && !defined(LIBSMBIOS_SOURCE)


#if defined(LIBSMBIOS_ALL_DYN_LINK)
#   define LIBSMBIOS_DYN_LINK
#endif

// ==============================
//
// select toolset if not defined already:
//
#ifndef LIBSMBIOS_LIB_TOOLSET
#if defined(LIBSMBIOS_MSVC) && (LIBSMBIOS_MSVC == 1200)

   // vc6:
#  define LIBSMBIOS_LIB_TOOLSET "vc6"

#elif defined(LIBSMBIOS_MSVC) && (LIBSMBIOS_MSVC == 1300)

   // vc7:
#  define LIBSMBIOS_LIB_TOOLSET "vc7"

#elif defined(LIBSMBIOS_MSVC) && (LIBSMBIOS_MSVC == 1310)

   // vc71:
#  define LIBSMBIOS_LIB_TOOLSET "vc71"

#elif defined(LIBSMBIOS_MSVC) && (LIBSMBIOS_MSVC >= 1400)

   // vc80:
#  define LIBSMBIOS_LIB_TOOLSET "vc80"
#endif
#endif


// ==============================
//
// select linkage opt:
//
#if (defined(_DLL) || defined(_RTLDLL)) && defined(LIBSMBIOS_DYN_LINK)
#  define LIBSMBIOS_LIB_PREFIX
#elif defined(LIBSMBIOS_DYN_LINK)
#  error "Mixing a dll boost library with a static runtime is a really bad idea..."
#else
#  define LIBSMBIOS_LIB_PREFIX "lib"
#endif


// ==============================
//
// select thread opt:
//
#if defined(_MT) || defined(__MT__)
#  define LIBSMBIOS_LIB_THREAD_OPT "-mt"
#else
#  define LIBSMBIOS_LIB_THREAD_OPT
#endif

// ==============================
//
// select runtime opt:
//
#if defined(_MSC_VER) || defined(__MWERKS__)
#  ifdef _DLL
#     if (defined(__SGI_STL_PORT) || defined(_STLPORT_VERSION)) && (defined(_STLP_OWN_IOSTREAMS) || defined(__STL_OWN_IOSTREAMS))

#        if defined(_DEBUG) && (defined(__STL_DEBUG) || defined(_STLP_DEBUG))
#            define LIBSMBIOS_LIB_RT_OPT "-gdp"
#        elif defined(_DEBUG)
#            define LIBSMBIOS_LIB_RT_OPT "-gdp"
#            pragma message("warning: STLPort debug versions are built with /D_STLP_DEBUG=1")
#            error "Build options aren't compatible with pre-built libraries"
#        else
#            define LIBSMBIOS_LIB_RT_OPT "-p"
#        endif

#     elif defined(__SGI_STL_PORT) || defined(_STLPORT_VERSION)

#        if defined(_DEBUG) && (defined(__STL_DEBUG) || defined(_STLP_DEBUG))
#            define LIBSMBIOS_LIB_RT_OPT "-gdpn"
#        elif defined(_DEBUG)
#            define LIBSMBIOS_LIB_RT_OPT "-gdpn"
#            pragma message("warning: STLPort debug versions are built with /D_STLP_DEBUG=1")
#            error "Build options aren't compatible with pre-built libraries"
#        else
#            define LIBSMBIOS_LIB_RT_OPT "-pn"
#        endif

#     else

#        if defined(_DEBUG)
#            define LIBSMBIOS_LIB_RT_OPT "-gd"
#        else
#            define LIBSMBIOS_LIB_RT_OPT
#        endif

#     endif
#  else

#     if (defined(__SGI_STL_PORT) || defined(_STLPORT_VERSION)) && (defined(_STLP_OWN_IOSTREAMS) || defined(__STL_OWN_IOSTREAMS))

#        if defined(_DEBUG) && (defined(__STL_DEBUG) || defined(_STLP_DEBUG))
#            define LIBSMBIOS_LIB_RT_OPT "-sgdp"
#        elif defined(_DEBUG)
#             define LIBSMBIOS_LIB_RT_OPT "-sgdp"
#            pragma message("warning: STLPort debug versions are built with /D_STLP_DEBUG=1")
#            error "Build options aren't compatible with pre-built libraries"
#        else
#            define LIBSMBIOS_LIB_RT_OPT "-sp"
#        endif

#     elif defined(__SGI_STL_PORT) || defined(_STLPORT_VERSION)

#        if defined(_DEBUG) && (defined(__STL_DEBUG) || defined(_STLP_DEBUG))
#            define LIBSMBIOS_LIB_RT_OPT "-sgdpn"
#        elif defined(_DEBUG)
#             define LIBSMBIOS_LIB_RT_OPT "-sgdpn"
#            pragma message("warning: STLPort debug versions are built with /D_STLP_DEBUG=1")
#            error "Build options aren't compatible with pre-built libraries"
#        else
#            define LIBSMBIOS_LIB_RT_OPT "-spn"
#        endif

#     else

#        if defined(_DEBUG)
#             define LIBSMBIOS_LIB_RT_OPT "-sgd"
#        else
#            define LIBSMBIOS_LIB_RT_OPT "-s"
#        endif

#     endif

#  endif
#endif

#define LIBSMBIOS_LIB_NAME smbios

#ifndef LIBSMBIOS_AUTO_LINK_NOMANGLE
#   pragma comment(lib, LIBSMBIOS_LIB_PREFIX LIBSMBIOS_STRINGIZE(LIBSMBIOS_LIB_NAME) "-" LIBSMBIOS_LIB_TOOLSET LIBSMBIOS_LIB_THREAD_OPT LIBSMBIOS_LIB_RT_OPT ".lib" )
#   ifdef LIBSMBIOS_LIB_DIAGNOSTIC
#       pragma message ("Automatically Linking to lib file: " LIBSMBIOS_LIB_PREFIX LIBSMBIOS_STRINGIZE(LIBSMBIOS_LIB_NAME) "-" LIBSMBIOS_LIB_TOOLSET LIBSMBIOS_LIB_THREAD_OPT LIBSMBIOS_LIB_RT_OPT ".lib")
#   endif
#   if defined(LIBSMBIOS_NEED_SMBIOSXML)
#       undef LIBSMBIOS_LIB_NAME
#       define LIBSMBIOS_LIB_NAME smbiosxml
#       pragma comment(lib, LIBSMBIOS_LIB_PREFIX LIBSMBIOS_STRINGIZE(LIBSMBIOS_LIB_NAME) "-" LIBSMBIOS_LIB_TOOLSET LIBSMBIOS_LIB_THREAD_OPT LIBSMBIOS_LIB_RT_OPT ".lib" )
#       ifdef LIBSMBIOS_LIB_DIAGNOSTIC
#           pragma message ("Automatically Linking to lib file: " LIBSMBIOS_LIB_PREFIX LIBSMBIOS_STRINGIZE(LIBSMBIOS_LIB_NAME) "-" LIBSMBIOS_LIB_TOOLSET LIBSMBIOS_LIB_THREAD_OPT LIBSMBIOS_LIB_RT_OPT ".lib")
#       endif
#   endif
#else
#   pragma comment(lib, LIBSMBIOS_STRINGIZE(LIBSMBIOS_LIB_NAME) ".lib" )
#   ifdef LIBSMBIOS_LIB_DIAGNOSTIC
#       pragma message ("Automatically Linking to lib file: " LIBSMBIOS_STRINGIZE(LIBSMBIOS_LIB_NAME) ".lib")
#   endif
#   if defined(LIBSMBIOS_NEED_SMBIOSXML)
#       undef LIBSMBIOS_LIB_NAME
#       define LIBSMBIOS_LIB_NAME smbiosxml
#       pragma comment(lib, LIBSMBIOS_STRINGIZE(LIBSMBIOS_LIB_NAME) ".lib" )
#       ifdef LIBSMBIOS_LIB_DIAGNOSTIC
#           pragma message ("Automatically Linking to lib file: " LIBSMBIOS_STRINGIZE(LIBSMBIOS_LIB_NAME) ".lib")
#       endif
#   endif
#endif

// automatic linking for xerces, if we are using XML
#if defined(LIBSMBIOS_NEED_SMBIOSXML)
#   undef LIBSMBIOS_LIB_NAME
#   if !defined(_MT) && !defined(__MT__) 
#       error("Xerces DLLs are only distributed as (debug|non-debug) x (multithreaded|multithreaded DLL). Your runtime code-generation must be set to either of these configurations.")
#   endif

#   if (defined(_DLL) || defined(_RTLDLL)) 
#       if defined(_DEBUG)
#           define LIBSMBIOS_LIB_NAME xerces-c_2D
#       else
#           define LIBSMBIOS_LIB_NAME xerces-c_2
#       endif
#   else
#       if defined(_DEBUG)
#           define LIBSMBIOS_LIB_NAME Xerces-c_static_2D
#       else
#           define LIBSMBIOS_LIB_NAME Xerces-c_static_2
#       endif
#   endif
#   pragma comment(lib, LIBSMBIOS_STRINGIZE(LIBSMBIOS_LIB_NAME) ".lib" )
#   ifdef LIBSMBIOS_LIB_DIAGNOSTIC
#       pragma message ("Automatically Linking to lib file: " LIBSMBIOS_STRINGIZE(LIBSMBIOS_LIB_NAME) ".lib")
#   endif
#endif

//  //#include <config/auto_link.hpp>
#endif /* !LIBSMBIOS_ALL_NO_LIB && ! LIBSMBIOS_SOURCE */
#endif /* LIBSMBIOS_PLATFORM_WIN32 */

#ifdef LIBSMBIOS_LIB_PREFIX
#  undef LIBSMBIOS_LIB_PREFIX
#endif
#if defined(LIBSMBIOS_LIB_NAME)
#  undef LIBSMBIOS_LIB_NAME
#endif
#if defined(LIBSMBIOS_LIB_TOOLSET)
#  undef LIBSMBIOS_LIB_TOOLSET
#endif
#if defined(LIBSMBIOS_LIB_THREAD_OPT)
#  undef LIBSMBIOS_LIB_THREAD_OPT
#endif
#if defined(LIBSMBIOS_LIB_RT_OPT)
#  undef LIBSMBIOS_LIB_RT_OPT
#endif
#if defined(LIBSMBIOS_LIB_LINK_OPT)
#  undef LIBSMBIOS_LIB_LINK_OPT
#endif
#if defined(LIBSMBIOS_LIB_DEBUG_OPT)
#  undef LIBSMBIOS_LIB_DEBUG_OPT
#endif
#if defined(LIBSMBIOS_DYN_LINK)
#  undef LIBSMBIOS_DYN_LINK
#endif
#if defined(LIBSMBIOS_AUTO_LINK_NOMANGLE)
#  undef LIBSMBIOS_AUTO_LINK_NOMANGLE
#endif

#endif /* AUTO_LINK_H */

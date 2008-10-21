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


#ifndef _STDFUNCS_H
#define _STDFUNCS_H

void setupForUnitTesting(std::string testdir, std::string writedir);
void reset();

std::string &strip_trailing_whitespace(std::string &s);
void copyFile( std::string dstFile, std::string srcFile );
bool fileExists(std::string fileName);
size_t FWRITE(const void *ptr, size_t size, size_t nmemb, FILE *stream);

#endif  /* ! defined _STDFUNCS_H */

/*
 * Copyright (c) 2016, Huamin Chen
 * Inspired by PL/R
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or (at
 * your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser
 * General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef R_H_
#define R_H_

#ifdef STANDARD
/* STANDARD is defined, don't use any mysql functions */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef __WIN__
typedef unsigned __int64 ulonglong;	/* Microsofts 64 bit types */
typedef __int64 longlong;
#else
typedef unsigned long long ulonglong;
typedef long long longlong;
#endif /*__WIN__*/
#else
#include <my_global.h>
#include <my_sys.h>
#include <m_string.h>
#endif
#include <mysql.h>
#include <ctype.h>

#define R_INTERFACE_PTRS
#define CSTACK_DEFNS
#include <R.h>
#include <Rversion.h>
#include <Rinternals.h>
#include <Rdefines.h>

#define HAVE_UINTPTR_T
#include <stdint.h>
#include <Rinterface.h>
#include <Rembedded.h>
#include <R_ext/Parse.h>
#include <R_ext/Callbacks.h>
#include <R_ext/Rdynload.h>
#include <R_ext/Memory.h>

void initR();
void deinitR();
SEXP call_r_func(SEXP fun, SEXP rargs);
SEXP convert_args(UDF_ARGS *args, char *is_null);
SEXP parse_func_body(const char *body);

struct r_data {
    SEXP fun;
	SEXP rargs;
	SEXP rvalue;
};

#define R_PARSEVECTOR(a_, b_, c_)		R_ParseVector(a_, b_, (ParseStatus *) c_, R_NilValue)
#define LOG_ERR(s) fprintf(stderr, "%s\n", s);

#endif /* R_H_ */

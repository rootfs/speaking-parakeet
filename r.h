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


#include "R.h"
#include "Rversion.h"


#if (R_VERSION >= 132096) /* R_VERSION >= 2.4.0 */
#include "Rembedded.h"
#endif
#if !defined(WIN32) && !defined(WIN64)
#include "Rinterface.h"
#else
extern int R_SignalHandlers;
#endif
#include "Rinternals.h"
#include "Rdefines.h"
#if (R_VERSION < 133120) /* R_VERSION < 2.8.0 */
#include "Rdevices.h"
#endif
#include "R_ext/Parse.h"

#ifdef ERROR
#undef ERROR
#endif

#ifdef WARNING
#undef WARNING
#endif

#define WARNING		1
#define ERROR		2
#define true        1
#define false       0

char *strncpy_alloc(const char *str, unsigned long length);
void **ptr_calloc(size_t nelem, size_t elsize);
void ptr_free(void **ptr);
int strncmp_caseins(char *str1, char *str2, size_t num);
int charinstr(char *str, char c, size_t num);

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


#define TRIM_BACKQUOTE(fnull) (fnull+(int)(fnull[0]=='`'))		// Skip starting backquote
#define RETURN_ERR(msg) { strcpy(message, msg); return 1; }		// Set error message and return in %_init functions
#define ATTRIBUTE_COMPARE(i, str, len) (args->attribute_lengths[i] == len && strncmp_caseins(args->attributes[i], str, len) == 0)

#define LOG_ERR(s) fprintf(stderr, "%s\n", s);

#endif /* R_H_ */

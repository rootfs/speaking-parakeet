/**
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
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

#include "r.h"

/**
 * Allocate unused space for a string with a specific size and null terminate it.
 * The string does not need to be null terminated.
 */
char *strncpy_alloc(const char *str, unsigned long length)
{
	if (str == NULL) return NULL;

	char *newstr = (char *)malloc((length+1) * sizeof(char));
	if (newstr == NULL) return NULL;

	strncpy(newstr, str, length);
	newstr[length] = '\0';

	return newstr;
}

/**
 * Allocate unused space for an array of nelem elements each of whose size in bytes is elsize.
 * The space shall be initialized to all bits 0.
 */
void **ptr_calloc(size_t nelem, size_t elsize)
{
	void **ptr = (void **)malloc(nelem * elsize + sizeof(int));
	if (ptr == NULL) return NULL;

	*(int *)ptr = nelem;

	ptr = (void **)((int*)ptr + 1);
	memset(ptr, 0, nelem * elsize);

	return ptr;
}

/**
 * Free allocated space of ptr and the space of all items from ptr.
 * Only use with mem allocated with ptr_calloc.
 */
void ptr_free(void **ptr)
{
    int i;
	for (i=0; i < *((int *)ptr - 1); i++) {
		if (ptr[i]) free(ptr[i]);
	}

	free((int*)ptr-1);
}

/**
 * Compare 2 (not \0 term) strings case insensative, specifying the length
 */
int strncmp_caseins(char *str1, char *str2, size_t num)
{
	char c1, c2;
    int i;

	for (i=0; i<num; i++) {
		c1 = (str1[i] >= 65 && str1[i] <= 90) ? str1[i] + 32 : str1[i]; /* Change to lower case */
		c2 = (str2[i] >= 65 && str2[i] <= 90) ? str2[i] + 32 : str2[i]; /* Change to lower case */

		if (c1 != c2) return (c1 < c2) * -2 + 1;   /* Could have used q?a:b, but... nerd power */
	}

	return 0;
}

/**
 * Check if a char appears in a string, specifying the length
 */
int charinstr(char *str, char c, size_t num)
{
    int i;

	for (i=0; i<num && str[i]; i++) {
		if (str[i] == c) return i;
	}

	return -1;
}

void initR()
{
    char *argv[] = {"REmbeddedUDF", "--gui=none", "--silent"};
    int argc = sizeof(argv)/sizeof(argv[0]);
    R_SignalHandlers = 0;
    Rf_initEmbeddedR(argc, argv);
    R_Interactive = 0;
}

SEXP call_r_func(SEXP fun, SEXP rargs)
{
	int		i;
	int		errorOccurred;
	SEXP	obj,
			args,
			call,
			ans;
	long	n = length(rargs);

	if(n > 0)
	{
		PROTECT(obj = args = allocList(n));
		for (i = 0; i < n; i++)
		{
			SETCAR(obj, VECTOR_ELT(rargs, i));
			obj = CDR(obj);
		}
		UNPROTECT(1);
        /*
         * NB: the headers of both R and Postgres define a function
         * called lcons, so use the full name to be precise about what
         * function we're calling.
         */
		PROTECT(call = Rf_lcons(fun, args));
	}
	else
	{
		PROTECT(call = allocVector(LANGSXP,1));
		SETCAR(call, fun);
	}

	ans = R_tryEval(call, R_GlobalEnv, &errorOccurred);
	UNPROTECT(1);

	if(errorOccurred)
	{
        LOG_ERR("failed to call r func");
	}
	return ans;
}

SEXP convert_args(UDF_ARGS *args, char *is_null)
{
    SEXP rargs;
    PROTECT(rargs = allocVector(VECSXP, args->arg_count));
    UNPROTECT(1);
    return rargs;
}

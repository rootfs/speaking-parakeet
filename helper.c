/*
 * Copyright (C) 2016, Huamin Chen
 *
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

#include "r.h"

#ifndef R_HOME
#define R_HOME ""
#endif

#ifndef R_HOME_DEFAULT
#define R_HOME_DEFAULT ""
#endif
void initR()
{
    char *argv[] = {"REmbeddedUDF", "--slave",  "--silent", "--vanilla", "--no-readline"};
    int argc = sizeof(argv)/sizeof(argv[0]);
	/* refuse to start if R_HOME is not defined */
	char *r_home = getenv("R_HOME");
    char *rhenv = NULL;

	if (r_home == NULL)
	{
		size_t	rh_len = strlen(R_HOME_DEFAULT);

		/* see if there is a compiled in default R_HOME */
		if (rh_len)
		{
			rhenv = malloc(8 + rh_len);

			sprintf(rhenv, "R_HOME=%s", R_HOME_DEFAULT);
			putenv(rhenv);
		}
    }
    R_SignalHandlers = 0;
    /* Don't do any stack checking */
#if (R_VERSION > R_Version(2, 2, 9))
    Rf_initialize_R(argc, argv);
    R_CStackLimit = -1;
    setup_Rmainloop ();
#else
    Rf_initEmbeddedR(argc, argv);
#endif
 
    if (rhenv) {
        free(rhenv);
    }
}

void deinitR()
{
    Rf_endEmbeddedR(0);
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
    int i;
    PROTECT(rargs = allocVector(VECSXP, args->arg_count - 1));
    for (i = 1; i < args->arg_count; i ++)
    {
        switch (args->arg_type[i]){
        case INT_RESULT:
            if (i == 1) {
                SETCAR(rargs, ScalarInteger((int)*args->args[i]));
            } else {
                SETCADR(rargs, ScalarInteger((int)*args->args[i]));
            }
            break;
        }
    }
    UNPROTECT(1);
    return rargs;
}

SEXP parse_func_body(const char *body)
{
	SEXP	rbody;
	SEXP	fun;
    SEXP	tmp;
	int		status;

	PROTECT(rbody = mkString(body));
	PROTECT(tmp = R_PARSEVECTOR(rbody, -1, &status));

	if (tmp != R_NilValue)
		PROTECT(fun = VECTOR_ELT(tmp, 0));
	else
		PROTECT(fun = R_NilValue);

	if (status != PARSE_OK)
	{
		UNPROTECT(3);
        LOG_ERR(body);
        return NULL;
	}

	UNPROTECT(3);
	return(fun);
}

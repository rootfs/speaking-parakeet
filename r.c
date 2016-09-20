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

#ifdef	__cplusplus
extern "C" {
#endif
	 my_bool r_ext_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
	 void r_ext_deinit(UDF_INIT *initid);
	 char *r_ext(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error);
#ifdef	__cplusplus
}
#endif

my_bool r_ext_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
    if (args->arg_count < 1) {
        strcpy(message, "no script provided");
        return 1;
    }
    if (args->arg_type[0] != STRING_RESULT) {
        strcpy(message, "argument 0 must be result");
        return 1;
    }
    initid->ptr = (char *)malloc(sizeof(struct r_data));
    if (initid->ptr == NULL) {
        strcpy(message, "failed to allocate ptr");
        return 1;
    }
    initR();
	return 0;
}

void r_ext_deinit(UDF_INIT *initid)
{
     if (initid->ptr == NULL)
         return;
     free(initid->ptr);
     initid->ptr = NULL;
     deinitR();
}

char* r_ext(UDF_INIT *initid, UDF_ARGS *args, char* result, unsigned long* length,	char *is_null, char *error)
{
    struct r_data *data = NULL;
    char *body = (char *)args->args[0];

    data = (struct r_data *)initid->ptr;
    PROTECT(data->fun = parse_func_body(body));
	/* Convert all call arguments */
    PROTECT(data->rargs = convert_args(args, is_null));
	/* Call the R function */
	PROTECT(data->rvalue = call_r_func(data->fun, data->rargs));
	UNPROTECT(3);

	strcpy(result, PACKAGE_STRING);
	*length = strlen(PACKAGE_STRING);
	return result;
}


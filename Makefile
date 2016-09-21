ifdef R_HOME
r_libdir1x = ${R_HOME}/bin
r_libdir2x = ${R_HOME}/lib
# location of R includes
r_includespec = -I${R_HOME}/include
rhomedef = ${R_HOME}
else
R_HOME := $(shell pkg-config --variable=rhome libR)
r_libdir1x := $(shell pkg-config --variable=rlibdir libR)
r_libdir2x := $(shell pkg-config --variable=rlibdir libR)
r_includespec := $(shell pkg-config --cflags-only-I libR)
rhomedef := $(shell pkg-config --variable=rhome libR)
endif

CFLAGS := $(r_includespec) -DR_HOME_DEFAULT=\"$(rhomedef)\" -DR_HOME="$(R_HOME)" $(CFLAGS)

all: r.c helper.c r.h
	gcc $(CFLAGS) -I/usr/include/mysql -Wall -Wpointer-arith  -Wendif-labels -Wmissing-format-attribute -Wformat-security -fno-strict-aliasing -fwrapv -fexcess-precision=standard -O2 -g -pipe -Werror=format-security -fexceptions -fstack-protector-strong --param=ssp-buffer-size=4 -grecord-gcc-switches -m64 -mtune=generic -fpic -shared -Wl,-z,relro   -Wl,--as-needed    -DMYSQL_DYNAMIC_PLUGIN -Wl,-z,relro   -Wl,--as-needed  -o mysqlr.so r.c helper.c -L/usr/lib64/mysql -lmysqlclient -L/usr/lib64/R/lib -lR

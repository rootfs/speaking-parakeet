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

CFLAGS := $(r_includespec) -DR_HOME_DEFAULT=$(rhomedef) $(CFLAGS)

all: r.c helper.c r.h
	gcc -Wall $(CFLAGS) -I/usr/include/mysql -shared -fPIC -DMYSQL_DYNAMIC_PLUGIN -o mysqlr.so r.c helper.c -L/usr/lib64/mysql -lmysqlclient -L/usr/lib64/R/lib -lR

#!/bin/bash
rm -rf autom4te.cache
rm -f  m4/libtool.m4 m4/ltsugar.m4 m4/ltversion.m4 m4/lt~obsolete.m4 m4/ltoptions.m4
rm -f  ltmain.sh aclocal.m4 ylwrap depcomp missing install-sh compile
rm -f  config.sub config.guess
rm -f  config.h.in configure Makefile.in

libtoolize --force
autoreconf --install --verbose --force

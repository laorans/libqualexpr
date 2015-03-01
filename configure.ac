dnl initial information about the project
AC_INIT([libqualexpr],m4_esyscmd([tr -d '\n' <VERSION]),[lm.github@laorans.com])

dnl automake initialisation 
AM_INIT_AUTOMAKE([])
AM_SILENT_RULES([yes])

dnl check if the source folder is correct
AC_CONFIG_SRCDIR([src/QualityExpressions.cc])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])

dnl setting extra locations for libraries and headers
AMCFLAGS="$CFLAGS"
AMCXXFLAGS="$CXXFLAGS"
AMLDFLAGS="$LDFLAGS"

dnl check for C++ preprocessor and compiler and the library compiler if not yet detected by platform checks

AC_CHECK_PROGS(YACC, 'bison -y' byacc yacc, :)
if test "$YACC" = ":"; then
  AC_MSG_ERROR([ERROR: Required program YACC is missing.])
else
  AC_SUBST([BISON_VERSION],[`${YACC} --version | egrep '[[0-9.-]]+[[0-9]]$' | sed -e s/'.* \([[0-9.-]]*[[0-9]]\)$'/'\1'/ -e s/[[.-]]//g`])
  AC_MSG_NOTICE([BISON_VERSION=${BISON_VERSION}])
fi

AC_CHECK_PROGS(LEX, flex lex, :)
if test "$LEX" = ":"; then
  AC_MSG_ERROR([ERROR: Required program LEX is missing.])
fi

AC_PROG_YACC
AM_PROG_LEX

AC_PROG_CXXCPP
AC_PROG_CPP
AC_PROG_CXX

dnl initialize libtool
LT_INIT

dnl use the C++ compiler for the following checks
AC_LANG([C++])

dnl custom environment checks
AC_QE_LIBBOOST
AC_QE_LIBPAPI
AC_QE_LIBQUALEXPR

dnl distribute additional compiler and linker flags
AC_SUBST([AM_CXXFLAGS])
AC_SUBST([AM_CFLAGS])
AC_SUBST([AM_LDFLAGS])
AC_SUBST([lib_LIBRARIES])
AC_SUBST([lib_LTLIBRARIES])
AC_SUBST([bin_PROGRAMS])

dnl files to generate via autotools (.am or .in source files)
AC_CONFIG_FILES([Makefile])

dnl generate the final Makefile etc.
AC_OUTPUT

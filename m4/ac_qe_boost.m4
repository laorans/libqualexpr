AC_DEFUN([AC_QE_LIBBOOST], [

AC_MSG_NOTICE([Checking for BOOST ...])

AC_ARG_VAR([BOOST_INC], [Include path to the Boost headers.])
AC_ARG_VAR([BOOST_LIB], [Library path to the Boost library.])

dnl check the header
AC_ARG_WITH([boost-header],
            [AS_HELP_STRING([--with-boost-header=<path-to-boost-headers>], 
                            [If boost headers are not installed in the default location, specify the dirname where it can be found.])],
            [qe_boost_inc_dir="${withval}"],  
            [qe_boost_inc_dir="${BOOST_INC-}"])

AS_IF([test "x$qe_boost_inc_dir" != "x"], [
    qe_boost_cppflags="-I$qe_boost_inc_dir"
], [
    qe_boost_cppflags=""
])

AC_LANG_PUSH([C++])
cppflags_save="$CPPFLAGS"

CPPFLAGS="$qe_boost_cppflags $CPPFLAGS"
AC_CHECK_HEADER([boost/serialization/list.hpp],
                [qe_boost_header="yes"],
                [qe_boost_header="no"])
CPPFLAGS="$cppflags_save"
AC_LANG_POP([C++])

dnl check the library
AC_ARG_WITH([boost-lib],
            [AS_HELP_STRING([--with-boost-lib=<path-to-libboost*>], 
                            [If libboost* are not installed in the default location, specify the dirname where it can be found.])],
            [qe_boost_lib_dir="${withval}"],  
            [qe_boost_lib_dir="${BOOST_LIB-}"] 
)
AC_LANG_PUSH([C++])
ldflags_save="$LDFLAGS"
if test "x${qe_boost_lib_dir}" = "x"; then
LDFLAGS="-lboost_serialization $LDFLAGS"
else
LDFLAGS="-L$qe_boost_lib_dir -lboost_serialization $LDFLAGS"
fi
qe_boost_lib_name="-lboost_serialization -lboost_program_options"

if test "x${qe_boost_header}" = "xyes"; then
qe_boost_library="yes"
else
qe_boost_library="no"
fi
LDFLAGS="$ldflags_save"
AC_LANG_POP([C++])

dnl Final check
AC_MSG_CHECKING([for boost support])
AC_MSG_RESULT([$qe_have_boost])
if test "x${qe_have_boost}" = "xyes"; then
    AC_DEFINE([HAVE_BOOST], [1],     [Defined if libboost is available.])
if test "x${qe_boost_lib_dir}" != "x"; then
    AC_SUBST([QE_BOOST_LDFLAGS], [-L${qe_boost_lib_dir}])
else
    AC_SUBST([QE_BOOST_LDFLAGS], [${qe_boost_lib_name}])
fi
    AC_SUBST([QE_BOOST_LIBS],    [${qe_boost_lib_name}])
else
    AC_MSG_NOTICE([Warning: library BOOST not available.])
fi
AM_CONDITIONAL([HAVE_BOOST],        [test "x${qe_have_boost}" = "xyes"])
AC_SUBST([QE_BOOST_CPPFLAGS],       [${qe_boost_cppflags}])
AC_SUBST([qe_boost_cppflags],       [${qe_boost_cppflags}])
AC_SUBST([QE_BOOST_LIBDIR],         [${qe_boost_lib_dir}])

])

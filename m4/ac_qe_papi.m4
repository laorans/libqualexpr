AC_DEFUN([AC_QE_LIBPAPI], [

  AC_MSG_NOTICE([Checking for PAPI ...])

  AC_ARG_ENABLE([papi], AS_HELP_STRING([--enable-papi], [Enable the use of PAPI counters. Options: yes no (disabled by default)]))

  papi_enabled=no

  if test x"${enable_papi}" = xyes; then
    echo "The use of PAPI counters is enabled."
    enable_papi="-D_WITH_PAPI"
    papi_enabled=yes
    if test "x${qe_have_papi}" = "xno"; then
      AC_MSG_ERROR([ERROR: The use of PAPI counters is enabled but a PAPI library not available.])
      exit -1
    fi
  else
    echo "The use of PAPI counters is disabled."
    enable_papi=""
    papi_enabled=no
  fi

  AC_SUBST([QE_PAPI], [${enable_papi}])

  dnl advertise the $PAPI_INC and $PAPI_LIB variables in the --help output
  AC_ARG_VAR([PAPI_INC], [Include path to the PAPI headers.])
  AC_ARG_VAR([PAPI_LIB], [Library path to the PAPI library.])

  dnl check the header
  AC_ARG_WITH([papi-header],
              [AS_HELP_STRING([--with-papi-header=<path-to-papi-headers>],
                              [If PAPI headers are not installed in the default location, specify the dirname where it can be found.])],
              [qe_papi_inc_dir="${withval}"],
              [qe_papi_inc_dir="${PAPI_INC-}"])
  AS_IF([test "x$qe_papi_inc_dir" != "x"],
        [qe_papi_cppflags="-I$qe_papi_inc_dir"],
        [qe_papi_cppflags=""])

  AC_LANG_PUSH([C++])
  cppflags_save="$CPPFLAGS"

  CPPFLAGS="$qe_papi_cppflags $CPPFLAGS"
  AC_CHECK_HEADER([papi.h],
                  [qe_papi_header="yes"],
                  [qe_papi_header="no"])
  CPPFLAGS="$cppflags_save"
  AC_LANG_POP([C++])


  dnl check the library
  AC_ARG_WITH([papi-lib],
              [AS_HELP_STRING([--with-papi-lib=<path-to-libpapi*>],
                              [If libpapi* are not installed in the default location, specify the dirname where it can be found.])],
              [qe_papi_lib_dir="${withval}"],
              [qe_papi_lib_dir="${PAPI_LIB-}"])
  AC_LANG_PUSH([C++])
  ldflags_save="$LDFLAGS"
  if test "x${qe_papi_lib_dir}" = "x"; then
    LDFLAGS="-lpapi -lpfm $LDFLAGS"
  else
    LDFLAGS="-L$qe_papi_lib_dir -lpapi -lpfm $LDFLAGS"
  fi
  qe_papi_lib_name="-lpapi -lpfm"
  LDFLAGS="$ldflags_save"
  AC_LANG_POP([C++])


  dnl final check
  qe_have_papi="no"
  if test "x${qe_papi_header}" = "xyes" && test "x${qe_papi_library}" = "xyes"; then
    qe_have_papi="yes"
  else
    qe_papi_lib_name=""
  fi

  AC_MSG_CHECKING([for papi support])
  AC_MSG_RESULT([$qe_have_papi])
  if test "x${qe_have_papi}" = "xyes"; then
    AC_DEFINE([HAVE_PAPI], [1],     [Defined if libpapi is available.])

    if test "x${qe_papi_lib_dir}" != "x"; then
      AC_SUBST([QE_PAPI_LDFLAGS], [-L${qe_papi_lib_dir}])
      AC_SUBST([PAPI_LIB_IN_QE_INSTRUMENT], ["-L${qe_papi_lib_dir} ${qe_papi_lib_name}"])
    else
      AC_SUBST([QE_PAPI_LIBS], [${qe_papi_lib_name}])
      AC_SUBST([PAPI_LIB_IN_QE_INSTRUMENT], ["${qe_papi_lib_name}"])
    fi
  else
    AC_SUBST([PAPI_LIB_IN_QE_INSTRUMENT], [""])
    AC_MSG_NOTICE([WARNING: PAPI library is missing; some features will not be enabled.])
  fi

  AM_CONDITIONAL([HAVE_PAPI], [test "x${qe_have_papi}" = "xyes"])
  AC_SUBST([QE_PAPI_CPPFLAGS], [${qe_papi_cppflags}])
  AC_SUBST([QE_PAPI_LIBDIR], [${qe_papi_lib_dir}])
  AC_SUBST([QE_PAPI_LIB], ["${qe_papi_lib_dir}/libpapi.a ${qe_papi_lib_dir}/libpfm.a"])

])

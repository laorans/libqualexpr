AC_DEFUN([AC_QE_LIBQUALEXPR], [

  AC_ARG_ENABLE([qualexpr], AS_HELP_STRING([--enable-qualexpr], [Enable the core quality expression library. Options: yes no (enabled by default)]))

  qualexpr_flag=yes

  if test x"${enable_qualexpr}" = xno; then
    echo "Warning : The use of Quality expressions is disabled."
    enable_qualexpr=""
    qualexpr_flag=no
    qualexpr_libs=""
  else
    AC_DEFINE([_WITH_QUALEXPR], [1],     [Defined if Quality Expressions are activated.])
    enable_qualexpr="-D_WITH_QUALEXPR"
    qualexpr_flag=yes
    qualexpr_libs="-lqualexpr"
  fi

  AC_SUBST([QUALEXPR_LIB_IN_QE_INSTRUMENT], ["${qualexpr_libs}"])
  AC_SUBST([_WITH_QUALEXPR], [${enable_qualexpr}])
  AM_CONDITIONAL([QE_QUALEXPR_ENABLE], [test x"${qualexpr_flag}" = xyes ])
  AC_DEFINE([_WITH_QUALEXPR_STANDALONE], [1],     [Defined if Quality Expressions is compiled standalone.])

])

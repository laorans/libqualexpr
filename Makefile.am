AUTOMAKE_OPTIONS = subdir-objects -Wno-portability

ACLOCAL_AMFLAGS= -I m4

AM_YFLAGS = -d
AM_LFLAGS = --header-file=conf_scanner.hpp

lib_LTLIBRARIES += libqualexpr.la
libqualexpr_la_REVISION:=$(shell cd $(top_srcdir) ; git log -n 1 --format="format:-%t" quality_expressions 2>/dev/null)
libqualexpr_la_TAG:=$(shell head -1 $(top_srcdir)/VERSION)$(libqualexpr_la_REVISION)
libqualexpr_la_VERSION_MAJOR:=$(shell echo $(libqualexpr_la_TAG) | cut -d. -f1)
libqualexpr_la_VERSION_MINOR:=$(shell echo $(libqualexpr_la_TAG) | cut -d. -f2 | cut -d- -f1)
libqualexpr_la_VERSION:=$(libqualexpr_la_VERSION_MAJOR):$(libqualexpr_la_VERSION_MINOR):0

libqualexpr_la_CXXFLAGS = ${global_compiler_flags} \
                          ${QE_BOOST_CPPFLAGS} \
                          ${QE_PAPI} \
                          ${QE_PAPI_CPPFLAGS} \
                          -rdynamic -Wl,-export-dynamic \
                          -I$(top_srcdir)/include \
                          -I$(top_srcdir)/src/qualexpr-profiler/include \
                          -I$(top_srcdir)/src/qualexpr-evaluator/include \
                          -DTAG=\"$(libqualexpr_la_TAG)\" \
                          -Wall # -Werror

libqualexpr_la_HEADERS = \
	$(top_srcdir)/include/quality-expressions/QualExprSemantic.h \
	$(top_srcdir)/include/quality-expressions/QualExprSemanticLocal.h \
	$(top_srcdir)/include/quality-expressions/QualExprSemanticNamespace.h \
	$(top_srcdir)/include/quality-expressions/QualExprSemanticPAPI.h \
	$(top_srcdir)/include/quality-expressions/QualityExpression.h \
	$(top_srcdir)/include/quality-expressions/QualityExpressions.h \
	$(top_srcdir)/include/quality-expressions/QualityExpressionsDB.h \
	$(top_srcdir)/include/quality-expressions/QualityExpressionsDesk.h \
	$(top_srcdir)/include/quality-expressions/QualityExpressionsEntry.h \
	$(top_srcdir)/include/quality-expressions/QualityExpressionsProfiler.h \
	$(top_srcdir)/include/quality-expressions/QualityExpressionsProfilerLocal.h \
	$(top_srcdir)/include/quality-expressions/QualityExpressionsProfilerPAPI.h \
	$(top_srcdir)/include/quality-expressions/QualityExpressionsProfilerSystem.h

libqualexpr_ladir = $(includedir)/quality-expressions/

libqualexpr_la_SOURCES = src/qualexpr-evaluator/QualExprEvaluatorParser.yy \
                         src/qualexpr-evaluator/QualExprEvaluatorLexer.ll \
                         src/qualexpr-evaluator/QualExprEvaluator.cc \
                         src/qualexpr-evaluator/QualExprManager.cc \
                         src/qualexpr-profiler/QualExprProfiler.cc \
                         src/QualityExpressions.cc \
                         src/QualityExpressionsDB.cc \
                         src/QualityExpressionsDesk.cc \
                         src/QualExprSemantic.cc \
                         src/QualityExpressionsProfilerLocal.cc \
                         src/QualExprSemanticLocal.cc \
                         src/QualityExpressionsProfilerPAPI.cc \
                         src/QualExprSemanticPAPI.cc

libqualexpr_la_LDFLAGS = -static $(PAPI_LIB_IN_QE_INSTRUMENT) -version-info $(libqualexpr_la_VERSION)

CLEANFILES= src/qualexpr-evaluator/QualExprEvaluatorParser.yy

src/qualexpr-evaluator/QualExprEvaluatorLexer.cc: src/qualexpr-evaluator/QualExprEvaluatorLexer.ll
	$(AM_V_LEX)$(am__skiplex) $(SHELL) $(YLWRAP) $< $(LEX_OUTPUT_ROOT).c $@ -- $(LEXCOMPILE)
	@mv QualExprEvaluatorLexer.h QualExprEvaluatorLexer.cc src/qualexpr-evaluator/

quality_expressions-clean:
	rm -f $(libqualexpr_la_OBJECTS) src/qualexpr-evaluator/QualExprEvaluator{Parser,Lexer}.{h,hh,cc,yy}

src/qualexpr-evaluator/QualExprEvaluatorParser.yy: src/qualexpr-evaluator/QualExprEvaluatorParser.yy.m4
	m4 -DBISON_VERSION=$(BISON_VERSION) $< > $@

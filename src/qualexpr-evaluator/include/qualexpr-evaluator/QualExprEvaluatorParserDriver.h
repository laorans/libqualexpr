/**
   @file    QualExprEvaluatorParserDriver.h
   @ingroup QualityExpressionEvaluation
   @brief   Commong Interface for both lexer and parser.
   @author  Laurent Morin
   @verbatim
   Revision:       $Revision$
   Revision date:  $Date$
   Committed by:   $Author$

   This file is part of the Periscope performance measurement tool.
   See http://www.lrr.in.tum.de/periscope for details.

   Copyright (c) 2005-2014, Technische Universitaet Muenchen, Germany
   See the COPYING file in the base directory of the package for details.

   @endverbatim
*/

#ifndef QUALEXPREVALUATORPARSERDRIVER_H
#define QUALEXPREVALUATORPARSERDRIVER_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "qualexpr-evaluator/QualExprEvaluator.h"

#ifdef _WITH_QUALEXPR_STANDALONE
#include "src/qualexpr-evaluator/QualExprEvaluatorParser.hh"
#else
#include "quality_expressions/src/qualexpr-evaluator/QualExprEvaluatorParser.hh"
#endif

#include "qualexpr-evaluator/BisonFlexInterface.h"

namespace quality_expressions_core
{
  /**
     @class QualExprEvaluatorParsingDriver
     @brief Quality expression parsing driver
     @ingroup QualityExpressionEvaluation
  */
  class QualExprEvaluatorParsingDriver : public ParserInterface<QualExprEvaluatorParser>
  {
  public:
    /* Constructor */ QualExprEvaluatorParsingDriver(void);
    /* Destructor */  ~QualExprEvaluatorParsingDriver(void) {}

    typedef QualExprEvaluatorParser			Grammar;
    typedef ParserInterface<Grammar>::Token		Token;
    typedef ParserInterface<Grammar>::TokenType		TokenType;

    int					parse(QualExprEvaluator &context, const std::string &filename, const std::string &buffer);		//!< Interface the parser, return error code.
    semantic_desc_t			buildSemantic(const std::string *snamespace, const std::string *name);
    aggregator_desc_t			buildAggregator(const char op, const std::string *name);
    QualExprComputeNodeOf<long64_t> *	buildSemanticAggregator(semantic_desc_t semantic, aggregator_desc_t aggreg);

    void				setResult(QualExprComputeNode *node)		{ m_resultNode = node; }
    QualExprComputeNode *		result(void)					{ return m_resultNode; }

  private: // Members //
    QualExprEvaluator *			m_currentContext;		//!< Parsing context. Used to update the database containing all semantic aggregators.
    QualExprComputeNode *		m_resultNode;			//!< Root node, result of the parsing.
  };
}

YY_DECL;


#endif

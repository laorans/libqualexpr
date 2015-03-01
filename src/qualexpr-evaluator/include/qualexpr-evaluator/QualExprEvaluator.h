/**
   @file    QualExprEvaluator.h
   @ingroup QualityExpressionEvaluation
   @brief   Evaluation of quality expressions - headers
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

#ifndef QUALEXP_EVALUATOR_H_
#define QUALEXP_EVALUATOR_H_

#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>

/**
 * @defgroup QualityExpressionEvaluation Quality expressions evaluation
 * @ingroup QualityExpressionCore
 */

#include "quality-expressions/QualityExpressions.h"
#include "quality-expressions/QualExprSemantic.h"
#include "quality-expressions/QualExprSemanticNamespace.h"

#include "quality-expressions/QualityExpressionsProfilerSystem.h"

#include "qualexpr-evaluator/QualExprAggregator.h"
#include "qualexpr-evaluator/QualExprAggregatorNamespace.h"

#include "qualexpr-evaluator/QualExprAggregatorEvalTemplates.h"
#include "qualexpr-evaluator/QualExprAggregatorImmediate.h"

#include "qualexpr-evaluator/QualExprAggregatorTime.h"
#include "qualexpr-evaluator/QualExprAggregatorSize.h"
#include "qualexpr-evaluator/QualExprAggregatorBandwidth.h"

namespace quality_expressions_core {
  typedef quality_expressions_ns::QualExprSemantic QualExprSemantic;
  typedef quality_expressions_ns::QualExprSemanticNamespace QualExprSemanticNamespace;
  typedef quality_expressions_ns::QualExprSemanticNamespaceStem QualExprSemanticNamespaceStem;
  typedef quality_expressions_ns::QualExprSemanticNamespaceLeaf QualExprSemanticNamespaceLeaf;
}

#include "qualexpr-evaluator/QualExprSemanticAggregator.h"
#include "qualexpr-evaluator/QualExprSemanticAggregatorDB.h"

#include "qualexpr-evaluator/QualExprComputeNode.h"

namespace quality_expressions_core
{
  /* ---------------------------------------------------------------------------------------------------------------- */
  /* ---------------------------------------------------------------------------------------------------------------- */
  /* ---------------------------------------------------------------------------------------------------------------- */
  /* ---------------------------------------------------------------------------------------------------------------- */

  class QualExprEvaluatorParsingDriver;
  class QualExprEvaluatorFrame;
  class QualExprEvaluator;
  typedef unsigned long Context_t;	//!< Define an evaluation context.

  /**
     @class QualExprEvaluatorFrame
     @brief Global quality expression Evaluation framework.
     @ingroup QualityExpressionEvaluation

     The evaluation frame is used to store global, thread dependent and mostly static objects used for the 
     evaluation of quality expressions.
  */
  class QualExprEvaluatorFrame : private QualExprSemaphore
  {
  public:
    /**
       @class Exception
       @brief Hold an exception with a description.
    */
    class Exception : public QualExprException {
    public:
      /* Constructor */ Exception(const char *message) throw() : QualExprException(message)  {}
      /* Constructor */ Exception(const std::string &message) throw() : QualExprException(message)  {}
      /* Destructor */ ~Exception(void) throw() {}
    };

  public:
    /* Constructor */ QualExprEvaluatorFrame(void);
    /* Destructor */ ~QualExprEvaluatorFrame(void);

  public: // -- Evaluation context API
    QualExprSemanticAggregatorDB &	buildFrameAggregator(void);
    QualExprComputeNode *		buildExpressionEvaluationTree(QualExprEvaluator &context, const QualityExpression &expression) throw(QualExprEvaluatorFrame::Exception);

  public: // -- Access API
    void   	displaySemantics(const std::string &indent, std::stringstream &s) const		{ m_semanticRootNamespace.display(indent, s); }
    void   	displayAggregator(const std::string &indent, std::stringstream &s) const	{ m_aggregatorRootNamespace.display(indent, s); }

  public: // -- Namespace management API
    void	registerSemanticNamespace(const QualExprSemanticNamespace &ns)			//!< Append the given namespace to the root semantic namespace.
    { lock(); m_semanticRootNamespace.registerNewNamespace(ns); unlock(); }

  private:
    QualExprSemanticNamespaceStem	m_semanticRootNamespace;		//!< Container for the root namespace of semantics.
    QualExprAggregatorNamespace		m_aggregatorRootNamespace;		//!< Container for the root namespace of aggregators.
    QualExprEvaluatorParsingDriver &	m_parser;				//!< Quality expression Parser and arithmetic builder.
  };

  /**
     @class QualExprEvaluatorStack
     @brief Global quality expression Evaluation framework.
     @ingroup QualityExpressionEvaluation

     This class is be used to store and manage all evaluation contexts.
  */
  class QualExprEvaluatorStack : private QualExprSemaphore
  {
  public:
    /* Constructor */ QualExprEvaluatorStack(void) : m_contextDB()		{}
    /* Destructor */ ~QualExprEvaluatorStack(void)				{ clearEvaluators(); }

  public: // -- Evaluator DB API
    QualExprEvaluator &			getEvaluator(QualExprEvaluatorFrame &frame, Context_t context);
    void				clearEvaluators(void);

    // -- Operations made on all the DB
    size_t			 	pushMeasure(const QualityExpressionEntry &qualExprEntry);			//!< Parse and build a quality expression.
    void				resetMeasures(void);								//!< Reset to the neutral value all aggregators.
    void			   	consolidate(void) throw();							//!< If possible improve data structures to speed-up event evaluations.
    void				evaluateEvent(const QualExprEvent &eventSem) throw();				//!< Dispatch an event.

  private:
    typedef std::map<Context_t, QualExprEvaluator *>	ContextNodeDB_t;	//!< Storage type choosen for the evaluation contexts.
    ContextNodeDB_t					m_contextDB;		//!< Evaluation context database.
  };

  /**
     @class QualExprEvaluator
     @brief Root evaluation platform. Performs runtime evaluations of events for registered quality expressions.
     @ingroup QualityExpressionEvaluation

     The root evaluation platform is composed of a semantic container - with the root semantic namespace - and
     of an aggregator container - with the root aggregator namespace.
  */
  class QualExprEvaluator
  {
  public:
    /**
       @class Exception
       @brief Hold an exception with a description.
    */
    class Exception : public QualExprException {
    public:
      /* Constructor */ Exception(const char *message) throw() : QualExprException(message)  {}
      /* Constructor */ Exception(const std::string &message) throw() : QualExprException(message)  {}
      /* Destructor */ ~Exception(void) throw() {}
    };

  public:
    /* Constructor */ QualExprEvaluator(QualExprEvaluatorFrame & frame);
    /* Destructor */ ~QualExprEvaluator(void);

  public: // -- Access API
    void   	display(const std::string &indent, std::stringstream &s) const;
    // void   	displaySemantics(const std::string &indent, std::stringstream &s) const		{ m_evaluationFrame.displaySemantics(indent, s); }
    // void   	displayAggregator(const std::string &indent, std::stringstream &s) const	{ m_evaluationFrame.displayAggregator(indent, s); }

    void	resetMeasures(void)		{ m_semanticAggregatorDB.resetMeasures(); }     //!< Reset to the neutral value all aggregators.
    void	clearMeasures(void);

  public: // -- Evaluation API
    size_t 	pushMeasure(const QualityExpressionEntry &qualExprEntry) throw(Exception);	//!< Parse and build a quality expression.
    void   	consolidate(void) throw();							//!< If possible improve data structures to speed-up event evaluations.
    void   	evaluateEvent(const QualExprEvent &) throw();					//!< Update quality expressions with event properties.

    long64_t	getLongCounter(QualityExpressionID_T id) throw(Exception);			//!< Return the current value of a quality expression by its ID.
    void        removeExpression(QualityExpressionID_T id) throw(Exception);                    //!< Remove a quality expression by its ID.
    void        resetExpression(QualityExpressionID_T id) throw(Exception);                     //!< Reset the value of a quality expression by its ID.

  public: // -- Parsing API
    QualExprComputeNodeOf<long64_t> *	pushAggregator(const std::string &eventName, const std::string aggregName) throw(Exception);		//!< Add a semantic aggregator. @return the aggregator ID.
    void				pushRootComputeNode(QualityExpressionID_T entryID, QualExprComputeNode *computeNode) throw(Exception);	//!< Add a compute node.

  public: // -- Namespace management API
    // void	registerSemanticNamespace(const QualExprSemanticNamespace &ns)			//!< Append the given namespace to the root semantic namespace.
    // { m_evaluationFrame.registerSemanticNamespace(ns); }

  private:
    typedef std::map<QualityExpressionID_T, QualExprComputeNode *>	ComputeNodeDB_t;        //!< Storage type choosen for the compute nodes.

    QualExprEvaluatorFrame &		m_evaluationFrame;			//!< Evaluation framework.
    QualExprSemanticAggregatorDB &	m_semanticAggregatorDB;                 //!< Database containing all semantic aggregators.
    ComputeNodeDB_t			m_computeNodeDB;                        //!< Database containing all compute nodes by ID.
  };

}

#endif // QUALEXP_EVALUATOR_H_

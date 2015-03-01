/**
   @file    QualExprManager.cc
   @ingroup QualityExpressionCore
   @brief   Quality Expression manager implementation
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <iostream>
#include <sstream>

#include "qualexpr-evaluator/QualExprManager.h"
#include "quality-expressions/QualityExpressionsProfilerLocal.h"

namespace quality_expressions_core
{
/* ---------------------------------------------------------------------------------------------------------------- */
/* ---------------------------------------------------------------------------------------------------------------- */
/* ---------------------------------------------------------------------------------------------------------------- */
/* ---------------------------------------------------------------------------------------------------------------- */

  /** @brief Quality Expression Manager initialization

      Initiliaze a global semaphore and a quality expression evaluator
      for the current thread.
   */
  /* Constructor */ QualExprManager::QualExprManager(void) throw() :
    QualExprSemaphore(), m_state(S_OFF), m_timer(), m_eventBuilder(m_timer)
  {
    evaluateVerbosityLevel();
  }

  /** @brief Quality Expression Manager destruction
      Warning: the clear method must be called before.
   */
  /* Destructor */ QualExprManager::~QualExprManager(void)
  {}

  /** @brief Quality expression evaluator Initializations
   */
  void QualExprManager::init(void)
  {
    lock();
    if (m_state == S_OFF) {
      display(m_evaluatorFrame);
      m_state = S_REGISTERED;
    }
    unlock();
  }

  /** @brief Manager cleanup
   */
  void QualExprManager::clear(void)
  {
    lock();
    if (m_state == S_REGISTERED) {
      m_evaluatorStack.clearEvaluators();
      m_state = S_OFF;
    }
    unlock();
  }

  /** @brief Read the verbosity level from an environment variable.
   */
  void QualExprManager::evaluateVerbosityLevel(void)
  {
    const char *env = getenv(QUALEXPR_VERBOSITY_ENVNAME);
    if (env) {
      m_debugLevel = (enum debug_level_t) atoi(env);
    }
    m_debugLevel = D_FULLEVENTS;
    if (m_debugLevel >= D_ON) {
      std::string header;
      log("Verbosity activated: ", &header);
      std::cerr << m_debugLevel << std::endl;
    }
    else m_debugLevel = D_OFF;
  }

  /** @brief Log a message to the standard error (check verbosity level).

      @param message text to display
      @param o_header indentation or header text
   */
  void QualExprManager::log(const std::string &message, std::string *o_header) const throw()
  {
    if (m_debugLevel >= D_ON) {
      pthread_t threadself = pthread_self();
      char threadId = '0' + (*((unsigned char *)&threadself)) % 9;
      if (o_header) {
        if (o_header->empty()) {
          std::stringstream s;
          s <<  "[quality expressions]["<< getpid() <<"]["<< threadId <<"] ";
          *o_header = s.str();
        }
        std::cerr << *o_header << message;
      }
      else std::cerr << "[quality expressions]["<< getpid() <<"]["<< pthread_self()%0xFF <<"] " << message << std::endl;
    }
  }

  /** @brief Display information and the data structures (check verbosity level).
   */
  void QualExprManager::display(const QualExprEvaluator &evaluator) const throw()
  {
    if (m_debugLevel >= D_ON) {
      std::string header;
      log("Context #", &header);
      std::cerr << &evaluator << std::endl;

      if (m_debugLevel >= D_FULL) {
        std::stringstream s;
        header =  "\n" + header + " *";
        s << header << "Context Measures: "; evaluator.display(header, s);
        std::cerr << s.str() << std::endl;
      }
    }
  }

  /** @brief Display information and the data structures (check verbosity level).
   */
  void QualExprManager::display(const QualExprEvaluatorFrame &evaluatorFrame) const throw()
  {
    if (m_debugLevel >= D_ON) {
      std::string header;
      log("Library version ", &header);
      std::cerr << TAG << std::endl;

      if (m_debugLevel >= D_FULL) {
        log("available measures", &header);
        std::stringstream s;
        header =  "\n" + header + " *";
        s << header << "Semantic Namespaces: "; evaluatorFrame.displaySemantics(header, s);
        s << header << "Aggregator Namespaces: "; evaluatorFrame.displayAggregator(header, s);
        std::cerr << s.str() << std::endl;
      }
    }
  }

  /** @brief Add globaly a semantic namespace.
      @param ns    the semantic namespace
   */
  void QualExprManager::registerSemanticNamespace(semantic_namespace_t ns)
  {
    const QualExprSemanticNamespace &semNs = *((const QualExprSemanticNamespace *)(ns));
    m_evaluatorFrame.registerSemanticNamespace(semNs);
  }

  /** @brief Append a quality expresion evaluation request to all threads.
      @param tid   thread id
      @param entry the quality expresion entry
      @return      the number of expression item registered
   */
  size_t QualExprManager::addGlobalCounter(const QualityExpressionEntry &entry) throw(QualExprManager::Exception)
  {
    size_t rcount = 0;
    if (m_state == S_REGISTERED) {
      m_evaluatorStack.pushMeasure(entry);
    }
    else throw(Exception("Profiler not initialized or already activated"));
    return rcount;
  }

  /** @brief Append a quality expresion evaluation request.
      @param tid   thread id
      @param entry the quality expresion entry
      @return      the number of expression item registered
   */
  size_t QualExprManager::addCounter(Context_t contextId, const QualityExpressionEntry &entry) throw(QualExprManager::Exception)
  {
    // pthread_t tid = pthread_self();
    size_t rcount = 0;
    if (m_state == S_REGISTERED) {
      // QualExprEvaluator * evaluator = getEvaluator(tid);
      QualExprEvaluator & evaluator = m_evaluatorStack.getEvaluator(m_evaluatorFrame, contextId);
      if (m_debugLevel >= D_ON) {
        std::stringstream msg;
        msg << "New quality expression: ";
        entry.display("", msg);
        log(msg.str());
      }
      try {
        rcount = evaluator.pushMeasure(entry);
      }
      catch(QualExprEvaluator::Exception e) {
        display(evaluator);
        throw(Exception(e.what()));
      }
    }
    else throw(Exception("Profiler not initialized or already activated"));
    return rcount;
  }

  /** @brief Get the result of a quality expresion evaluation.
      @param tid thread id
      @param id  the quality expresion ID
      @return    the value
   */
  long long QualExprManager::getLongCounter(Context_t contextId, QualityExpressionID_T id) throw(QualExprManager::Exception)
  {
    if (m_state == S_REGISTERED) {
      QualExprEvaluator & evaluator = m_evaluatorStack.getEvaluator(m_evaluatorFrame, contextId);
      long long result = 0;
      try {
        result = evaluator.getLongCounter(id);
      }
      catch(QualExprEvaluator::Exception e) {
        display(evaluator);
        throw(Exception(e.what()));
      }
      if (m_debugLevel >= D_FULLEVENTS) {
        std::stringstream msg;
        msg << "Fetch quality expression (id:"<< id <<") = " << result;
        log(msg.str());
      }
      return result;
    }
    else throw(Exception("Profiler not initialized or already activated"));
  }

  /** @brief Reset the result of a quality expresion evaluation.
      @param tid thread id
      @param id  the quality expresion ID
   */
  void QualExprManager::resetCounter(Context_t contextId, QualityExpressionID_T id) throw(QualExprManager::Exception)
  {
    if (m_state == S_REGISTERED) {
      QualExprEvaluator & evaluator = m_evaluatorStack.getEvaluator(m_evaluatorFrame, contextId);
      try {
        evaluator.resetExpression(id);
      }
      catch(QualExprEvaluator::Exception e) {
        display(evaluator);
        throw(Exception(e.what()));
      }
      if (m_debugLevel >= D_FULLEVENTS) {
        std::stringstream msg;
        msg << "Reset quality expression (id:"<< id <<")";
        log(msg.str());
      }
    }
    else throw(Exception("Profiler not initialized or already activated"));
  }

  /** @brief Remove a quality expresion evaluation.
      @param tid thread id
      @param id  the quality expresion ID
   */
  void QualExprManager::removeCounter(Context_t contextId, QualityExpressionID_T id) throw(QualExprManager::Exception)
  {
    // pthread_t tid = pthread_self();
    if (m_state == S_REGISTERED) {
      // QualExprEvaluator * evaluator = getEvaluator(tid);
      QualExprEvaluator & evaluator = m_evaluatorStack.getEvaluator(m_evaluatorFrame, contextId);
      try {
        evaluator.removeExpression(id);
      }
      catch(QualExprEvaluator::Exception e) {
        display(evaluator);
        throw(Exception(e.what()));
      }
      if (m_debugLevel >= D_FULLEVENTS) {
        std::stringstream msg;
        msg << "Reset quality expression (id:"<< id <<")";
        log(msg.str());
      }
    }
    else throw(Exception("Profiler not initialized or already activated"));
  }

  /** @brief Remove a quality expresion evaluation request.
      @param tid   thread id
  */
  void QualExprManager::removeCounters(Context_t contextId) throw(QualExprManager::Exception)
  {
    if (m_state == S_REGISTERED) {
      QualExprEvaluator & evaluator = m_evaluatorStack.getEvaluator(m_evaluatorFrame, contextId);

      if (m_debugLevel >= D_FULL) {
        std::string header;
        log("Remove quality expression measures:", &header);

        std::stringstream s;
        header =  "\n" + header + " *";
        s << header ; evaluator.display(header, s);
        std::cerr << s.str() << std::endl;
      }

      evaluator.clearMeasures();
    }
    else throw(Exception("Profiler not initialized or already activated"));
  }

  /** @brief Reset all quality expresions to their neutral value.
      @param tid   thread id
  */
  void QualExprManager::resetCounters(Context_t contextId) throw(QualExprManager::Exception)
  {
    if (m_state == S_REGISTERED) {
      QualExprEvaluator & evaluator = m_evaluatorStack.getEvaluator(m_evaluatorFrame, contextId);

      if (m_debugLevel >= D_FULL) {
        std::string header;
        log("Reset quality expression measures:", &header);

        std::stringstream s;
        header =  "\n" + header + " ";
        s << header ; evaluator.display(header, s);
        std::cerr << s.str() << std::endl;
      }

      evaluator.resetMeasures();
    }
    else throw(Exception("Profiler not initialized or already activated"));
  }

  /** @brief Remove all quality expression evaluators.
      @param tid   thread id
  */
  void QualExprManager::removeAllCounters(void) throw(QualExprManager::Exception)
  {
    if (m_state == S_REGISTERED) {
      m_evaluatorStack.clearEvaluators();
    }
    else throw(Exception("Profiler not initialized or already activated"));
  }

  /** @brief Reset all quality expresions to their neutral value.
      @param tid   thread id
  */
  void QualExprManager::resetAllCounters(void) throw(QualExprManager::Exception)
  {
    // pthread_t tid = pthread_self();
    if (m_state == S_REGISTERED) {
      if (m_debugLevel >= D_FULL) {
        std::string header;
        log("Reset all quality expression measures:", &header);
        std::cerr << header << std::endl;
      }

      m_evaluatorStack.resetMeasures();
    }
    else throw(Exception("Profiler not initialized or already activated"));
  }

  /** @brief Enable measurements.
      @param tid   thread id
  */
  void QualExprManager::enableMeasures(void)
  {
    if (m_state == S_REGISTERED || m_state == S_ON) {
      m_state = S_ON;
    }
    else throw(Exception("Profiler not initialized"));

    m_evaluatorStack.consolidate();
  }

  /** @brief Disable measurements.
      @param tid   thread id
  */
  void QualExprManager::disableMeasures(void)
  {
    if (m_state == S_ON) {
      m_state = S_REGISTERED;
    }
    else throw(Exception("Profiler not activated"));
  }

  /** @brief Handle an event for quality expressions.
   */
  void QualExprManager::event(profiling_event_t * event) throw()
  {
    if (event && m_state == S_ON) {

      lock();
      const QualExprEvent &eventSem = m_eventBuilder.pushEvent_singleThread(event);
      m_evaluatorStack.evaluateEvent(eventSem);

      m_eventBuilder.popEventSequence_singleThread(eventSem);
      unlock();
    }
  }

} // namespace quality_expressions_core

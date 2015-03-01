/**
   @file    QualityExpressionsProfiler.h
   @ingroup QualityExpressionProfilerInterface
   @brief   Quality Expression profiling headers
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

#ifndef QUALITYEXPRESSION_PROFILER_H_
#define QUALITYEXPRESSION_PROFILER_H_

/**
 * @defgroup QualityExpressionProfiler Quality expressions profiling
 * @ingroup QualityExpression
 */

/** @defgroup QualityExpressionProfilerInterface Quality expressions profiling interface
 *  @ingroup QualityExpressionProfiler
 @{
*/

/** @brief Define the event state.
*/
typedef enum event_state_t {
  D_STATE_UNDEFINED=0,			//!< Undefined state by default.
  D_START,				//!< Event Started.
  D_WAIT,				//!< Event finished, lack synchronization.
  D_STOP,				//!< Event Stopped.
  D_COUNTER				//!< Event holding a counter.
} event_state_t;

/** @brief Generic Event descriptor.
*/
typedef struct profiling_event_t {
  event_state_t		m_state;		//!< Event state (start/stop/wait).
  unsigned int		m_semanticId;		//!< Event semantic ID - defined globally.
  unsigned int		m_eid;			//!< Event unique ID - mandatory for intealeaved events.
  long long		m_value;		//!< Value of the counter or event (size, count, ...).
} profiling_event_t;

typedef void (*listen_event_func_t)(profiling_event_t *);	//!< Function type used to evaluate events generated by the profiler.
typedef void *semantic_namespace_t;				//!< Opaque type used to register a profiling namespace.

/**@}*/

#endif // /QUALITYEXPRESSION_PROFILER_H_

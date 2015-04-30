/*
 * opencog/atoms/execution/ExecutionOutputLink.cc
 *
 * Copyright (C) 2009, 2013, 2015 Linas Vepstas
 * All Rights Reserved
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License v3 as
 * published by the Free Software Foundation and including the exceptions
 * at http://opencog.org/wiki/Licenses
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program; if not, write to:
 * Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <stdlib.h>

#include <opencog/atomspace/atom_types.h>
#include <opencog/atomspace/AtomSpace.h>
#include <opencog/atoms/NumberNode.h>
#include <opencog/cython/PythonEval.h>
#include <opencog/guile/SchemeEval.h>

#include "ExecutionOutputLink.h"

using namespace opencog;

ExecutionOutputLink::ExecutionOutputLink(const HandleSeq& oset,
                                         TruthValuePtr tv,
                                         AttentionValuePtr av)
	: FreeLink(EXECUTION_OUTPUT_LINK, oset, tv, av)
{
	if ((2 != oset.size()) or
	   (LIST_LINK != oset[1]->getType()))
	{
		throw RuntimeException(TRACE_INFO,
			"ExecutionOutputLink must have schema and args!");
	}
}

ExecutionOutputLink::ExecutionOutputLink(const Handle& schema,
                                         const Handle& args,
                                         TruthValuePtr tv,
                                         AttentionValuePtr av)
	: FreeLink(EXECUTION_OUTPUT_LINK, schema, args, tv, av)
{
	if (LIST_LINK != args->getType())
	{
		throw RuntimeException(TRACE_INFO,
			"ExecutionOutputLink must have schema and args!");
	}
}

ExecutionOutputLink::ExecutionOutputLink(Link& l)
	: FreeLink(l)
{
	Type tscope = l.getType();
	if (EXECUTION_OUTPUT_LINK != tscope)
	{
		throw RuntimeException(TRACE_INFO,
			"Expection an ExecutionOutputLink!");
	}
}

/// do_execute -- Find all occurances of ExecutionOutputLink or
/// PlusLink or TimesLink, or etc... and execute them. Then create
/// a new atom, where all occurances of ExecutionOutputLink have been
/// replaced by the returned value -- by the atom that was returned
/// by the execution.  The execution is recursive: such links can be
/// nested arbitrarily.
///
/// Each ExecutionOutputLink should have the form:
///
///     ExecutionOutputLink
///         GroundedSchemaNode "lang: func_name"
///         ListLink
///             SomeAtom
///             OtherAtom
///
/// The "lang:" should be either "scm:" for scheme, or "py:" for python.
/// This method will then invoke "func_name" on the provided ListLink
/// of arguments to the function.
///
Handle ExecutionOutputLink::do_execute(AtomSpace* as, const Handle& h)
{
	LinkPtr lll(LinkCast(h));
	if (NULL == lll) return h;

	// If its an execution link, execute it.
	Type t = lll->getType();
	if ((EXECUTION_OUTPUT_LINK == t)
	   or (PLUS_LINK == t)
	   or (TIMES_LINK == t))
	{
		return do_execute(as, t, lll->getOutgoingSet());
	}

	// Search for additional execution links, and execute them too.
	// We will know that happend if the returned handle differs from
	// the input handle.
	std::vector<Handle> new_oset;
	bool changed = false;
	for (Handle ho : lll->getOutgoingSet())
	{
		Handle nh(do_execute(as, ho));
		new_oset.push_back(nh);
		if (nh != ho) changed = true;
	}
	if (not changed) return h;
	return as->addLink(t, new_oset);
}

// handle->double conversion
static inline double get_double(AtomSpace *as, const Handle& ha)
{
	Handle h(ha);

	// Recurse, if needed: handle may be another numeric operator.
	if (NUMBER_NODE != h->getType())
		h = ExecutionOutputLink::do_execute(as, h);

	if (NUMBER_NODE != h->getType())
		throw RuntimeException(TRACE_INFO,
		     "Not a number!");

	NumberNodePtr nnn(NumberNodeCast(h));
	OC_ASSERT (nnn != NULL,
		"This should never happen; there's a bug in the code!");

	return nnn->getValue();
}

/// do_execute -- execute the GroundedSchemaNode of the ExecutionOutputLink
///
/// If the type t is an EXECUTION_OUTPUTLINK, then:
/// Expects the sequence to be exactly two atoms long.
/// Expects the first handle of the sequence to be a GroundedSchemaNode
/// Expects the second handle of the sequence to be a ListLink
/// Executes the GroundedSchemaNode, supplying the second handle as argument
///
/// If the type t is either PLUS_LINK or TIMES_LINK, then:
/// Exepects the atoms to be either NumberNodes, or executable links
/// that end up being valued as NumberNodes.
///
Handle ExecutionOutputLink::do_execute(AtomSpace* as, Type t,
                                       const HandleSeq& sna)
{
	if (EXECUTION_OUTPUT_LINK == t)
	{
		if (2 != sna.size())
		{
			throw RuntimeException(TRACE_INFO,
			     "Incorrect arity for an ExecutionOutputLink!");
		}
		return do_execute(as, sna[0], sna[1]);
	}
	else if (TIMES_LINK == t)
	{
		double prod = 1.0;
		for (Handle h: sna)
		{
			prod *= get_double(as, h);
		}
		return as->addAtom(createNumberNode(prod));
	}
	else if (PLUS_LINK == t)
	{
		double sum = 0.0;
		for (Handle h: sna)
		{
			sum += get_double(as, h);
		}
		return as->addAtom(createNumberNode(sum));
	}

	throw RuntimeException(TRACE_INFO,
	      "Expecting to get an ExecutionOutputLink!");
}

/// do_execute -- execute the GroundedSchemaNode of the ExecutionOutputLink
///
/// Expects "gsn" to be a GroundedSchemaNode
/// Expects "args" to be a ListLink
/// Executes the GroundedSchemaNode, supplying the args as argument
///
Handle ExecutionOutputLink::do_execute(AtomSpace* as,
                         const Handle& gsn, const Handle& cargs)
{
	if (GROUNDED_SCHEMA_NODE != gsn->getType())
	{
		throw RuntimeException(TRACE_INFO, "Expecting GroundedSchemaNode!");
	}

	if (LIST_LINK != cargs->getType())
	{
		throw RuntimeException(TRACE_INFO,
		     "Expecting arguments to ExecutionOutputLink!");
	}

	// Search for additional execution links, and execute them too.
	// We will know that happend if the returned handle differs from
	// the input handle.
	LinkPtr largs(LinkCast(cargs));
	Handle args(cargs);
	if (largs)
	{
		std::vector<Handle> new_oset;
		bool changed = false;
		for (Handle ho : largs->getOutgoingSet())
		{
			Handle nh(do_execute(as, ho));
			new_oset.push_back(nh);
			if (nh != ho) changed = true;
		}
		if (changed)
			args = as->addLink(LIST_LINK, new_oset);
	}

	// Get the schema name.
	const std::string& schema = NodeCast(gsn)->getName();
	// printf ("Grounded schema name: %s\n", schema.c_str());

	// At this point, we only run scheme and python schemas.
	if (0 == schema.compare(0,4,"scm:", 4))
	{
#ifdef HAVE_GUILE
		// Be friendly, and strip leading white-space, if any.
		size_t pos = 4;
		while (' ' == schema[pos]) pos++;

		SchemeEval* applier = SchemeEval::get_evaluator(as);
		Handle h(applier->apply(schema.substr(pos), args));

		// Exceptions were already caught, before leaving guile mode,
		// so we can't rethrow.  Just throw a new exception.
		if (applier->eval_error())
			throw RuntimeException(TRACE_INFO,
			    "Failed evaluation; see logfile for stack trace.");
		return h;
#else
		throw RuntimeException(TRACE_INFO,
		    "Cannot evaluate scheme GroundedSchemaNode!");
#endif /* HAVE_GUILE */
	}

	if (0 == schema.compare(0, 3,"py:", 3))
	{
#ifdef HAVE_CYTHON
		// Be friendly, and strip leading white-space, if any.
		size_t pos = 3;
		while (' ' == schema[pos]) pos++;

		// Get a reference to the python evaluator. NOTE: We are
		// passing in a reference to our atom space to invoke
		// the safety checking to make sure the singleton instance
		// is using the same atom space.
		PythonEval &applier = PythonEval::instance(as);

		Handle h = applier.apply(schema.substr(pos), args);

		// Return the handle
		return h;
#else
		throw RuntimeException(TRACE_INFO,
		    "Cannot evaluate python GroundedSchemaNode!");
#endif /* HAVE_CYTHON */
	}

	// Unkown proceedure type.
	throw RuntimeException(TRACE_INFO,
	    "Cannot evaluate unknown GroundedSchemaNode!");
}

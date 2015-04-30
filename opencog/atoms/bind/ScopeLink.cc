/*
 * ScopeLink.cc
 *
 * Copyright (C) 2009, 2014, 2015 Linas Vepstas
 *
 * Author: Linas Vepstas <linasvepstas@gmail.com>  January 2009
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License v3 as
 * published by the Free Software Foundation and including the
 * exceptions
 * at http://opencog.org/wiki/Licenses
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public
 * License
 * along with this program; if not, write to:
 * Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <opencog/atomspace/ClassServer.h>
#include <opencog/atoms/TypeNode.h>

#include "ScopeLink.h"

using namespace opencog;

void ScopeLink::init(const HandleSeq& oset)
{
	// Must have variable decls and body
	if (2 != oset.size())
		throw InvalidParamException(TRACE_INFO,
			"Expecting variabe decls and body, got size %d", oset.size());

	VariableList::validate_vardecl(oset[0]);
	_body = oset[1];     // Body
}

ScopeLink::ScopeLink(const HandleSeq& oset,
                       TruthValuePtr tv, AttentionValuePtr av)
	: VariableList(SCOPE_LINK, oset, tv, av)
{
	init(oset);
}

ScopeLink::ScopeLink(const Handle& vars, const Handle& body,
                       TruthValuePtr tv, AttentionValuePtr av)
	: VariableList(SCOPE_LINK, HandleSeq({vars, body}), tv, av)
{
	init(getOutgoingSet());
}

ScopeLink::ScopeLink(Type t, const HandleSeq& oset,
                       TruthValuePtr tv, AttentionValuePtr av)
	: VariableList(t, oset, tv, av)
{
	// Derived classes have a different initialization sequence
	if (SCOPE_LINK != t) return;
	init(oset);
}

ScopeLink::ScopeLink(Link &l)
	: VariableList(l)
{
	// Type must be as expected
	Type tscope = l.getType();
	if (not classserver().isA(tscope, SCOPE_LINK))
	{
		const std::string& tname = classserver().getTypeName(tscope);
		throw InvalidParamException(TRACE_INFO,
			"Expecting a ScopeLink, got %s", tname.c_str());
	}

	// Dervided types have a different initialization sequence
	if (SCOPE_LINK != tscope) return;
	init(l.getOutgoingSet());
}

/* ===================== END OF FILE ===================== */

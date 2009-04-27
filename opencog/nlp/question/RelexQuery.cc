/**
 * WordRelQuery.cc
 *
 * Implement pattern matching for RelEx queries. 
 * A "RelEx query" is a sentence such as "What did Bob eat?"
 * RelEx generates a dependency graph for tthis sentence,  replacing 
 * "What" by "$qVar". Pattern matching is used to find an identical
 * dependency graph, for which $qVar would have a grounding; e.g.
 * "Bob ate cake", so that $qVar is grounded as "cake", thus "solving"
 * the query.
 *
 * The result of matching dependency graphs means that queries will be
 * interpreted very literally; the structure of a query sentence must 
 * closely resemble the structure of a sentence in the corpus; otherwise,
 * no matching response will be found.  Some generality can be obtained
 * by converting dependency graphs into semantic triples; the code below
 * show work for that case as well.
 *
 * Copyright (c) 2008 Linas Vepstas <linas@linas.org>
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

#include "RelexQuery.h"

#include <stdio.h>

#include <opencog/atomspace/ForeachChaseLink.h>
#include <opencog/atomspace/Node.h>

using namespace opencog;

WordRelQuery::WordRelQuery(void)
{
	atom_space = NULL;
	pme = NULL;
}

WordRelQuery::~WordRelQuery()
{
	if (pme) delete pme;
	pme = NULL;
}

/* ======================================================== */
/* Routines used to determine if an assertion is a query.
 * XXX This algo is flawed, fragile, but simple.
 * XXX It almost surely would be much better to implement this in 
 * scheme instead of C++.
 */

/**
 * Return true, if atom is of type Node, and if the node
 * name is "match_name" (currently hard-coded as _$qVar)
 */
bool WordRelQuery::is_qVar(Handle word_prop)
{
	Atom *atom = TLB::getAtom(word_prop);
	if (DEFINED_LINGUISTIC_CONCEPT_NODE != atom->getType()) return false;

	Node *n = static_cast<Node *>(atom);
	const std::string& name = n->getName();
	const char * str = name.c_str();
	if (0 == strcmp(str, "who"))
		return true;
	if (0 == strcmp(str, "what"))
		return true;
	if (0 == strcmp(str, "when"))
		return true;
	if (0 == strcmp(str, "where"))
		return true;
	if (0 == strcmp(str, "why"))
		return true;

	return false;
}

/**
 * Search for queries.
 * XXX This implementation is kinda-wrong, its very specific
 * to the structure of the relex-to-opencog conversion, and
 * is fragile, if that structure changes.
 */
bool WordRelQuery::is_word_a_query(Handle word_inst)
{
	return foreach_binary_link(word_inst, INHERITANCE_LINK, &WordRelQuery::is_qVar, this);
}

/* ======================================================== */
/* Routines to help put the query into normal form. */

/**
 * Return true, if the node is, for example, #singluar or #masculine.
 */
bool WordRelQuery::is_ling_cncpt(Atom *atom)
{
	if (DEFINED_LINGUISTIC_CONCEPT_NODE == atom->getType()) return true;
	return false;
}

bool WordRelQuery::is_cncpt(Atom *atom)
{
	if (CONCEPT_NODE == atom->getType()) return true;
	return false;
}

void WordRelQuery::add_to_predicate(Handle ah)
{
	/* scan for duplicates, and don't add them */
	std::vector<Handle>::const_iterator i;
	for (i = normed_predicate.begin();
	     i != normed_predicate.end(); i++)
	{
		Handle h = *i;
		if (h == ah) return;
	}
	normed_predicate.push_back(ah);
}

void WordRelQuery::add_to_vars(Handle ah)
{
	/* scan for duplicates, and don't add them */
	std::vector<Handle>::const_iterator i;
	for (i = bound_vars.begin();
	     i != bound_vars.end(); i++)
	{
		Handle h = *i;
		if (h == ah) return;
	}
	bound_vars.push_back(ah);
}

/**
 * Look to see if word instance is a bound variable,
 * if it is, then add it to the variables list.
 */
bool WordRelQuery::find_vars(Handle word_instance)
{
	foreach_outgoing_handle(word_instance, &WordRelQuery::find_vars, this);

	bool qvar = is_word_a_query(word_instance);
	if (!qvar) return false;

	add_to_vars(word_instance);
	return false;
}

/* ======================================================== */
/* runtime matching routines */

/**
 * Do two word instances have the same word lemma (word root form)?
 * Return true if they are are NOT (that is, if they
 * are mismatched). This stops iteration in the standard
 * iterator.
 *
 * Current structure is:
 * (LemmaLink (stv 1.0 1.0)
 *    (WordInstanceNode "threw@e5649eb8-eac5-48ae-adab-41e351e29e4e")
 *    (WordNode "throw")
 * )
 * (ReferenceLink (stv 1.0 1.0)
 *    (WordInstanceNode "threw@e5649eb8-eac5-48ae-adab-41e351e29e4e")
 *    (WordNode "threw")
 * )
 */
bool WordRelQuery::word_instance_match(Atom *aa, Atom *ab)
{
	// printf ("concept comp "); prt(aa);
	// printf ("          to "); prt(ab);

	// If they're the same atom, then clearly they match.
	if (aa == ab) return false;

	// Look for incoming links that are LemmaLinks.
	// The word lemma should be at the far end.
	Atom *ca = fl.follow_binary_link(aa, LEMMA_LINK);
	Atom *cb = fl.follow_binary_link(ab, LEMMA_LINK);

	// printf ("gen comp %d ", ca==cb); prt(ca);
	// printf ("        to "); prt(cb);

	if (ca == cb) return false;
	return true;
}

#ifdef DEAD_CODE_BUT_DONT_DELETE_JUST_YET
/**
 * Return the word string associated with a word instance.
 * xxxxxxxxx this routine is never called!
 *
 * XXX
 * The actual determination of whether some concept is
 * represented by some word is fragily dependent on the
 * actual nature of concept representation in the
 * relex-to-opencog mapping. Until this is placed into
 * concrete, its inherently fragile.  This is subject
 * to change, if the relex-to-opencog mapping changes.
 *
 * Current mapping is:
 *   ReferenceLink
 *      WordInstanceNode "bark@e798a7dc"
 *      WordNode "bark"
 *
 */
const char * WordRelQuery::get_word_instance(Atom *atom)
{
	// We want the word-node associated with this word instance.
	Atom *wrd = fl.follow_binary_link(atom, REFERENCE_LINK);
	if (!wrd) return NULL;

	if (WORD_NODE != wrd->getType()) return NULL;

	Node *n = static_cast<Node *>(wrd);
	const std::string& name = n->getName();

	return name.c_str();
}
#endif /* DEAD_CODE_BUT_DONT_DELETE_JUST_YET */

/**
 * Are two nodes "equivalent", as far as the opencog representation
 * of RelEx expressions are concerned?
 *
 * Return true to signify a mismatch,
 * Return false to signify equivalence.
 */
bool WordRelQuery::node_match(Node *npat, Node *nsoln)
{
	// If we are here, then we are comparing nodes.
	// The result of comparing nodes depends on the
	// node types.
	Type pattype = npat->getType();
	Type soltype = nsoln->getType();
	if (pattype != soltype) return true;

	// DefinedLinguisticRelation nodes must match exactly;
	// so if we are here, there's already a mismatch.
	if (DEFINED_LINGUISTIC_RELATIONSHIP_NODE == soltype) return true;

	// Word instances match only if they have the same word lemma.
	if (WORD_INSTANCE_NODE == soltype)
	{
		bool mismatch = word_instance_match(npat, nsoln);
		// printf("tree_comp word instance mismatch=%d\n", mismatch);
		return mismatch;
	}

	if (DEFINED_LINGUISTIC_CONCEPT_NODE == soltype)
	{
		/* We force agreement for gender, etc.
		 * be have more relaxed agreement for tense...
		 * i.e. match #past to #past_infinitive, etc.
		 */
		const char * sa = npat->getName().c_str();
		const char * sb = nsoln->getName().c_str();
printf("duude compare %s to %s\n", sa, sb);
		const char * ua = strchr(sa, '_');
		if (ua)
		{
			size_t len = ua-sa;
			char * s = (char *) alloca(len+1);
			strncpy(s,sa,len);
			s[len] = 0x0;
			sa = s;
		}
		const char * ub = strchr(sb, '_');
		if (ub)
		{
			size_t len = ub-sb;
			char * s = (char *) alloca(len+1);
			strncpy(s,sb,len);
			s[len] = 0x0;
			sb = s;
		}
		if (!strcmp(sa, sb)) return false;
		return true;
	}

	fprintf(stderr, "Error: unexpected node type %d %s\n", soltype,
	        classserver().getTypeName(soltype).c_str());

	std::string sa = npat->toString();
	std::string sb = nsoln->toString();
	fprintf (stderr, "unexpected comp %s\n"
	                 "             to %s\n", sa.c_str(), sb.c_str());

	return true;
}

/* ======================================================== */

bool WordRelQuery::solution(std::map<Handle, Handle> &pred_grounding,
                          std::map<Handle, Handle> &var_grounding)
{
	// Reject any solution where a variable is solved
	// by another variable (e.g. if there are multiple
	// questions in the corpus, and we just happened to
	// find one of them.)
	std::map<Handle, Handle>::const_iterator j;
	for (j=var_grounding.begin(); j != var_grounding.end(); j++)
	{
		std::pair<Handle, Handle> pv = *j;
		Handle soln = pv.second;

		// Solution is a word instance; is it alos a query variable?
		bool qvar = is_word_a_query(soln);
		if (qvar) return false;
	}

	printf ("duude Found solution:\n");
	PatternMatchEngine::print_solution(pred_grounding, var_grounding);

	// And now for a cheesy hack to report the solution
	Handle hq = atom_space->addNode(ANCHOR_NODE, "# QUERY SOLUTION");
	Handle hv = bound_vars[0];
	Handle ha = var_grounding[hv];

	Atom *a = TLB::getAtom(ha);
	Atom *wrd = fl.follow_binary_link(a, REFERENCE_LINK);
	Node *n = dynamic_cast<Node *>(wrd);
	if (!n) return false;
	printf("duude answer=%s\n", n->getName().c_str());

	Handle hw = TLB::getHandle(wrd);
	atom_space->addLink(LIST_LINK, hq, hw);
	
	return false;
}

/* ===================== END OF FILE ===================== */

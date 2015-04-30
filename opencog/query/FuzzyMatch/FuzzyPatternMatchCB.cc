/*
 * FuzzyPatternMatchCB.cc
 *
 * Copyright (C) 2015 OpenCog Foundation
 *
 * Author: Leung Man Hin <https://github.com/leungmanhin>
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

#include "FuzzyPatternMatchCB.h"

using namespace opencog;

//#define DEBUG

FuzzyPatternMatchCB::FuzzyPatternMatchCB(AtomSpace* as)
	: DefaultPatternMatchCB(as)
{
}

/**
 * Implement the initiate_search calllback.
 *
 * For a fuzzy match, it starts with a full atomspace search to find candidates.
 *
 * @param pme        The pointer of the PatternMatchEngine
 * @param vars       A set of nodes that are considered as variables
 * @param clauses    The clauses for the query
 * @param negations  The negative clauses
 */
bool FuzzyPatternMatchCB::initiate_search(PatternMatchEngine* pme,
                                          const Variables& vars,
                                          const Pattern& pat)
{
    const HandleSeq& clauses = pat.mandatory;

    _root = clauses[0];
    _starter_term = _root;
    // XXX FIXME  I'm pretty sure that this search is not going to be
    // complete, probably missing lots of similar patterns, simply
    // because it starts out with just the type of the very first
    // clause, and thus missing things that are fuzzily close to it.
    //
    // I know that this is the case, because when I substitute the
    // below with link_type_search(), which normally would be
    // equivalent but faster, the FuzzyUTest failed. Tis means that
    // if the "equivalent" search is missing answers, then the search
    // below is missing them too.
    //
    // So, instead of using the type of clause[0], you probably want
    // look at several types ???  There is a high risk that the
    // resulting search will be very inefficeint: this inefficiency
    // is exactly what link_type_search() was trying to avoid...
    Type ptype = _root->getType();
    HandleSeq handle_set;
    _as->getHandlesByType(handle_set, ptype);
    for (const Handle& h : handle_set)
    {
        bool found = pme->explore_neighborhood(_root, _starter_term, h);
        if (found) return found;
    }
    return false;
}

/**
 * Override the link_match callback.
 *
 * This is for finding similar links/hypergraphs in the atomspace. The possible
 * grounding link (gLink) will be compared with the pattern link (pLink) and see
 * if it should be accepted, based on the similarity between them.
 *
 * @param pLink  The link from the query
 * @param gLink  A possible grounding link found by the Pattern Matcher
 * @return       Always return false to search for more solutions
 */
bool FuzzyPatternMatchCB::link_match(const LinkPtr& pLink, const LinkPtr& gLink)
{
    // If two links are identical, skip it
    if (pLink == gLink) return false;

    // Check if the types of the links are the same before going further into
    // the similarity estimation.
    // This is mainly for reducing the amount of hypergraphs being processed
    // as the content of two links with different types are likely to be quite
    // different.
    if (pLink->getType() != gLink->getType()) return false;

    check_if_accept(pLink->getHandle(), gLink->getHandle());

    return false;
}

/**
 * Override the node_match callback.
 *
 * This is for finding similar nodes in the atomspace. The possible grounding
 * node (gNode) will be compared with the pattern node (pNode) and see if it
 * should be accepted, based on the similarity between them.
 *
 * @param pNode  A handle of the node form the query
 * @param gNode  A handle of a possible grounding node
 * @return       Always return false to search for more solutions
 */
bool FuzzyPatternMatchCB::node_match(const Handle& pNode, const Handle& gNode)
{
    // If two handles are identical, skip it
    if (pNode == gNode) return false;

    check_if_accept(pNode, gNode);

    return false;
}

/**
 * Implement the grounding callback.
 *
 * Always return false to search for more solutions.
 *
 * @param var_soln   The variable & links mapping
 * @param term_soln  The clause mapping
 * @return           Always return false to search for more solutions
 */
bool FuzzyPatternMatchCB::grounding(const std::map<Handle, Handle>& var_soln,
                                    const std::map<Handle, Handle>& term_soln)
{
    return false;
}

/**
 * Check how similar the two hypergraphs are by computing the edit distance. If
 * the edit distance is smaller than or equals to the previous minimum, then
 * it will be accepted.
 *
 * @param ph  A handle of the hypergraph from the query
 * @param gh  A handle of a possible grounding hypergraph
 * @return    True if it is accepted, false otherwise
 */
bool FuzzyPatternMatchCB::check_if_accept(const Handle& ph, const Handle& gh)
{
    // Compute the edit distance
    cand_edit_dist = ged.compute(ph, gh);

    // Skip identical hypergraphs, if any
    if (cand_edit_dist == 0)
    {
#ifdef DEBUG
        printf("Cost = %.3f\nSkipped!\n\n", cand_edit_dist);
#endif
        return false;
    }

    // If the edit distance of the current hypergraph is smaller than the
    // previous minimum, it becomes the new minimum
    else if (cand_edit_dist < min_edit_dist)
    {
#ifdef DEBUG
        printf("Cost = %.3f\nMin. Cost = %.3f\nAccepted!\n\n",
               cand_edit_dist, min_edit_dist);
#endif
        min_edit_dist = cand_edit_dist;
        solns.clear();
        solns.push_back(gh);
        return true;
    }

    // If the edit distance of the current hypergraph is the same as the
    // previous minimum, add it to the solution-list
    else if (cand_edit_dist == min_edit_dist)
    {
#ifdef DEBUG
        printf("Cost = %.3f\nMin. Cost = %.3f\nAccepted!\n\n",
               cand_edit_dist, min_edit_dist);
#endif
        solns.push_back(gh);
        return true;
    }

    // If the edit distance of the current hypergraph is greater than the
    // previous minimum, reject it
    else
    {
#ifdef DEBUG
        printf("Cost = %.3f\nMin. Cost = %.3f\nRejected!\n\n",
               cand_edit_dist, min_edit_dist);
#endif
        return false;
    }
}

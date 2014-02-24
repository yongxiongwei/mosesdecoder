/*
 * Rule.cpp
 *
 *  Created on: 20 Feb 2014
 *      Author: hieu
 */

#include <sstream>
#include "Rule.h"
#include "AlignedSentence.h"
#include "ConsistentPhrase.h"
#include "NonTerm.h"
#include "Parameter.h"

using namespace std;

Rule::Rule(const ConsistentPhrase &consistentPhrase, const AlignedSentence &alignedSentence)
:m_nonTerm(consistentPhrase.GetNonTerm())
,m_alignedSentence(alignedSentence)
,m_isValid(true)
,m_canRecurse(true)
{
	CreateSource();
}

Rule::Rule(const Rule &copy, const ConsistentPhrase &cp)
:m_nonTerm(copy.m_nonTerm)
,m_alignedSentence(copy.m_alignedSentence)
,m_isValid(true)
,m_canRecurse(true)
,m_nonterms(copy.m_nonterms)
{
	m_nonterms.push_back(&cp.GetNonTerm());
	CreateSource();

}

Rule::~Rule() {
	// TODO Auto-generated destructor stub
}

const ConsistentPhrase &Rule::GetConsistentPhrase() const
{ return m_nonTerm.GetConsistentPhrase(); }

void Rule::CreateSource()
{
  const NonTerm *cp = NULL;
  size_t nonTermInd = 0;
  if (nonTermInd < m_nonterms.size()) {
	  cp = m_nonterms[nonTermInd];
  }

  for (int sourcePos = m_nonTerm.GetConsistentPhrase().corners[0];
		  sourcePos <= m_nonTerm.GetConsistentPhrase().corners[1];
		  ++sourcePos) {

	  const RuleSymbol *ruleSymbol;
	  if (cp && cp->GetConsistentPhrase().corners[0] <= sourcePos && sourcePos <= cp->GetConsistentPhrase().corners[1]) {
		  // replace words with non-term
		  ruleSymbol = cp;
		  sourcePos = cp->GetConsistentPhrase().corners[1];
		  if (m_nonterms.size()) {
			  cp = m_nonterms[nonTermInd];
		  }

		  // move to next non-term
		  ++nonTermInd;
		  if (nonTermInd < m_nonterms.size()) {
			  cp = m_nonterms[nonTermInd];
		  }
		  else {
			  cp = NULL;
		  }
	  }
	  else {
		  // terminal
		  ruleSymbol = m_alignedSentence.GetPhrase(Moses::Input)[sourcePos];
	  }

	  m_source.push_back(ruleSymbol);
  }
}

int Rule::GetNextSourcePosForNonTerm() const
{
	if (m_nonterms.empty()) {
		// no non-terms so far. Can start next non-term on left corner
		return m_nonTerm.GetConsistentPhrase().corners[0];
	}
	else {
		// next non-term can start just left of previous
		const ConsistentPhrase &cp = m_nonterms.back()->GetConsistentPhrase();
		int nextPos = cp.corners[1] + 1;
		if (nextPos >= m_alignedSentence.GetPhrase(Moses::Input).size()) {
			return -1;
		}
		else {
			return nextPos;
		}
	}
}

std::string Rule::Debug() const
{
  stringstream out;

  // source
  for (size_t i =  0; i < m_source.size(); ++i) {
	  const RuleSymbol &symbol = *m_source[i];
	  out << symbol.Debug() << " ";
  }

  // target
  out << "||| ";
  for (size_t i =  0; i < m_target.size(); ++i) {
	  const RuleSymbol &symbol = *m_target[i];
	  out << symbol.Debug() << " ";
  }

  // overall range
  out << "||| " << m_nonTerm.Debug();

  return out.str();
}

void Rule::Output(std::ostream &out) const
{
  // source
  for (size_t i =  0; i < m_source.size(); ++i) {
	  const RuleSymbol &symbol = *m_source[i];
	  symbol.Output(out);
	  out << " ";
  }
  m_nonTerm.Output(out, Moses::Input);

  out << " ||| ";

  // target
  for (size_t i =  0; i < m_target.size(); ++i) {
	  const RuleSymbol &symbol = *m_target[i];
	  symbol.Output(out);
	  out << " ";
  }
  m_nonTerm.Output(out, Moses::Output);

}

void Rule::Prevalidate(const Parameter &params)
{
  if (m_source.size() >= params.maxSymbolsSource) {
	  m_canRecurse = false;
	  if (m_source.size() > params.maxSymbolsSource) {
		  m_isValid = false;
		  return;
	  }
  }

  // check number of non-terms
  int numNonTerms = 0;
  for (size_t i = 0; i < m_source.size(); ++i) {
	  const RuleSymbol *arc = m_source[i];
	  if (arc->IsNonTerm()) {
		  ++numNonTerms;
	  }
  }

  if (numNonTerms >= params.maxNonTerm) {
	  m_canRecurse = false;
	  if (numNonTerms > params.maxNonTerm) {
		  m_isValid = false;
		  return;
	  }
  }


}

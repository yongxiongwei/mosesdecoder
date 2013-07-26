#include "InputPath.h"
#include "ScoreComponentCollection.h"
#include "TargetPhraseCollection.h"
#include "StaticData.h"
#include "TypeDef.h"
#include "AlignmentInfo.h"

using namespace std;

namespace Moses
{
InputPath::InputPath(const Phrase &phrase, const WordsRange &range, const InputPath *prevNode
                     ,const ScoreComponentCollection *inputScore)
  :m_prevNode(prevNode)
  ,m_phrase(phrase)
  ,m_range(range)
  ,m_inputScore(inputScore)
{
  FactorType placeholderFactor = StaticData::Instance().GetPlaceholderFactor().first;
  if (placeholderFactor != NOT_FOUND) {
    for (size_t pos = 0; pos < m_phrase.GetSize(); ++pos) {
      if (m_phrase.GetFactor(pos, placeholderFactor)) {
        m_placeholders.push_back(pos);
      }
    }
  }
}

InputPath::~InputPath()
{
  delete m_inputScore;
}

const TargetPhraseCollection *InputPath::GetTargetPhrases(const PhraseDictionary &phraseDictionary) const
{
  std::map<const PhraseDictionary*, std::pair<const TargetPhraseCollection*, const void*> >::const_iterator iter;
  iter = m_targetPhrases.find(&phraseDictionary);
  if (iter == m_targetPhrases.end()) {
    return NULL;
  }
  return iter->second.first;
}

const void *InputPath::GetPtNode(const PhraseDictionary &phraseDictionary) const
{
  std::map<const PhraseDictionary*, std::pair<const TargetPhraseCollection*, const void*> >::const_iterator iter;
  iter = m_targetPhrases.find(&phraseDictionary);
  if (iter == m_targetPhrases.end()) {
    return NULL;
  }
  return iter->second.second;
}

void InputPath::SetTargetPhrases(const PhraseDictionary &phraseDictionary
                                 , const TargetPhraseCollection *targetPhrases
                                 , const void *ptNode)
{
  FactorType placeholderFactor = StaticData::Instance().GetPlaceholderFactor().first;
  if (targetPhrases == NULL || placeholderFactor == NOT_FOUND || m_placeholders.size() == 0) {
    // use all of the target phrase given
    std::pair<const TargetPhraseCollection*, const void*> value(targetPhrases, ptNode);
    m_targetPhrases[&phraseDictionary] = value;
  } else {
    // filter out target phrases with alignments that are not 1-to-1 with placeholder
    m_copiedSet.push_back(TargetPhraseCollection());
    TargetPhraseCollection &newTargetPhrases = m_copiedSet.back();
    std::pair<const TargetPhraseCollection*, const void*> value(&newTargetPhrases, ptNode);
    m_targetPhrases[&phraseDictionary] = value;

    TargetPhraseCollection::const_iterator iter;
    for (iter = targetPhrases->begin(); iter != targetPhrases->end(); ++iter) {
      TargetPhrase *targetPhrase = new TargetPhrase(**iter);
      bool ok = SetPlaceholders(targetPhrase);

      if (ok) {
        newTargetPhrases.Add(targetPhrase);
      } else {
        delete targetPhrase;
      }
    }
  }
}

bool InputPath::SetPlaceholders(TargetPhrase *targetPhrase) const
{
  FactorType sourcePlaceholderFactor = StaticData::Instance().GetPlaceholderFactor().first;
  FactorType targetPlaceholderFactor = StaticData::Instance().GetPlaceholderFactor().second;

  const AlignmentInfo &alignments = targetPhrase->GetAlignTerm();
  for (size_t i = 0; i < m_placeholders.size(); ++i) {
    size_t sourcePos = m_placeholders[i];
    set<size_t> targetPos = alignments.GetAlignmentsForSource(sourcePos);
    if (targetPos.size() == 1) {
      const Word &sourceWord = m_phrase.GetWord(sourcePos);
      Word &targetWord = targetPhrase->GetWord(*targetPos.begin());
      targetWord[targetPlaceholderFactor] = sourceWord[sourcePlaceholderFactor];
    } else {
      return false;
    }
  }
  return true;
}

std::ostream& operator<<(std::ostream& out, const InputPath& obj)
{
  out << &obj << " " << obj.GetWordsRange() << " " << obj.GetPrevNode() << " " << obj.GetPhrase();

  out << "pt: ";
  std::map<const PhraseDictionary*, std::pair<const TargetPhraseCollection*, const void*> >::const_iterator iter;
  for (iter = obj.m_targetPhrases.begin(); iter != obj.m_targetPhrases.end(); ++iter) {
    const PhraseDictionary *pt = iter->first;
    out << pt << " ";
  }

  return out;
}

}
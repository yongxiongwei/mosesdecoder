// $Id$
// vim:tabstop=2
/***********************************************************************
 Moses - factored phrase-based language decoder
 Copyright (C) 2010 Hieu Hoang

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 ***********************************************************************/

#include "CoveredChartSpan.h"

namespace Moses
{

std::ostream& operator<<(std::ostream &out, const CoveredChartSpan &coveredChartSpan)
{
  out << coveredChartSpan.GetWordsRange()
      << "=" << coveredChartSpan.GetSourceWord() << " ";
  if (coveredChartSpan.m_prevCoveredChartSpan)
    out << " " << *coveredChartSpan.m_prevCoveredChartSpan;

  return out;
}


} // namespace
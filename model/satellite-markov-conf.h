/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013 Magister Solutions Ltd
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Frans Laakso <frans.laakso@magister.fi>
 */

#ifndef SAT_MARKOV_CONF_H
#define SAT_MARKOV_CONF_H

#include "ns3/object.h"
#include "ns3/uinteger.h"
#include "ns3/log.h"
#include "ns3/simulator.h"

namespace ns3 {

/**
 * \brief A configuration class for three state Markov model
 *
 */
class SatMarkovConf : public Object
{
public:
  static const uint32_t DEFAULT_ELEVATION_COUNT = 4;
  static const uint32_t DEFAULT_STATE_COUNT = 3;
  static const uint32_t DEFAULT_LOO_PARAMETER_COUNT = 5;

  static TypeId GetTypeId (void);
  SatMarkovConf ();
  virtual ~SatMarkovConf ()
  {
  }
  uint32_t GetProbabilitySetID (double elevation);
  std::vector<std::vector<double> > GetElevationProbabilities (uint32_t setId);
  std::vector<std::vector<double> > GetLooParameters (uint32_t setId);
  uint32_t GetStateCount ();
  Time GetCooldownPeriod ();
  double GetMinimumPositionChange ();
  uint32_t GetNumOfOscillators ();
  double GetDopplerFrequency ();
  uint32_t GetNumOfSets ();

private:
  uint32_t m_elevationCount;
  uint32_t m_stateCount;
  std::vector<std::vector<std::vector<double> > > m_markovProbabilities;
  std::map<double, uint32_t> m_markovElevations;
  std::vector<std::vector<std::vector<double> > > m_looParameters;
  Time m_cooldownPeriodLength;
  double m_minimumPositionChangeInMeters;
  uint32_t m_numOfOscillators;
  double m_dopplerFrequencyHz;
};

} // namespace ns3

#endif /* SAT_MARKOV_CONF_H */
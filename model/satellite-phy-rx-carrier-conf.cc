/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013 Magister Solutions Ltd.
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
 * Author: Jani Puttonen <jani.puttonen@magister.fi>
 */

#include "ns3/log.h"
#include "ns3/enum.h"
#include "ns3/uinteger.h"
#include "satellite-utils.h"
#include "satellite-phy-rx-carrier-conf.h"

NS_LOG_COMPONENT_DEFINE ("SatPhyRxCarrierConf");

namespace ns3 {

SatPhyRxCarrierConf::SatPhyRxCarrierConf()
{
  m_errorModel = EM_CONSTANT;
  m_ifModel = IF_CONSTANT;
}

SatPhyRxCarrierConf::SatPhyRxCarrierConf ( double rxTemperature_dBK, double rxOtherSysNoise_dBW,
                                           ErrorModel errorModel, InterferenceModel ifModel,
                                           RxMode rxMode)
{
  m_errorModel = errorModel;
  m_ifModel = ifModel;
  m_rxTemperature_K = SatUtils::DbToLinear (rxTemperature_dBK);
  m_rxMode = rxMode;
  m_rxOtherSysNoise_W = SatUtils::DbWToW<double> (rxOtherSysNoise_dBW);

}

TypeId
SatPhyRxCarrierConf::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::SatPhyRxCarrierConf")
    .SetParent<Object> ()
    .AddAttribute("ChannelType", "The type of the channel where this object is asscociated.",
                   EnumValue (SatChannel::UNKNOWN_CH),
                   MakeEnumAccessor (&SatPhyRxCarrierConf::m_channelType),
                   MakeEnumChecker (SatChannel::FORWARD_FEEDER_CH, "Forward feeder channel",
                                    SatChannel::FORWARD_USER_CH, "Forward user channel",
                                    SatChannel::RETURN_FEEDER_CH, "Return feeder channel",
                                    SatChannel::RETURN_USER_CH, "Return user channel"))
    .AddAttribute ("CarrierBandwidhtConverter", "Callback to convert carrier id to bandwidth.",
                    CallbackValue (),
                    MakeCallbackAccessor (&SatPhyRxCarrierConf::m_carrierBandwidthConverter),
                    MakeCallbackChecker ())
    .AddAttribute ("CarrierCount", "The count of carriers to create in installation",
                    UintegerValue (1),
                    MakeUintegerAccessor (&SatPhyRxCarrierConf::m_carrierCount),
                    MakeUintegerChecker<uint32_t> (1))
    .AddConstructor<SatPhyRxCarrierConf> ()
  ;
  return tid;
}


void
SatPhyRxCarrierConf::SetLinkResults (Ptr<SatLinkResults> linkResults)
{
  m_linkResults = linkResults;
}

uint32_t SatPhyRxCarrierConf::GetCarrierCount () const
{
  return m_carrierCount;
}


SatPhyRxCarrierConf::ErrorModel SatPhyRxCarrierConf::GetErrorModel () const
{
  return m_errorModel;
}

SatPhyRxCarrierConf::InterferenceModel SatPhyRxCarrierConf::GetInterferenceModel () const
{
  return m_ifModel;
}

Ptr<SatLinkResults> SatPhyRxCarrierConf::GetLinkResults () const
{
  return m_linkResults;
}

double SatPhyRxCarrierConf::GetCarrierBandwidth_Hz ( uint32_t carrierId ) const
{
  return m_carrierBandwidthConverter( m_channelType, carrierId );
}

double SatPhyRxCarrierConf::GetRxTemperature_K () const
{
  return m_rxTemperature_K;
}

double SatPhyRxCarrierConf::GetRxOtherSystemNoise_W () const
{
  return m_rxOtherSysNoise_W;
}

SatPhyRxCarrierConf::RxMode SatPhyRxCarrierConf::GetRxMode () const
{
  return m_rxMode;
}

} // namespace ns3
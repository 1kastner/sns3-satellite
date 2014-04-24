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

#ifndef SATELLITE_PHY_RX_CARRIER_CONF_H_
#define SATELLITE_PHY_RX_CARRIER_CONF_H_

#include "ns3/object.h"

#include "satellite-channel.h"
#include "satellite-link-results.h"
#include "satellite-channel-estimation-error-container.h"

namespace ns3 {

/**
 * \brief Information of beam users liken UTs and their users.
 * Used e.g. when creating beams.
 *
 */
class SatPhyRxCarrierConf : public Object
{
public:

  /**
   * \brief Callback for SINR calculation
   * \param Calculated C/NI
   * \return Calculated Final SINR in linear
   */
  typedef Callback<double, double> SinrCalculatorCallback;

  /**
   * \brief Callback for carrier bandwidth
   * \param channelType     The type of the channel
   * \param carrierId       The id of the carrier
   * \param bandwidthType   The type of the bandwidth
   *
   * \return The bandwidth of the carrier.
   */
  typedef Callback<double, SatEnums::ChannelType_t, uint32_t, SatEnums::CarrierBandwidthType_t > CarrierBandwidthConverter;

  /**
   *  \brief RX mode enum
   *
   *  TRANSPARENT   Only beam Id is checked for receiving
   *  NORMAL        Beam Id and valid address (own or broadcast) is checked for receiving
  */
  enum RxMode
  {
    TRANSPARENT, NORMAL
  };

  /**
   * \brief Error model enum
   */
  enum ErrorModel
  {
    EM_NONE, EM_CONSTANT, EM_AVI
  };

  /**
   * \brief Interference model enum
   */
  enum InterferenceModel
  {
    IF_PER_PACKET, IF_TRACE, IF_CONSTANT
  };

  /**
   * \brief Random access collision model enum
   */
  enum RandomAccessCollisionModel
  {
    RA_COLLISION_NOT_DEFINED, RA_COLLISION_ALWAYS_DROP_ALL_COLLIDING_PACKETS, RA_COLLISION_CHECK_AGAINST_SINR
  };

  /**
   * \brief Struct for storing the information for SatPhyRxCarrierConf.
   * \param rxTemperatureK RX noise temperature in Kelvins
   * \param m_aciIfWrtNoiseFactor Adjacent channel interference wrt noise factor
   * \param errorModel Used error model
   * \param daIfModel Used interference model for dedicated access
   * \param raIfModel Used interference model for random access
   * \param rxMode RX mode used in carrier
   * \param chType RX channel type
   * \param converter Bandwidth converter
   * \param carrierCount carrier count
   * \param cec Channel estimation error container
   */
  typedef struct
  {
    double                                  m_rxTemperatureK;
    double                                  m_extNoiseDensityWhz;
    double                                  m_aciIfWrtNoiseFactor;
    ErrorModel                              m_errorModel;
    InterferenceModel                       m_daIfModel;
    InterferenceModel                       m_raIfModel;
    RxMode                                  m_rxMode;
    SatEnums::ChannelType_t                 m_chType;
    CarrierBandwidthConverter               m_converter;
    uint32_t                                m_carrierCount;
    Ptr<SatChannelEstimationErrorContainer> m_cec;
    RandomAccessCollisionModel              m_raCollisionModel;
    bool                                    m_isRandomAccessEnabled;
  } RxCarrierCreateParams_s;

  /**
   * Default constructor for SatPhyRxCarrierConf
   * \param params Rx carrier parameters
   */
  SatPhyRxCarrierConf (RxCarrierCreateParams_s params);

  /**
   * Constructor for SatPhyRxCarrierConf
   */
  SatPhyRxCarrierConf ( );

  /**
   * Destructor for SatPhyRxCarrierConf
   */
  virtual ~SatPhyRxCarrierConf () {}

  static TypeId GetTypeId (void);

  /**
   * \brief Dispose function
   */
  virtual void DoDispose ();

  /**
   * \brief Set link results for the carrier configuration, either DVB-RCS2 or DVB-S2
   */
  void SetLinkResults (Ptr<SatLinkResults> linkResults);

  /**
   * \brief Get the number of configured carriers
   * \return the number of configured carriers
   */
  uint32_t GetCarrierCount () const;

  /**
   * \brief Get configured error model
   * \return configured error model
   */
  ErrorModel GetErrorModel () const;

  /**
   * \brief Get configured interference model
   * \param isRandomAccessCarrier Do we want RA or DA IF model
   * \return configured interference model
   */
  InterferenceModel GetInterferenceModel (bool isRandomAccessCarrier) const;

  /**
   * \brief Get configured link results
   * \return configured link results
   */
  Ptr<SatLinkResults> GetLinkResults () const;

  /**
   * \brief Get configured bandwidth
   * \param carrierId   Id of the carrier
   * \param bandwidthType Type of the bandwidth
   * \return Bandwidth of the requested carrier.
   */
  double GetCarrierBandwidthHz ( uint32_t carrierId, SatEnums::CarrierBandwidthType_t bandwidthType ) const;

  /**
   * \brief Get configured RX noise temperature
   * \return configured RX noise temperature
   */
  double GetRxTemperatureK () const;

  /**
   * \brief Get other system RX noise
   * \return other system RX noise
   */
  double GetExtPowerDensityWhz () const;

  /**
   * \brief Get adjacent channel interference wrt noise (factor)
   * \return adjacent channel interference wrt noise (factor)
   */
  double GetRxAciInterferenceWrtNoiseFactor () const;

  /**
   * \brief Get configured RX mode
   * \return configured RX mode
   */
  RxMode GetRxMode () const;

  /**
   * \brief Function for getting the error rate for constant error model
   * \return error rate for constant error model
   */
  double GetConstantErrorRate () const;

  /**
   * \brief Get channel type
   * \return channel type
   */
  SatEnums::ChannelType_t GetChannelType () const;

  /**
   * \brief Is interference output trace enabled
   * \return true or false
   */
  bool IsIntfOutputTraceEnabled () const;

  /**
   * \brief Get callback function to calculate final SINR
   * \return final SINR
   */
  inline SinrCalculatorCallback  GetSinrCalculatorCb () {return m_sinrCalculate;}

  /**
   * \brief Set callback function to calculate final SINR
   * \param sinrCalculator SINR calculator callback
   */
  inline void SetSinrCalculatorCb (SinrCalculatorCallback sinrCalculator) {m_sinrCalculate = sinrCalculator;}

  /**
   * \brief Get channel estimator container
   * \return channel estimator container
   */
  Ptr<SatChannelEstimationErrorContainer> GetChannelEstimatorErrorContainer () const;

  /**
   * \brief Get random access collision model
   * \return random access collision model
   */
  RandomAccessCollisionModel GetRandomAccessCollisionModel () const;

  /**
   * \brief Get random access average normalized offered load measurement window size
   * \return average normalized offered load measurement window size
   */
  uint32_t GetRandomAccessAverageNormalizedOfferedLoadMeasurementWindowSize () const;

private:

  /*
   * Note, that different carriers may be different bandwidth (symbol rate).
   * This might be one place to have the frame configuration as well, so that
   * we can pass the bandwidth information for each carrier. The bandwidth may be needed
   * for at least noise and C/No calculation.
   */
  InterferenceModel m_daIfModel;
  InterferenceModel m_raIfModel;
  ErrorModel m_errorModel;
  double m_rxTemperatureK;
  double m_rxAciIfWrtNoiseFactor;
  RxMode m_rxMode;
  uint32_t m_carrierCount;
  CarrierBandwidthConverter m_carrierBandwidthConverter;
  SatEnums::ChannelType_t m_channelType;
  Ptr<SatChannelEstimationErrorContainer> m_channelEstimationError;
  SinrCalculatorCallback m_sinrCalculate;
  double m_constantErrorRate;
  Ptr<SatLinkResults> m_linkResults;
  double m_rxExtNoiseDensityWhz;
  bool m_enableIntfOutputTrace;
  uint32_t m_randomAccessAverageNormalizedOfferedLoadMeasurementWindowSize;
  RandomAccessCollisionModel m_raCollisionModel;
};

} // namespace ns3


#endif /* SATELLITE_PHY_RX_CARRIER_CONF_H_ */

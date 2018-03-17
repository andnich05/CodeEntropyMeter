/*
 * RMSMeter: Calculation of RMS
 *
 * Copyright (C) 2014  Andrej Nichelmann
 *                     Klaus Michael Indlekofer
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "RMSMeter.hpp"
#include <cmath>

const double INF = -999.0;

RMSMeter::RMSMeter(RMSMeterListener *listener)
    : m_rmsListener(listener)
    , m_actualValue(-60.0)
    , m_returnTimeValue(0.0)
    , m_referenceValue(0)
    , m_i(0)
    , m_maximumDynamicRange(0.0)
{
}

void RMSMeter::updateMeter(const std::vector<int32_t> & signalValues)
{
    if(!m_rmsListener)
    {
        return;
    }

    emitRmsValue(calculateRootMeanSquare(signalValues));
}

void RMSMeter::updateBitdepth(int bitdepth)
{
    m_referenceValue = static_cast<uint32_t>(std::pow(2.0, bitdepth-1));
    m_maximumDynamicRange = 20.0*std::log10(std::pow(2.0,bitdepth)/2.0);
}

double RMSMeter::calculateRootMeanSquare(const std::vector<int32_t> & signalValues)
{
    long double rms = 0;

    // Square
    for(const auto& signalValue : signalValues)
    {
        rms += std::pow(signalValue, 2);
    }

    // Mean + Root
    rms = std::sqrt(rms / signalValues.size());

    // Convert to dB if rms isn't zero
    if(rms > 0)
    {
        if(rms < 1)
        {
            rms = 1;
        }
        rms = 20.0*std::log10(rms/static_cast<double>(m_referenceValue));
        return rms;
    }
    else
    {
        return INF;
    }
}

void RMSMeter::emitRmsValue(double rms)
{
    // Check if the calculated value is greater than the value which is currently being displayed
    if(rms > m_actualValue)
    {
        m_actualValue = rms;
        // Check if the holder needs to be updated
        if(rms >= -60.0)
        {
            m_rmsListener->receiveRmsHolderValue(m_actualValue);
        }
    }
    // Check if the signal is clipping
    else if(rms >= 0)
    {
        m_actualValue = 0;
    }

    // Update current meter value
    m_rmsListener->receiveRmsMeterValue(m_actualValue);

    // Decrement current value if the calculated value is smaller (return time)
    if(rms < m_actualValue)
    {
        if(m_actualValue > -m_maximumDynamicRange)
        {
            m_actualValue -= m_returnTimeValue;
        }
        else
        {
            m_actualValue = INF;
        }
    }
}

void RMSMeter::setReturnTimeValue(double value)
{
    m_returnTimeValue = value;
}

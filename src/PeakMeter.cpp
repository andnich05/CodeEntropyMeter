/*
 * PeakMeter: Calculation of peak values
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

#include "PeakMeter.hpp"

#include <cmath>
#include <algorithm>

const double INF = -999.0;

PeakMeter::PeakMeter(PeakMeterListener *listener)
    : m_peakMeterListener(listener)
    , m_actualValue(-60.0)
    , m_returnTimeValue(0.0)
    , m_currentValue(0)
    , m_referenceValue(0)
    , m_maxValue(0)
    , m_absoluteValue(0)
    , m_maximumDynamicRange(0.0)
{
}

void PeakMeter::updateMeter(const std::vector<int32_t> & signalValues)
{
    if(!m_peakMeterListener)
    {
        return;
    }

    // Get the maximum value of the samples
    m_currentValue = getMaximum(signalValues);
    emitPeakValue(calculatePeak(m_currentValue, m_referenceValue));
}

void PeakMeter::updateBitdepth(int bitdepth)
{
    m_referenceValue = static_cast<uint32_t>(std::pow(2.0, bitdepth-1.0));
    m_maximumDynamicRange = 20.0*std::log10(std::pow(2.0,bitdepth)/2.0);
}

uint32_t PeakMeter::getMaximum(const std::vector<int32_t> & signalValues)
{
    m_maxValue = 0;
    m_absoluteValue = 0;
    for(const auto& signalValue : signalValues)
    {
        m_absoluteValue = std::abs(signalValue);
        m_maxValue = std::max(m_maxValue, m_absoluteValue);
    }
    return m_maxValue;
}

double PeakMeter::calculatePeak(int currentValue, int referenceValue)
{
    if(m_currentValue > 0)
    {
        return 20.0*std::log10(static_cast<double>(currentValue)/static_cast<double>(referenceValue));
    }
    else
    {
        return INF;
    }
}

void PeakMeter::emitPeakValue(double peak)
{
    // Check if the calculated value is greater than the value which is currently being displayed
    if(peak > m_actualValue)
    {
        m_actualValue = peak;
        // Check if the holder needs to be updated
        if(peak >= -60.0)
        {
            m_peakMeterListener->receivePeakHolderValue(m_actualValue);
        }
    }
    // Check if the signal is clipping
    else if(peak >= 0.0)
    {
        m_actualValue = 0.0;
    }

    // Update current meter value
    m_peakMeterListener->receivePeakMeterValue(m_actualValue);

    // Decrement current value if the calculated value is smaller (return time)
    if(peak < m_actualValue)
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

void PeakMeter::setReturnTimeValue(double value)
{
    m_returnTimeValue = value;
}

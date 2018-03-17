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

#ifndef RMSMETER_H
#define RMSMETER_H

#include <cstdint>
#include <vector>

class RMSMeterListener
{
public:
    RMSMeterListener() {}

    virtual void receiveRmsMeterValue(double rms) = 0;
    virtual void receiveRmsHolderValue(double rms) = 0;
};

class RMSMeter
{
public:
    RMSMeter(RMSMeterListener *listener = nullptr);

public:
    // Set the time for meter return
    void setReturnTimeValue(double value);
    void updateMeter(const std::vector<int32_t> & signalValues);
    void updateBitdepth(int bitdepth);

private:
    double calculateRootMeanSquare(const std::vector<int32_t> & signalValues);
    void emitRmsValue(double rms);

private:
    RMSMeterListener *m_rmsListener;
    double m_actualValue;
    double m_returnTimeValue;
    uint32_t m_referenceValue;
    int32_t m_i;
    double m_maximumDynamicRange;
};

#endif // RMSMETER_H

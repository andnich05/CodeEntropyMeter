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

#ifndef PEAKMETER_H
#define PEAKMETER_H

#include <cstdint>
#include <vector>

class PeakMeterListener {
public:
    PeakMeterListener() {}

    virtual void receivePeakMeterValue(double value) = 0;
    virtual void receivePeakHolderValue(double value) = 0;
};

class PeakMeter {
public:
    PeakMeter(PeakMeterListener *listener = nullptr);
    // Set the time for meter return
    void setReturnTimeValue(double value);
    void updateMeter(const std::vector<int32_t> & signalValues);
    void updateBitdepth(int bitdepth);

private:
    // Get maximum value of all samples
    uint32_t getMaximum(const std::vector<int32_t> & signalValues);
    // Convert to dB
    double calculatePeak(int currentValue, int referenceValue);
    // Pass the value to MeterDisplay
    void emitPeakValue(double peak);

private:
    PeakMeterListener *peakMeterListener;
    double actualValue;
    double returnTimeValue;
    uint32_t currentValue;
    uint32_t referenceValue;
    uint32_t maxValue;
    uint32_t absoluteValue;
    double maximumDynamicRange;
};

#endif // PEAKMETER_H

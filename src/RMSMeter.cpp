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
#include <math.h>
#include <QDebug>

const double INF = -999;

RMSMeter::RMSMeter(QWidget *parent)
    : QWidget(parent) {

    actualValue = -60;
    returnTimeValue = 0;
    referenceValue = 0;
    maximumDynamicRange = 0;
}

void RMSMeter::updateMeter(QList<qint32> *signalValues) {
    emitRmsValue(calculateRootMeanSquare(signalValues));
}

void RMSMeter::updateBitdepth(int bitdepth) {
    referenceValue = pow(2, bitdepth-1);
    maximumDynamicRange = 20*log10(pow(2,bitdepth)/(double)2);
}

double RMSMeter::calculateRootMeanSquare(QList<qint32> *signalValues) {
    long double rms = 0;

    // Square
    for(i=0; i<signalValues->size(); i++) {
        rms += pow(signalValues->at(i), 2);
    }

    // Mean + Root
    rms = sqrt(rms / signalValues->size());

    // Convert to dB if rms isn't zero
    if(rms > 0) {
        if(rms < 1) {
            rms = 1;
        }
        rms = 20*log10(rms/(double)referenceValue);
        return rms;
    }
    else {
        return INF;
    }
}

void RMSMeter::emitRmsValue(double rms) {
    // Check if the calculated value is greater than the value which is currently being displayed
    if(rms > actualValue) {
        actualValue = rms;
        // Check if the holder needs to be updated
        if(rms >= -60) {
            emit signalUpdateRmsHolder(actualValue);
        }
    }
    // Check if the signal is clipping
    else if(rms >= 0) {
        actualValue = 0;
    }

    // Update current meter value
    emit signalUpdateRmsMeter(actualValue);

    // Decrement current value if the calculated value is smaller (return time)
    if(rms < actualValue) {
        if(actualValue > -maximumDynamicRange) {
            actualValue -= returnTimeValue;
        }
        else {
            actualValue = INF;
        }
    }
}

void RMSMeter::setReturnTimeValue(double value) {
    this->returnTimeValue = value;
}

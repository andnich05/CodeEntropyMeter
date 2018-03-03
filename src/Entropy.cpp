/*
 * Entropy: Calculation of entropy of audio samples
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

#include "Entropy.hpp"
#include <cmath>
#include <QDebug>

const double LOG2 = log10(2);

Entropy::Entropy() {
    // Propabilities for positive symbols, range is from 0 to 2^24/2-1
    counterArrayPositive = new quint32[0x800000];

    // Propabilities for negative symbols, range is from -1 to -2^24/2
    counterArrayNegative = new quint32[0x800000];

    i = 0;
    numberOfSymbols = pow(2,16);
    numberOfSamples = 0;
    entropy = 0.0;
    blockCounter = 0;
    numberOfBlocks = 50;
    probabilityPositive = 0;
    probabilityNegative = 0;
}

Entropy::~Entropy() {
    delete [] counterArrayNegative;
    delete [] counterArrayPositive;
}

void Entropy::countValues(QVector<qint32> signalValues) {
    i = 0;

    // Clear all arrays if its the first block
    if(blockCounter == 0) {
        clear();
    }

    // Count how often each symbol occurs
    mutex.lock();
    for(i=0; i<(quint32)signalValues.size(); i++) {
        if(signalValues.at(i) >= 0) {
            counterArrayPositive[signalValues.at(i)]++;
            //qDebug() << counterArrayPositive[signalValues.at(i)] << signalValues.at(i);
        }
        else {
            counterArrayNegative[-signalValues.at(i)]++;
            //qDebug() << counterArrayNegative[signalValues.at(i)] << signalValues.at(i);
        }

    }
    mutex.unlock();
    blockCounter++;

    // Calculate entropy if all blocks have been processed
    if(blockCounter == numberOfBlocks) {
        calculateEntropy(signalValues.size());
        blockCounter = 0;
    }
}

void Entropy::calculateEntropy(int blockSize) {
    entropy = 0.0;
    probabilityPositive = 0;
    probabilityNegative = 0;
    numberOfSamples = blockSize*numberOfBlocks;
    mutex.lock();
    for(i=0; i<numberOfSymbols/2; i++) {

        // Calculate propabilities
        probabilityPositive = (double)counterArrayPositive[i]/(numberOfSamples);
        probabilityNegative = (double)counterArrayNegative[i+1]/(numberOfSamples);

        // Calculate entropy
        if(probabilityPositive > 0.0) {
            entropy += probabilityPositive * (log10(probabilityPositive));
        }
        if(probabilityNegative > 0.0) {
            entropy += probabilityNegative * (log10(probabilityNegative));
        }
    }
    entropy = -entropy/LOG2;
    emit finished(entropy);
    mutex.unlock();
}

void Entropy::setNumberOfBlocks(int numberOfBlocks) {
    this->numberOfBlocks = numberOfBlocks;
}

void Entropy::setNumberOfSymbols(int bitdepth) {
    numberOfSymbols = pow(2,bitdepth);
}

void Entropy::clear() {
    entropy = 0.0;
    // Fill arrays with zeros
    for(i=0; i<numberOfSymbols/2; i++) {
        counterArrayPositive[i] = 0.0;
        counterArrayNegative[i+1] = 0.0;
    }
}

void Entropy::reset() {
    clear();
    blockCounter = 0;
    //emit finished(0);
}

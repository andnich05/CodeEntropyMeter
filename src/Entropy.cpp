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

Entropy::Entropy(EntropyListener *listener)
    : entropyListener(listener)
    , counterArrayPositive(0x800000) // Propabilities for positive symbols, range is from 0 to 2^24/2-1
    , counterArrayNegative(0x800000) // Propabilities for negative symbols, range is from -1 to -2^24/2
    , probabilityPositive(0.0)
    , probabilityNegative(0.0)
    , numberOfSymbols(pow(2,16))
    , numberOfSamples(0)
    , i(0)
    , entropy(0.0)
    , blockCounter(0)
    , numberOfBlocks(50)
    , blockSize(0)
{
}

void Entropy::addSamples(const std::vector<int32_t> & signalValues) {
    if(!entropyListener)
    {
        return;
    }

    // Clear all arrays if its the first block
    if(blockCounter == 0) {
        clear();
    }

    // Count how often each symbol occurs
    for(i=0; i<signalValues.size(); ++i) {
        if(signalValues.at(i) >= 0) {
            ++counterArrayPositive[signalValues[i]];
        }
        else {
            ++counterArrayNegative[-signalValues[i]];
        }

    }
    ++blockCounter;

    // Calculate entropy if all blocks have been processed
    if(blockCounter == numberOfBlocks) {
        entropyListener->receiveEntropy(calculateEntropy(static_cast<int>(signalValues.size())));
        blockCounter = 0;
    }
}

double Entropy::calculateEntropy(int blockSize) {
    entropy = 0.0;
    probabilityPositive = 0;
    probabilityNegative = 0;
    numberOfSamples = blockSize*numberOfBlocks;
    for(i=0; i<numberOfSymbols/2; i++) {

        // Calculate propabilities
        probabilityPositive = (double)counterArrayPositive[i]/numberOfSamples;
        probabilityNegative = (double)counterArrayNegative[i+1]/numberOfSamples;

        // Calculate entropy
        if(probabilityPositive > 0.0) {
            entropy += probabilityPositive * (log10(probabilityPositive));
        }
        if(probabilityNegative > 0.0) {
            entropy += probabilityNegative * (log10(probabilityNegative));
        }
    }
    entropy = -entropy/LOG2;
    return entropy;
}

void Entropy::setNumberOfBlocks(int numberOfBlocks) {
    this->numberOfBlocks = numberOfBlocks;
}

void Entropy::setNumberOfSymbols(int bitdepth) {
    numberOfSymbols = pow(2,bitdepth);
}

void Entropy::clear() {
    entropy = 0.0;
    std::fill(counterArrayPositive.begin(), counterArrayPositive.end(), 0);
    std::fill(counterArrayNegative.begin(), counterArrayNegative.end(), 0);
}

void Entropy::reset() {
    clear();
    blockCounter = 0;
}

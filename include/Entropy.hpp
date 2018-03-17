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

#ifndef ENTROPY_H
#define ENTROPY_H

#include <cstdint>
#include <map>
#include <vector>

class EntropyListener
{
public:
    EntropyListener() {}

    virtual void receiveEntropy(double entropy) = 0;
};

class Entropy
{

public:
    Entropy(EntropyListener *listener = nullptr);

public:
    void addSamples(const std::vector<int32_t> & signalValues);
    // Set number of blocks to process
    void setNumberOfBlocks(int numberOfBlocks);
    // Set new numberOfSymbols if bitdepth has changed
    void setNumberOfSymbols(int bitdepth);
    // Clear everything
    void clear();
    // Reset blockCounter if "Stop" has been pressed
    void reset();

private:
    void calculateEntropy(int blockSize);

private:
    EntropyListener *m_entropyListener;
    double m_probability;
    uint32_t m_numberOfSymbols;
    uint32_t m_numberOfSamples;
    double m_entropy;
    int m_blockCounter;
    int m_numberOfBlocks;
    int m_blockSize;
    std::map<int32_t, uint32_t> m_mapSymbolsToOccurrence;
};

#endif // ENTROPY_H

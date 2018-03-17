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

const double LOG2 = log10(2);

Entropy::Entropy(EntropyListener *listener)
    : m_entropyListener(listener)
    , m_probability(0.0)
    , m_numberOfSymbols(static_cast<uint32_t>(pow(2,16)))
    , m_numberOfSamples(0)
    , m_entropy(0.0)
    , m_blockCounter(0)
    , m_numberOfBlocks(50)
    , m_blockSize(0)
{
}

void Entropy::addSamples(const std::vector<int32_t> & signalValues)
{
    if(!m_entropyListener)
    {
        return;
    }

    // Reset everything if its the first block
    if(m_blockCounter == 0)
    {
        clear();
    }

    // Count how often each symbol occurs
    for(const auto& signalValue : signalValues)
    {
        m_mapSymbolsToOccurrence[signalValue]++;
    }
    ++m_blockCounter;

    // Calculate entropy if all blocks have been processed
    if(m_blockCounter == m_numberOfBlocks)
    {
        calculateEntropy(static_cast<int>(signalValues.size()));
        m_entropyListener->receiveEntropy(m_entropy);
        m_blockCounter = 0;
    }
}

void Entropy::calculateEntropy(int blockSize)
{
    m_entropy = 0.0;
    m_probability = 0.0;
    m_numberOfSamples = blockSize*m_numberOfBlocks;
    for(const auto& symbolCount : m_mapSymbolsToOccurrence)
    {
        // Calculate propabilities
        m_probability = static_cast<double>(symbolCount.second)/m_numberOfSamples;
        // Calculate entropy
        m_entropy += m_probability * (std::log10(m_probability));
    }
    m_entropy = -m_entropy/LOG2;
}

void Entropy::setNumberOfBlocks(int numberOfBlocks)
{
    m_numberOfBlocks = numberOfBlocks;
}

void Entropy::setNumberOfSymbols(int bitdepth)
{
    m_numberOfSymbols = static_cast<uint32_t>(pow(2,bitdepth));
}

void Entropy::clear()
{
    m_entropy = 0.0;
    m_probability = 0.0;
    m_entropy = 0.0;
    m_blockCounter = 0;
    m_mapSymbolsToOccurrence.clear();
}

void Entropy::reset()
{
    clear();
}

/*
 * RingBuffer: Ring buffer fot storing callback samples
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

#include "RingBuffer.hpp"

RingBuffer::RingBuffer(int capacity, RingBufferReceiver *receiver)
    : m_inBuffer(capacity, 0)
    , m_receiverObject(receiver)
{
}

void RingBuffer::clearAndResize(int capacity)
{
    m_inBuffer.assign(capacity, 0);
}

void RingBuffer::insertItem(int32_t item, uint32_t position)
{
    // Write sample to first buffer
    m_inBuffer[position] = item;
    // Check if buffer is full
    if(static_cast<size_t>(position) == m_inBuffer.size()-1)
    {
        // Lock mutex to prevent the callback function from writing new samples to first buffer
        std::lock_guard<std::mutex> lock(m_mutex);
        // Copy first buffer to second buffer
        m_inBuffer;
        // Second buffer is ready to be read
        if(m_receiverObject)
        {
            m_receiverObject->receiveSamples(m_inBuffer);
        }
    }
}

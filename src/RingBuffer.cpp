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
#include <QDebug>

RingBuffer::RingBuffer(int capacity, QWidget *parent)
    : QWidget(parent) {

    // Allocate memory for both buffers by setting capacity
    inBuffer.fill(0, capacity);
    outBuffer.fill(0, capacity);
}

void RingBuffer::clearAndResize(int capacity) {
    // Allocate memory for both buffers by setting capacity
    inBuffer.fill(0, capacity);
    outBuffer.fill(0, capacity);
}

void RingBuffer::insertItem(qint32 item, quint32 position) {
    // Write sample to first buffer
    inBuffer[position] = item;
    // Check if buffer is full
    if(static_cast< int >( position ) == inBuffer.size()-1) {
        // Lock mutex to prevent the callback function from writing new samples to first buffer
        mutex.lock();
        // Copy first buffer to second buffer
        outBuffer = inBuffer;
        // Unlock mutex
        mutex.unlock();
        // Second buffer is ready to be read
        emitBufferReadyToBeRead();
    }
}

void RingBuffer::emitBufferReadyToBeRead() {
    emit signalBufferReadyToBeRead(outBuffer);
}

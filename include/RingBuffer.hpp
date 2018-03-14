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

#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <memory>
#include <mutex>
#include <vector>

class RingBufferReceiver {
public:
    RingBufferReceiver() {}

    virtual void receiveSamples(const std::vector<int32_t> & samples) = 0;
};

class RingBuffer {

public:
    RingBuffer(int capacity, RingBufferReceiver *receiver = nullptr);
    void clearAndResize(int capacity);
    // Insert sample at position
    void insertItem(int32_t item, uint32_t position);

private:
    // Buffer to which is written by the callback function
    std::vector<int32_t> inBuffer;
    // Buffer to pass to further processing
    std::vector<int32_t> outBuffer;
    // Mutex to lock while copying from inBuffer to outBuffer
    std::mutex mutex;
    RingBufferReceiver *receiverObject;

//private slots:
//    void emitBufferReadyToBeRead();

//signals:
//    void signalBufferReadyToBeRead(const std::vector<int32_t> & samples);

};

#endif // RINGBUFFER_H

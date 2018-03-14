/*
 * PortAudioIO: PortAudio callback
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

#ifndef PORTAUDIOIO_H
#define PORTAUDIOIO_H

#include <cstdint>
#include <memory>

#include "portaudio.h"

class RingBuffer;

class PortAudioIO {

public:
    PortAudioIO();
    // PortAudio callback function
    static int getInputCallback(const void *input, void *output, unsigned long frameCount,
                                             const PaStreamCallbackTimeInfo* timeInfo,
                                             PaStreamCallbackFlags statusFlags, void *userData);

    // Array with custom user data which is passed to the callback function
    typedef struct {
        std::shared_ptr<RingBuffer> buffer;
        int bitDepth;
        bool littleEndian;
        int channel;
    } paTestData;

private:
    static uint32_t sampleCounter;
    static const int8_t *bufferPointer;

    // 8 bit pointer used to iterate trhough the memory addresses
    static int32_t sampleData;
    static uint32_t s1;
    static uint32_t s2;
    static uint32_t s3;

    static paTestData *data;
};

#endif // PORTAUDIOIO_H

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

#include "PortAudioIO.hpp"
#include "RingBuffer.hpp"

// static variables
uint32_t PortAudioIO::sampleCounter = 0;

// Used to clear the signed bits after converting a signed integer to unsigned (AND operation)
static const uint32_t clearFirst24BitsOf32BitsAND = 255;
static const uint32_t clearFirst16BitsOf32BitsAND = 65535;
static const uint32_t clearFirst8BitsOf32BitsAND = 16777215;

PortAudioIO::PortAudioIO()
{

}

int PortAudioIO::getInputCallback(const void *input, void *output, unsigned long frameCount,
                                  const PaStreamCallbackTimeInfo* timeInfo,
                                  PaStreamCallbackFlags statusFlags, void *userData)
{
    // Prevent compiler warnings
    (void) output;
    (void) timeInfo;
    (void) statusFlags;

    int32_t sampleData = 0;
    uint32_t s1 = 0;
    uint32_t s2 = 0;
    uint32_t s3 = 0;
    PortAudioUserData *data = nullptr;
    const int8_t *bufferPointer;

    bufferPointer = static_cast<const int8_t *>(input);
    data = static_cast<PortAudioUserData *>(userData);

    for(unsigned int i=0; i<frameCount; i++)
    {
        if(data->m_bitDepth == 8)
        {
            for(int i=1; i<data->m_channel; i++)
            {
                bufferPointer++;
            }
            sampleData = static_cast<int32_t>(*bufferPointer++);
        }
        else if(data->m_bitDepth == 16)
        {
            for(int i=1; i<data->m_channel; i++)
            {
                bufferPointer++;
                bufferPointer++;
            }
            if(data->m_littleEndian == true)
            {
                s1 = static_cast<uint32_t>(*bufferPointer++) & clearFirst24BitsOf32BitsAND;
                s2 = static_cast<uint32_t>(*bufferPointer++) << 8;
                sampleData = static_cast<int32_t>(s1 | s2);
            }
            else
            {
                s1 = static_cast<uint32_t>(*bufferPointer++) << 8;
                s2 = static_cast<uint32_t>(*bufferPointer++) & clearFirst24BitsOf32BitsAND;
                sampleData = static_cast<int32_t>(s1 | s2);
            }
        }
        else if(data->m_bitDepth == 24)
        {
            for(int i=1; i<data->m_channel; i++)
            {
                bufferPointer++;
                bufferPointer++;
                bufferPointer++;
            }
            if(data->m_littleEndian == true)
            {
                s1 = static_cast<uint32_t>(*bufferPointer++) & clearFirst24BitsOf32BitsAND;
                s2 = (static_cast<uint32_t>(*bufferPointer++) << 8) & clearFirst16BitsOf32BitsAND;
                s3 = static_cast<uint32_t>(*bufferPointer++) << 16;
                sampleData = static_cast<int32_t>(s1 | s2 | s3);
            }
            else
            {
                s1 = static_cast<uint32_t>(*bufferPointer++) << 16;
                s2 = (static_cast<uint32_t>(*bufferPointer++) << 8) & clearFirst16BitsOf32BitsAND;
                s3 = static_cast<uint32_t>(*bufferPointer++) & clearFirst24BitsOf32BitsAND;
                sampleData = static_cast<int32_t>(s1 | s2 | s3);
            }
        }

        // Write sample value to the buffer
        data->m_buffer->insertItem(sampleData, sampleCounter);
        ++sampleCounter;

        // Reset sampleCounter if a whole block has been processed
        if(sampleCounter == frameCount)
        {
            sampleCounter = 0;
        }
    }
    return 0;
}

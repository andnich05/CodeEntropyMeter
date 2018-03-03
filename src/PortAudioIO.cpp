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
#include <QDebug>

// static variables
quint32 PortAudioIO::sampleCounter = 0;
const qint8 *PortAudioIO::bufferPointer = NULL;
qint32 PortAudioIO::sampleData = 0;
quint32 PortAudioIO::s1 = 0;
quint32 PortAudioIO::s2 = 0;
quint32 PortAudioIO::s3 = 0;
PortAudioIO::paTestData *PortAudioIO::data = NULL;

// Used to clear the signed bits after converting a signed integer to unsigned (AND operation)
static const quint32 clearFirst24BitsOf32BitsAND = 255;
static const quint32 clearFirst16BitsOf32BitsAND = 65535;
static const quint32 clearFirst8BitsOf32BitsAND = 16777215;

PortAudioIO::PortAudioIO() {

}

int PortAudioIO::getInputCallback(const void *input, void *output, unsigned long frameCount,
                                  const PaStreamCallbackTimeInfo* timeInfo,
                                  PaStreamCallbackFlags statusFlags, void *userData) {
    // Prevent compiler warnings
    (void) output;
    (void) timeInfo;
    (void) statusFlags;

    bufferPointer = (const qint8*)input;
    data = (paTestData*)userData;

    for(unsigned int i=0; i<frameCount; i++) {
        if(data->bitDepth == 8) {
            for(int i=1; i<data->channel; i++) {
                *bufferPointer++;
            }
            sampleData = (qint32)(*bufferPointer++);
        }
        else if(data->bitDepth == 16) {
            for(int i=1; i<data->channel; i++) {
                *bufferPointer++;
                *bufferPointer++;
            }
            if(data->littleEndian == true) {
                s1 = (quint32)(*bufferPointer++) & clearFirst24BitsOf32BitsAND;
                s2 = ((quint32)(*bufferPointer++) << 8);
                sampleData = (qint32)(s1 | s2);
            }
            else {
                s1 = (quint32)(*bufferPointer++) << 8;
                s2 = (quint32)(*bufferPointer++) & clearFirst24BitsOf32BitsAND;
                sampleData = (qint32)(s1 | s2);
            }
        }
        else if(data->bitDepth == 24) {
            for(int i=1; i<data->channel; i++) {
                *bufferPointer++;
                *bufferPointer++;
                *bufferPointer++;
            }
            if(data->littleEndian == true) {
                s1 = (quint32)(*bufferPointer++) & clearFirst24BitsOf32BitsAND;
                s2 = ((quint32)(*bufferPointer++) << 8) & clearFirst16BitsOf32BitsAND;
                s3 = (quint32)(*bufferPointer++) << 16;
                sampleData = (qint32)(s1 | s2 | s3);
            }
            else {
                s1 = (quint32)(*bufferPointer++) << 16;
                s2 = ((quint32)(*bufferPointer++) << 8) & clearFirst16BitsOf32BitsAND;
                s3 = (quint32)(*bufferPointer++) & clearFirst24BitsOf32BitsAND;
                sampleData = (qint32)(s1 | s2 | s3);
            }
        }

        // Write sample value to the buffer
        data->buffer->insertItem(sampleData, sampleCounter);
        sampleCounter++;

        // Reset sampleCounter if a whole block has been processed
        if(sampleCounter == frameCount) {
            sampleCounter = 0;
        }
    }
    return 0;
}

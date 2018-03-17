/*
 * PortAudioControl: Control of PortAudio
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

#ifndef PORTAUDIOCONTROL_H
#define PORTAUDIOCONTROL_H

#include <vector>
#include <portaudio.h>

#include "PortAudioIO.hpp"
#include "RingBuffer.hpp"

class PortAudioControlListener
{
public:
    PortAudioControlListener() {}

    virtual void receivePortAudioSamples(const std::vector<int32_t> & samples) = 0;
};

class PortAudioControl
    : public RingBufferReceiver
{
public:
    PortAudioControl(PortAudioControlListener *listener = nullptr);
    // Initialize PortAudio, return "true" if everything is okay
    bool initialize();
    // Get an array with all PortAudio devices
    const std::vector<PaDeviceInfo> & getPaDeviceInfo();
    // Get info about a specific host api
    const PaHostApiInfo & getApiInfo(int apiIndex);
    // Get the supported samples rates for a specific device
    const std::vector<uint32_t> & getSupportedSampleRates(int deviceNumber);
    // Open stream with given parameters
    bool openStream(int deviceNumber, int channel, int bitDepth, uint32_t sampleRate, uint32_t blockSize);
    void closeStream();

    virtual void receiveSamples(const std::vector<int32_t> & samples) override;

private:
    PortAudioControlListener *m_controlListener;
    PaStream *m_stream;
    std::shared_ptr<RingBuffer> m_buffer;
    // Array with custom data to pass to the callback function
    PortAudioIO::PortAudioUserData m_data;
    std::vector<PaDeviceInfo> m_deviceInfos;
    PaHostApiInfo m_apiInfo;
    std::vector<uint32_t> m_supportedSampleRates;
};

#endif // PORTAUDIOCONTROL_H

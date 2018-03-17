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

#include "PortAudioControl.hpp"

#include <iostream>

PortAudioControl::PortAudioControl(PortAudioControlListener *listener)
    : RingBufferReceiver()
    , m_controlListener(listener)
    , m_stream(nullptr)
    , m_buffer(new RingBuffer(50000, this))
{
    m_data.m_buffer = m_buffer;
    m_data.m_littleEndian = true;
    m_data.m_bitDepth = 16;
    m_data.m_channel = 0;
}

bool PortAudioControl::initialize()
{
    PaError err = Pa_Initialize();
    if(err != paNoError)
    {
        std::cout << "Error initializing PortAudio" << std::endl;
        return false;
    }
    std::cout << "PortAudio version:" << Pa_GetVersion() << ";" << Pa_GetVersionText() << std::endl;
    return true;
}

const std::vector<PaDeviceInfo> & PortAudioControl::getPaDeviceInfo()
{
    m_deviceInfos.clear();
    int numberOfDevices = Pa_GetDeviceCount();
    if(numberOfDevices < 0)
    {
        std::cout << "ERROR: No devices found!" << std::endl;
        return m_deviceInfos;
    }
    else
    {
        std::cout << "Number of devices found:" << numberOfDevices << std::endl;
    }

    for(int i=0; i<numberOfDevices; i++)
    {
        m_deviceInfos.push_back(*Pa_GetDeviceInfo(i));
    }
    return m_deviceInfos;
}

const PaHostApiInfo & PortAudioControl::getApiInfo(int apiIndex)
{
    m_apiInfo = *Pa_GetHostApiInfo(apiIndex);
    return m_apiInfo;
}

const std::vector<uint32_t> & PortAudioControl::getSupportedSampleRates(int deviceNumber)
{
    m_supportedSampleRates.clear();

    const PaDeviceInfo *deviceInfo = Pa_GetDeviceInfo(deviceNumber);

    PaStreamParameters inputParameters;
    inputParameters.device = deviceNumber;
    inputParameters.channelCount = deviceInfo->maxInputChannels;
    inputParameters.sampleFormat = paInt16;
    inputParameters.suggestedLatency = 0;
    inputParameters.hostApiSpecificStreamInfo = nullptr;

    // Sample rates to test
    static std::vector<double> standardSampleRates =
    {
        8000.0, 9600.0, 11025.0, 12000.0, 16000.0, 22050.0, 24000.0, 32000.0,
        44100.0, 48000.0, 88200.0, 96000.0, 192000.0, -1
    };

    PaError err;

    for(const auto& sampleRate : standardSampleRates)
    {
        err = Pa_IsFormatSupported(&inputParameters, NULL, sampleRate);
        if(err == paFormatIsSupported)
        {
            m_supportedSampleRates.push_back(static_cast<uint32_t>(sampleRate));
        }
    }

    return m_supportedSampleRates;
}

bool PortAudioControl::openStream(int deviceNumber, int channel, int bitDepth, uint32_t sampleRate, uint32_t blockSize)
{
    m_buffer->clearAndResize(blockSize);

    PaSampleFormat sampleFormat;
    switch(bitDepth)
    {
        case 8:
            sampleFormat = paUInt8;
            break;
        case 16:
            sampleFormat = paInt16;
            break;
        case 24:
            sampleFormat = paInt24;
            break;
        default:
            // ???
            sampleFormat = paInt16;
            break;
    }

    PaStreamParameters inputParameters;
    inputParameters.device = deviceNumber;
    inputParameters.channelCount = channel;
    inputParameters.sampleFormat = sampleFormat;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo(inputParameters.device)->defaultLowInputLatency;
    inputParameters.hostApiSpecificStreamInfo = nullptr;

    m_data.m_bitDepth = bitDepth;
    m_data.m_littleEndian = true;
    m_data.m_channel = channel;

    // Test if the chosen input parameters are supported before opening stream
    if(Pa_IsFormatSupported(&inputParameters, nullptr, sampleRate) != paFormatIsSupported)
    {
        std::cout << "Format not supported" << std::endl;
        return false;
    }

    std::cout << Pa_GetDeviceInfo(deviceNumber)->name << std::endl;

    PaError err = Pa_OpenStream(&m_stream, &inputParameters, NULL, sampleRate, blockSize, paNoFlag, PortAudioIO::getInputCallback, &m_data);
    if(err != paNoError)
    {
        std::cout << Pa_GetErrorText(err) << std::endl;
        std::cout << Pa_GetLastHostErrorInfo()->errorText << std::endl;
        return false;
    }
    else
    {
        std::cout << "- Stream openend -" << std::endl;
        std::cout << "Name:" << Pa_GetDeviceInfo(inputParameters.device)->name << "| Sample rate:" << sampleRate << "| Bitdepth:" << bitDepth << "| Input channel:" << channel << std::endl;
    }

    err = Pa_StartStream(m_stream);
    if(err != paNoError)
    {
        std::cout << "ERROR: Could not start stream!" << std::endl;
        return false;
    }
    return true;
}

void PortAudioControl::closeStream()
{
    if(Pa_IsStreamActive(m_stream))
    {
        Pa_CloseStream(m_stream);
        std::cout << "- Stream closed -" << std::endl;
    }
    else
    {
        std::cout << "- Stream already closed -" << std::endl;
    }
}

void PortAudioControl::receiveSamples(const std::vector<int32_t> & samples)
{
    if(m_controlListener)
    {
        m_controlListener->receivePortAudioSamples(samples);
    }
}

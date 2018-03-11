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
#include "RingBuffer.hpp"
//#include "pa_asio.h"
//#include "pa_win_wasapi.h"

#include <QDebug>

PortAudioControl::PortAudioControl(QWidget *parent)
    : QWidget(parent) {
    buffer = new RingBuffer(50000, this);

    connect(buffer, SIGNAL(signalBufferReadyToBeRead(const QVector<qint32> &)), this, SLOT(readBuffer(const QVector<qint32> &)));

    data.buffer = buffer;
    data.littleEndian = true;
    data.bitDepth = 16;
}

PortAudioControl::~PortAudioControl() {

}

bool PortAudioControl::initialize() {
    PaError err = Pa_Initialize();
    if(err != paNoError) {
        qDebug() << "Error initializing PortAudio";
        return false;
    }
    qDebug() << "PortAudio version:" << Pa_GetVersion() << ";" << Pa_GetVersionText();
    return true;
}

QList<PaDeviceInfo> PortAudioControl::getPaDeviceInfo() {
    QList<PaDeviceInfo> deviceInfos;
    int numberOfDevices = Pa_GetDeviceCount();
    if(numberOfDevices < 0) {
        qDebug() << "ERROR: No devices found!";
        return deviceInfos;
    }
    else {
        qDebug() << "Number of devices found:" << numberOfDevices;
    }

    for(int i=0; i<numberOfDevices; i++) {
        deviceInfos.append(*Pa_GetDeviceInfo(i));
    }
    return deviceInfos;
}

PaHostApiInfo PortAudioControl::getApiInfo(int apiIndex) {
    PaHostApiInfo apiInfo = *Pa_GetHostApiInfo(apiIndex);
    return apiInfo;
}

QList<quint32> PortAudioControl::getSupportedSampleRates(int deviceNumber) {
    QList<quint32> supportedSampleRates;

    const PaDeviceInfo *deviceInfo = Pa_GetDeviceInfo(deviceNumber);

    PaStreamParameters inputParameters;
    inputParameters.device = deviceNumber;
    inputParameters.channelCount = deviceInfo->maxInputChannels;
    inputParameters.sampleFormat = paInt16;
    inputParameters.suggestedLatency = 0;

    // Experimental: WASAPI in exclusive mode (needs "pa_win_wasapi.h")
    /*if(Pa_GetHostApiInfo(Pa_GetDeviceInfo(deviceNumber)->hostApi)->type == paWASAPI) {
        PaWasapiStreamInfo wasapi;
        wasapi.size = sizeof(PaWasapiStreamInfo);
        wasapi.hostApiType = paWASAPI;
        wasapi.version = 1;
        wasapi.flags = paWinWasapiExclusive;
        wasapi.channelMask = NULL;
        wasapi.hostProcessorOutput = NULL;
        wasapi.hostProcessorInput = NULL;
        inputParameters.hostApiSpecificStreamInfo = &wasapi;
    }*/
    //else {
        inputParameters.hostApiSpecificStreamInfo = NULL;
    //}


    // Sample rates to test
    static double standardSampleRates[] = {
        8000.0, 9600.0, 11025.0, 12000.0, 16000.0, 22050.0, 24000.0, 32000.0,
        44100.0, 48000.0, 88200.0, 96000.0, 192000.0, -1
    };

    PaError err;

    for(int i=0; standardSampleRates[i]>0; i++) {
        err = Pa_IsFormatSupported(&inputParameters, NULL, standardSampleRates[i]);
        if(err == paFormatIsSupported) {
            supportedSampleRates.append(standardSampleRates[i]);
        }
    }

    return supportedSampleRates;
}

bool PortAudioControl::openStream(int deviceNumber, int channel, int bitDepth, quint32 sampleRate, quint32 blockSize) {
    buffer->clearAndResize(blockSize);

    PaSampleFormat sampleFormat;
    if(bitDepth == 8) sampleFormat = paUInt8;
    else if(bitDepth == 16) sampleFormat = paInt16;
    else if(bitDepth == 24) sampleFormat = paInt24;

    PaStreamParameters inputParameters;
    inputParameters.device = deviceNumber;
    inputParameters.channelCount = channel;
    inputParameters.sampleFormat = sampleFormat;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo(inputParameters.device)->defaultLowInputLatency;

    // Experimental: WASAPI in exclusive mode (needs "pa_win_wasapi.h")
    /*if(Pa_GetHostApiInfo(Pa_GetDeviceInfo(deviceNumber)->hostApi)->type == paWASAPI) {
        PaWasapiStreamInfo wasapi;
        wasapi.size = sizeof(PaWasapiStreamInfo);
        wasapi.hostApiType = paWASAPI;
        wasapi.version = 1;
        wasapi.flags = paWinWasapiExclusive;
        wasapi.channelMask = NULL;
        wasapi.hostProcessorOutput = NULL;
        wasapi.hostProcessorInput = NULL;
        inputParameters.hostApiSpecificStreamInfo = &wasapi;
    }*/
    //else {
        inputParameters.hostApiSpecificStreamInfo = NULL;
    //}

    data.bitDepth = bitDepth;
    data.littleEndian = true;
    data.channel = channel;

    // Test if the chosen input parameters are supported before opening stream
    if(Pa_IsFormatSupported(&inputParameters, NULL, sampleRate) != paFormatIsSupported) {
        qDebug() << "Format not supported";
        return false;
    }

    qDebug() << Pa_GetDeviceInfo(deviceNumber)->name;

    PaError err = Pa_OpenStream(&stream, &inputParameters, NULL, sampleRate, blockSize, paNoFlag, PortAudioIO::getInputCallback, &data);
    if(err != paNoError) {
        qDebug() << Pa_GetErrorText(err);
        qDebug() << Pa_GetLastHostErrorInfo()->errorText;
        return false;
    }
    else {
        qDebug() << "- Stream openend -";
        qDebug() << "Name:" << Pa_GetDeviceInfo(inputParameters.device)->name << "| Sample rate:" << sampleRate << "| Bitdepth:" << bitDepth << "| Input channel:" << channel;
    }

    err = Pa_StartStream(stream);
    if(err != paNoError) {
        qDebug() << "ERROR: Could not start stream!";
        return false;
    }
    return true;
}

void PortAudioControl::readBuffer(const QVector<qint32> & samples) {
    emit signalSampleListReady(samples);
}

void PortAudioControl::closeStream() {
    Pa_CloseStream(stream);
    qDebug() << "- Stream closed -";
}

/*
void PortAudioControl::showAsioPanel(int deviceId, qint32 id) {
    PaAsio_ShowControlPanel(deviceId, (void*)id);
}
*/

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

#include <QWidget>
#include "PortAudioIO.hpp"
#include <portaudio.h>

class RingBuffer;

class PortAudioControl : public QWidget {
    Q_OBJECT

public:
    PortAudioControl(QWidget *parent = 0);
    ~PortAudioControl();
    // Initialize PortAudio, return "true" if everything is okay
    bool initialize();
    // Get an array with all PortAudio devices
    QList<PaDeviceInfo> getPaDeviceInfo();
    // Get info about a specific host api
    PaHostApiInfo getApiInfo(int apiIndex);
    // Get the supported samples rates for a specific device
    QList<quint32> getSupportedSampleRates(int deviceNumber);
    // Open stream with given parameters
    bool openStream(int deviceNumber, int channel, int bitDepth, quint32 sampleRate, quint32 blockSize);
    void closeStream();

private:
    PaStream *stream;
    RingBuffer *buffer;
    // Array with custom data to pass to the callback function
    PortAudioIO::paTestData data;

public slots:
    // Show Asio panel, for experimental purposes (currently not beeing used)
    //void showAsioPanel(int deviceId, qint32 id);
    // Get buffer and pass it to the MainWindow class
    void readBuffer(QVector<qint32> samples);

signals:
    void signalSampleListReady(QList<qint32> *sampleList);

};

#endif // PORTAUDIOCONTROL_H

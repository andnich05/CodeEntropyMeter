/*
 * MainWindow: Central Qt-UI element
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>
#include <QMainWindow>
#include <QThread>
#include "portaudio.h"
#include "PortAudioControl.hpp"
#include "Entropy.hpp"
#include "PeakMeter.hpp"
#include "RMSMeter.hpp"

class OptionPanel;
class BitDisplay;
class MeterDisplay;
class EntropyDisplay;
class InfoWindow;

class QHBoxLayout;
class QVBoxLayout;

class MainWindow
    : public QMainWindow
    , public PortAudioControlListener
    , public EntropyListener
    , public PeakMeterListener
    , public RMSMeterListener
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    virtual ~MainWindow ();

public:
    virtual void receivePortAudioSamples(const std::vector<int32_t> & samples);

    virtual void receiveEntropy(double entropy);

    virtual void receivePeakHolderValue(double value);
    virtual void receivePeakMeterValue(double value);

    virtual void receiveRmsHolderValue(double rms);
    virtual void receiveRmsMeterValue(double rms);
private:
    // Struct with information of all input devices
    struct device {
        QString name;
        int deviceIndex;
        PaHostApiTypeId hostApi;
        int maxInputChannels;
        std::vector<uint32_t> supportedSampleRates;
    };
    QList<device> devices;

    // Struct with selected parameters in optionsPanel
    struct selectedParameters {
        int hostApiId;
        int device;
        PaDeviceIndex deviceIndex;
        quint32 sampleRate;
        PaSampleFormat sampleFormat;
        quint32 blockSize;
        int bitDepth;
        int channel;
    } parameters;

    // Objects
    OptionPanel *optionsPanel;
    BitDisplay *bitDisplay;
    std::unique_ptr<Entropy> entropy;
    PeakMeter *peakMeter;
    std::unique_ptr<RMSMeter> rmsMeter;
    MeterDisplay *meterDisplay;
    std::unique_ptr<PortAudioControl> portAudioControl;
    EntropyDisplay *entropyDisplay;
    InfoWindow *infoWindow;

    QHBoxLayout *mainHLayout;
    QVBoxLayout *mainVLayout;
    QHBoxLayout *mainLayout;

    void initializeUI();
    // Fill device info struct
    bool getDeviceInformation();
    // Fill UI elements of optionsPanel
    bool setOptions();
    void connectUI();

protected:
    void resizeEvent(QResizeEvent *event);
    void closeEvent(QCloseEvent *event);

private slots:
    // Start processing
    void start();
    // Stop processing
    void stop();

    void anotherApiSelected(int api);
    void anotherDeviceSelected(int device);
    void anotherSampleRateSelected(int sampleRate);
    void anotherBlockSizeSelected(int blockSize);
    void anotherBitDepthSelected(int bits);
    void anotherChannelSelected(int channel);
    void setEntropyNumberOfBlocks(int numberOfBlocks);
    void showAsioPanel();
    void showInfoWindow();
    void updateEntropyDisplay(double entropy);
    void updatePeakHolder(double value);
    void updatePeakMeter(double value);
    void updateRmsHolder(double value);
    void updateRmsMeter(double value);

signals:
    void signalUpdateEntropyDisplay(double entropy);
    void signalUpdatePeakHolder(double value);
    void signalUpdatePeakMeter(double value);
    void signalUpdateRmsHolder(double value);
    void signalUpdateRmsMeter(double value);
};


#endif // MAINWINDOW_H

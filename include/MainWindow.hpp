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

#include <QMainWindow>

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
    virtual void receivePortAudioSamples(const std::vector<int32_t> & samples) override;

    virtual void receiveEntropy(double entropy) override;

    virtual void receivePeakHolderValue(double value) override;
    virtual void receivePeakMeterValue(double value) override;

    virtual void receiveRmsHolderValue(double rms) override;
    virtual void receiveRmsMeterValue(double rms) override;

private:
    // Struct with information of all input devices
    struct DeviceInformation
    {
        std::string m_name;
        int m_deviceIndex;
        PaHostApiTypeId m_hostApi;
        int m_maxInputChannels;
        std::vector<uint32_t> m_supportedSampleRates;
    };
    std::vector<DeviceInformation> m_devices;

    // Struct with selected parameters in optionsPanel
    struct SelectedParameters
    {
        int m_hostApiId;
        int m_device;
        PaDeviceIndex m_deviceIndex;
        quint32 m_sampleRate;
        PaSampleFormat m_sampleFormat;
        quint32 m_blockSize;
        int m_bitDepth;
        int m_channel;
    };
    SelectedParameters m_parameters;

    // Objects
    OptionPanel *m_optionsPanel;
    BitDisplay *m_bitDisplay;
    std::unique_ptr<Entropy> m_entropy;
    PeakMeter *m_peakMeter;
    std::unique_ptr<RMSMeter> m_rmsMeter;
    MeterDisplay *m_meterDisplay;
    std::unique_ptr<PortAudioControl> m_portAudioControl;
    EntropyDisplay *m_entropyDisplay;
    InfoWindow *m_infoWindow;

    QHBoxLayout *m_mainHLayout;
    QVBoxLayout *m_mainVLayout;
    QHBoxLayout *m_mainLayout;

    void initializeUI();
    // Fill device info struct
    bool getDeviceInformation();
    // Fill UI elements of optionsPanel
    bool setOptions();
    void connectUI();

protected:
    virtual void resizeEvent(QResizeEvent *event) override;
    virtual void closeEvent(QCloseEvent *event) override;

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

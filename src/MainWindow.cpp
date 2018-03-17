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

#include "MainWindow.hpp"

#include "BitDisplay.hpp"
#include "OptionPanel.hpp"
//#include "PeakMeter.hpp"
//#include "RMSMeter.hpp"
#include "MeterDisplay.hpp"
//#include "PortAudioControl.hpp"
#include "EntropyDisplay.hpp"

//#include "Entropy.hpp"
#include "InfoWindow.hpp"

//#include <QComboBox>
#include <QDebug>
#include <QLayout>
//#include <QPushButton>
//#include <QSpinBox>
//#include <QThread>
//#include <QVector>
#include <QGroupBox>

const QColor colorBackground(50,50,50);
const QColor colorWidgetBackground(80,80,80);
const QColor colorFont(255,255,255);
const QColor colorFrame(80,80,80);

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Code Entropy Meter");

    initializeUI();

    if(!m_portAudioControl->initialize())
    {
        return;
    }

    if(!getDeviceInformation())
    {
        return;
    }

    if(!setOptions())
    {
        return;
    }

    connectUI();

    m_parameters.m_bitDepth = 16;
    m_parameters.m_blockSize = 2048;
    m_parameters.m_channel = 1;
    m_parameters.m_device = 0;
    m_parameters.m_deviceIndex = 0;
    m_parameters.m_hostApiId = 0;
    m_parameters.m_sampleFormat = paInt16;
    m_parameters.m_sampleRate = 44100;

    anotherApiSelected(m_devices.at(0).m_hostApi);
    anotherDeviceSelected(0);
    anotherChannelSelected(1);
    anotherBitDepthSelected(16);
    anotherSampleRateSelected(44100);
    anotherBlockSizeSelected(2048);
    m_bitDisplay->setSampleMaximum(2048);
}

void MainWindow::receivePortAudioSamples(const std::vector<int32_t> & samples)
{
    m_peakMeter->updateMeter(samples);
    m_rmsMeter->updateMeter(samples);
    m_bitDisplay->updateDisplay(samples, m_parameters.m_bitDepth);
    m_entropy->addSamples(samples);
}

void MainWindow::receiveEntropy(double entropy)
{
    emit signalUpdateEntropyDisplay(entropy);
}

void MainWindow::receivePeakHolderValue(double value)
{
    emit signalUpdatePeakHolder(value);
}

void MainWindow::receivePeakMeterValue(double value)
{
    emit signalUpdatePeakMeter(value);
}

void MainWindow::receiveRmsHolderValue(double rms)
{
    emit signalUpdateRmsHolder(rms);
}

void MainWindow::receiveRmsMeterValue(double rms)
{
    emit signalUpdateRmsMeter(rms);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    (void) event;
    m_infoWindow->setGeometry(size().width()/2-125, size().height()/2-50,250,100);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    (void) event;
    m_infoWindow->close();
}

void MainWindow::initializeUI()
{
    setFixedSize(510,540);

    m_optionsPanel = new OptionPanel(this);
    m_optionsPanel->setObjectName("optionsPanel");
    m_optionsPanel->disableStopButton(true);

    QHBoxLayout *optionsLayout = new QHBoxLayout();
    optionsLayout->addWidget(m_optionsPanel);
    optionsLayout->setContentsMargins(0,0,0,0);
    QGroupBox *boxOptions = new QGroupBox("", this);
    boxOptions->setLayout(optionsLayout);

    m_bitDisplay = new BitDisplay(this);
    m_bitDisplay->setNumberOfBits(16);
    m_bitDisplay->setAutoFillBackground(true);
    m_bitDisplay->setObjectName("bitDisplay");

    QHBoxLayout *bitDisplayLayout = new QHBoxLayout();
    bitDisplayLayout->addWidget(m_bitDisplay);
    bitDisplayLayout->setContentsMargins(0,0,0,0);
    QGroupBox *boxBitDisplay = new QGroupBox("", this);
    boxBitDisplay->setLayout(bitDisplayLayout);

    m_entropyDisplay = new EntropyDisplay(this);
    m_entropyDisplay->setAutoFillBackground(true);
    m_entropyDisplay->setObjectName("entropyDisplay");

    QHBoxLayout *entropyDisplayLayout = new QHBoxLayout();
    entropyDisplayLayout->addWidget(m_entropyDisplay);
    entropyDisplayLayout->setContentsMargins(0,0,0,0);
    QGroupBox *boxEntropyDisplay = new QGroupBox("", this);
    boxEntropyDisplay->setLayout(entropyDisplayLayout);

    m_entropy.reset(new Entropy(this));

    m_peakMeter = new PeakMeter(this);
    m_rmsMeter.reset(new RMSMeter(this));

    m_meterDisplay = new MeterDisplay(this);
    m_meterDisplay->setObjectName("meterDisplay");

    QHBoxLayout *meterDisplayLayout = new QHBoxLayout();
    meterDisplayLayout->addWidget(m_meterDisplay);
    meterDisplayLayout->setContentsMargins(0,0,0,0);
    QGroupBox *boxMeters = new QGroupBox("", this);
    boxMeters->setFixedWidth(135);
    boxMeters->setLayout(meterDisplayLayout);

    m_portAudioControl.reset(new PortAudioControl(this));

    m_mainVLayout = new QVBoxLayout();
    m_mainVLayout->addWidget(boxBitDisplay,1);
    m_mainVLayout->addWidget(boxEntropyDisplay,1);
    m_mainVLayout->addWidget(boxOptions,4);
    m_mainVLayout->setSpacing(2);

    m_mainHLayout = new QHBoxLayout();
    m_mainHLayout->setContentsMargins(0,0,0,0);
    m_mainHLayout->setSpacing(2);
    m_mainHLayout->addWidget(boxMeters);
    m_meterDisplay->setFixedWidth(135);

    m_mainLayout = new QHBoxLayout();
    m_mainLayout->addLayout(m_mainVLayout,1);
    m_mainLayout->addLayout(m_mainHLayout,2);
    m_mainLayout->setSpacing(2);

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(m_mainLayout);
    setCentralWidget(centralWidget);

    m_infoWindow = new InfoWindow();
    m_infoWindow->setObjectName("infoWindow");
    m_infoWindow->setFixedSize(300,330);
    m_infoWindow->setWindowTitle("Information");
    //infoWindow->hide();

    boxEntropyDisplay->setStyleSheet("QGroupBox { border: 1px outset " + colorFrame.name() + "; }");
    boxBitDisplay->setStyleSheet("QGroupBox { border: 1px outset " + colorFrame.name() + ";  }");
    boxOptions->setStyleSheet("QGroupBox { border: 1px outset " + colorFrame.name() + "; }");
    boxMeters->setStyleSheet("QGroupBox { border: 1px outset " + colorFrame.name() + "; }");

    setStyleSheet("QMainWindow { background-color: " + colorBackground.name() + "; }"
                        "QWidget#optionsPanel { background-color: " + colorWidgetBackground.name() + "; }"
                        "QWidget#bitDisplay { background-color: " + colorWidgetBackground.name() + "; }"
                        "QWidget#meterDisplay { background-color: " + colorWidgetBackground.name() + "; }"
                        "QWidget#infoWindow { background-color: white; border: 2px outset grey }"
                        "QWidget#entropyDisplay { background-color: " + colorWidgetBackground.name() + "; }");

}

MainWindow::~MainWindow()
{
    // Stop portaudio stream
    stop();
}

bool MainWindow::getDeviceInformation()
{
    int numberOfDevices = Pa_GetDeviceCount();
    if(numberOfDevices < 1)
    {
        qDebug() << "No devices found!";
        return false;
    }
    else
    {
        qDebug() << "Number of devices:" << numberOfDevices;
    }

    m_devices.clear();
    const std::vector<PaDeviceInfo> & deviceInfo = m_portAudioControl->getPaDeviceInfo();
    if(deviceInfo.empty())
    {
        return false;
    }

    for(size_t i=0; i<deviceInfo.size(); i++)
    {
        if(deviceInfo[i].maxInputChannels > 0)
        {
            DeviceInformation d;
            d.m_name = deviceInfo[i].name;
            d.m_deviceIndex = static_cast<int>(i);
            d.m_hostApi = m_portAudioControl->getApiInfo(deviceInfo[i].hostApi).type;
            d.m_maxInputChannels = deviceInfo[i].maxInputChannels;
            d.m_supportedSampleRates = m_portAudioControl->getSupportedSampleRates(static_cast<int>(i));
            m_devices.push_back(d);
        }
    }

    return true;
}

bool MainWindow::setOptions()
{
    QList<QString> hostList;
    QList<int> apiIds;
    for(int i=0; i<Pa_GetHostApiCount(); i++)
    {
        int apiTypeId = m_portAudioControl->getApiInfo(i).type;
        if(m_portAudioControl->getApiInfo(i).deviceCount > 0)
        {
            hostList.append(m_portAudioControl->getApiInfo(i).name);
            apiIds.append(apiTypeId);
        }
    }
    if(hostList.size() > 0)
    {
        m_optionsPanel->setHostApis(hostList, apiIds);
    }

    QList<int> bitDepths;
    bitDepths << 8 << 16 << 24;
    m_optionsPanel->setBitDepths(bitDepths);

    return true;
}

void MainWindow::connectUI()
{
    connect(m_optionsPanel, SIGNAL(signalStartButtonPressed()), this, SLOT(start()));
    connect(m_optionsPanel, SIGNAL(signalStopButtonPressed()), this, SLOT(stop()));
    connect(m_optionsPanel, SIGNAL(signalBitDepthChanged(int)), this, SLOT(anotherBitDepthSelected(int)));
    connect(m_optionsPanel, SIGNAL(signalSampleRateChanged(int)), this, SLOT(anotherSampleRateSelected(int)));
    connect(m_optionsPanel, SIGNAL(signalBlockSizeChanged(int)), this, SLOT(anotherBlockSizeSelected(int)));
    connect(m_optionsPanel, SIGNAL(signalHostApiChanged(int)), this, SLOT(anotherApiSelected(int)));
    connect(m_optionsPanel, SIGNAL(signalInputDeviceChanged(int)), this, SLOT(anotherDeviceSelected(int)));
    connect(m_optionsPanel, SIGNAL(signalInputChannelChanged(int)), this, SLOT(anotherChannelSelected(int)));
    connect(this, SIGNAL(signalUpdatePeakMeter(double)), this, SLOT(updatePeakMeter(double)));
    connect(this, SIGNAL(signalUpdatePeakHolder(double)), this, SLOT(updatePeakHolder(double)));
    connect(this, SIGNAL(signalUpdateRmsMeter(double)), this, SLOT(updateRmsMeter(double)));
    connect(this, SIGNAL(signalUpdateRmsHolder(double)), this, SLOT(updateRmsHolder(double)));
    connect(this, SIGNAL(signalUpdateEntropyDisplay(double)), this, SLOT(updateEntropyDisplay(double)));
    connect(m_entropyDisplay, SIGNAL(signalNumberOfBlocksChanged(int)), this, SLOT(setEntropyNumberOfBlocks(int)));
    connect(m_optionsPanel, SIGNAL(signalInfoButtonPressed()), this, SLOT(showInfoWindow()));
}

void MainWindow::anotherApiSelected(int api)
{
    m_parameters.m_hostApiId = api;
    QList<QString> devicesList;
    QList<int> deviceIds;
    for(size_t i=0; i<m_devices.size(); i++)
    {
        if(m_devices[i].m_hostApi == api)
        {
            devicesList.append(QString::fromStdString(m_devices[i].m_name));
            deviceIds.append(i);
        }
    }
    m_optionsPanel->setInputDevices(devicesList, deviceIds);
}

void MainWindow::anotherDeviceSelected(int device)
{
    m_parameters.m_device = device;
    m_parameters.m_deviceIndex = m_devices.at(device).m_deviceIndex;
    m_optionsPanel->setSampleRates(m_devices.at(device).m_supportedSampleRates);
    m_optionsPanel->setChannels(m_devices.at(device).m_maxInputChannels);
}

void MainWindow::anotherSampleRateSelected(int sampleRate)
{
    m_parameters.m_sampleRate = sampleRate;
    setEntropyNumberOfBlocks(m_entropyDisplay->getNumberOfBlocks());
    m_peakMeter->setReturnTimeValue((static_cast<double>(m_parameters.m_blockSize)/static_cast<double>(sampleRate))*(20.0/1.7));
    m_rmsMeter->setReturnTimeValue((static_cast<double>(m_parameters.m_blockSize)/static_cast<double>(sampleRate))*(20.0/1.7));
}

void MainWindow::anotherBlockSizeSelected(int blockSize)
{
    m_parameters.m_blockSize = blockSize;
    setEntropyNumberOfBlocks(m_entropyDisplay->getNumberOfBlocks());
    m_peakMeter->setReturnTimeValue((static_cast<double>(blockSize)/static_cast<double>(m_parameters.m_sampleRate))*(20.0/1.7));
    m_rmsMeter->setReturnTimeValue((static_cast<double>(blockSize)/static_cast<double>(m_parameters.m_sampleRate))*(20.0/1.7));
    m_bitDisplay->setSampleMaximum(blockSize);
}

void MainWindow::anotherBitDepthSelected(int bits)
{
    m_parameters.m_bitDepth = bits;
    m_bitDisplay->setNumberOfBits(bits);
    m_entropy->setNumberOfSymbols(bits);
    m_peakMeter->updateBitdepth(bits);
    m_rmsMeter->updateBitdepth(bits);
}

void MainWindow::anotherChannelSelected(int channel)
{
    m_parameters.m_channel = channel;
}

void MainWindow::start()
{
    m_optionsPanel->disableUI(true);
    m_entropyDisplay->disableUI(true);
    if(m_portAudioControl->openStream(m_parameters.m_deviceIndex, m_parameters.m_channel, m_parameters.m_bitDepth, m_parameters.m_sampleRate, m_parameters.m_blockSize) == false)
    {
        m_optionsPanel->disableUI(false);
        m_entropyDisplay->disableUI(false);
    }
}

void MainWindow::stop()
{
    m_portAudioControl->closeStream();
    m_entropy->reset();
    m_optionsPanel->disableUI(false);
    m_entropyDisplay->disableUI(false);
}

void MainWindow::showAsioPanel()
{
    //portAudioControl->showAsioPanel(devices[optionsPanel->boxAudioInputDevice->itemData(optionsPanel->boxAudioInputDevice->currentIndex()).toInt()].deviceIndex, winId());
}

void MainWindow::setEntropyNumberOfBlocks(int numberOfBlocks)
{
    m_entropy->setNumberOfBlocks(numberOfBlocks);
    m_entropyDisplay->updateIntegrationTimeLabel(static_cast<double>(m_parameters.m_blockSize)/static_cast<double>(m_parameters.m_sampleRate)*1000.0);
}

void MainWindow::showInfoWindow()
{
    if(m_infoWindow->isHidden())
    {
        m_infoWindow->setGeometry(geometry().center().x()-m_infoWindow->size().width()/2,
                                geometry().center().y()-m_infoWindow->size().height()/2,
                                m_infoWindow->size().width(),
                                m_infoWindow->size().height());
        m_infoWindow->show();
    }
    else
    {
        m_infoWindow->hide();
    }
}

void MainWindow::updateEntropyDisplay(double entropy)
{
    m_entropyDisplay->updateEntropy(entropy);
}

void MainWindow::updatePeakHolder(double value)
{
    m_meterDisplay->updatePeakHolder(value);
}

void MainWindow::updatePeakMeter(double value)
{
    m_meterDisplay->updatePeakMeter(value);
}

void MainWindow::updateRmsHolder(double value)
{
    m_meterDisplay->updateRmsHolder(value);
}

void MainWindow::updateRmsMeter(double value)
{
   m_meterDisplay->updateRmsMeter(value);
}

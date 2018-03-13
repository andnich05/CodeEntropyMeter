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

#include "BitDisplay.hpp"
#include "OptionPanel.hpp"
#include "PeakMeter.hpp"
#include "RMSMeter.hpp"
#include "MeterDisplay.hpp"
#include "PortAudioControl.hpp"
#include "EntropyDisplay.hpp"
#include "MainWindow.hpp"
#include "Entropy.hpp"
#include "InfoWindow.hpp"

#include <QComboBox>
#include <QDebug>
#include <QLayout>
#include <QPushButton>
#include <QSpinBox>
#include <QThread>
#include <QVector>
#include <QGroupBox>

const QColor colorBackground(50,50,50);
const QColor colorWidgetBackground(80,80,80);
const QColor colorFont(255,255,255);
const QColor colorFrame(80,80,80);

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {

    this->setWindowTitle("Code Entropy Meter");

    initializeUI();

    if(portAudioControl->initialize() == false) {
        return;
    }

    if(getDeviceInformation() == false) {
        return;
    }

    if(setOptions() == false) {
        return;
    }

    connectUI();

    parameters.bitDepth = 16;
    parameters.blockSize = 2048;
    parameters.channel = 1;
    parameters.device = 0;
    parameters.deviceIndex = 0;
    parameters.hostApiId = 0;
    parameters.sampleFormat = paInt16;
    parameters.sampleRate = 44100;

    anotherApiSelected(devices.at(0).hostApi);
    anotherDeviceSelected(0);
    anotherChannelSelected(1);
    anotherBitDepthSelected(16);
    anotherSampleRateSelected(44100);
    anotherBlockSizeSelected(2048);
    bitDisplay->setSampleMaximum(2048);
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    (void) event;
    infoWindow->setGeometry(this->size().width()/2-125, this->size().height()/2-50,250,100);
}

void MainWindow::closeEvent(QCloseEvent *event) {
    (void) event;
    infoWindow->close();
}

void MainWindow::initializeUI() {
    this->setFixedSize(510,540);

    optionsPanel = new OptionPanel(this);
    optionsPanel->setObjectName("optionsPanel");
    optionsPanel->disableStopButton(true);

    QHBoxLayout *optionsLayout = new QHBoxLayout();
    optionsLayout->addWidget(optionsPanel);
    optionsLayout->setContentsMargins(0,0,0,0);
    QGroupBox *boxOptions = new QGroupBox("", this);
    boxOptions->setLayout(optionsLayout);

    bitDisplay = new BitDisplay(this);
    bitDisplay->setNumberOfBits(16);
    bitDisplay->setAutoFillBackground(true);
    bitDisplay->setObjectName("bitDisplay");

    QHBoxLayout *bitDisplayLayout = new QHBoxLayout();
    bitDisplayLayout->addWidget(bitDisplay);
    bitDisplayLayout->setContentsMargins(0,0,0,0);
    QGroupBox *boxBitDisplay = new QGroupBox("", this);
    boxBitDisplay->setLayout(bitDisplayLayout);

    entropyDisplay = new EntropyDisplay(this);
    entropyDisplay->setAutoFillBackground(true);
    entropyDisplay->setObjectName("entropyDisplay");

    QHBoxLayout *entropyDisplayLayout = new QHBoxLayout();
    entropyDisplayLayout->addWidget(entropyDisplay);
    entropyDisplayLayout->setContentsMargins(0,0,0,0);
    QGroupBox *boxEntropyDisplay = new QGroupBox("", this);
    boxEntropyDisplay->setLayout(entropyDisplayLayout);

    entropy = new Entropy();
    // Move entropy calculation to another thread to prevent a blocked UI
    entropy->moveToThread(&workerThread);
    workerThread.start();

    peakMeter = new PeakMeter(this);
    rmsMeter = new RMSMeter(this);

    meterDisplay = new MeterDisplay(this);
    meterDisplay->setObjectName("meterDisplay");

    QHBoxLayout *meterDisplayLayout = new QHBoxLayout();
    meterDisplayLayout->addWidget(meterDisplay);
    meterDisplayLayout->setContentsMargins(0,0,0,0);
    QGroupBox *boxMeters = new QGroupBox("", this);
    boxMeters->setFixedWidth(135);
    boxMeters->setLayout(meterDisplayLayout);

    portAudioControl = new PortAudioControl(this);

    mainVLayout = new QVBoxLayout();
    mainVLayout->addWidget(boxBitDisplay,1);
    mainVLayout->addWidget(boxEntropyDisplay,1);
    mainVLayout->addWidget(boxOptions,4);
    mainVLayout->setSpacing(2);

    mainHLayout = new QHBoxLayout();
    mainHLayout->setContentsMargins(0,0,0,0);
    mainHLayout->setSpacing(2);
    mainHLayout->addWidget(boxMeters);
    meterDisplay->setFixedWidth(135);

    mainLayout = new QHBoxLayout();
    mainLayout->addLayout(mainVLayout,1);
    mainLayout->addLayout(mainHLayout,2);
    mainLayout->setSpacing(2);

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(mainLayout);
    this->setCentralWidget(centralWidget);

    infoWindow = new InfoWindow();
    infoWindow->setObjectName("infoWindow");
    infoWindow->setFixedSize(300,330);
    infoWindow->setWindowTitle("Information");
    //infoWindow->hide();

    boxEntropyDisplay->setStyleSheet("QGroupBox { border: 1px outset " + colorFrame.name() + "; }");
    boxBitDisplay->setStyleSheet("QGroupBox { border: 1px outset " + colorFrame.name() + ";  }");
    boxOptions->setStyleSheet("QGroupBox { border: 1px outset " + colorFrame.name() + "; }");
    boxMeters->setStyleSheet("QGroupBox { border: 1px outset " + colorFrame.name() + "; }");

    this->setStyleSheet("QMainWindow { background-color: " + colorBackground.name() + "; }"
                        "QWidget#optionsPanel { background-color: " + colorWidgetBackground.name() + "; }"
                        "QWidget#bitDisplay { background-color: " + colorWidgetBackground.name() + "; }"
                        "QWidget#meterDisplay { background-color: " + colorWidgetBackground.name() + "; }"
                        "QWidget#infoWindow { background-color: white; border: 2px outset grey }"
                        "QWidget#entropyDisplay { background-color: " + colorWidgetBackground.name() + "; }");

}

MainWindow::~MainWindow() {
    stop();
    workerThread.quit();
    workerThread.wait();
}

bool MainWindow::getDeviceInformation() {
    int numberOfDevices = Pa_GetDeviceCount();
    if(numberOfDevices < 1) {
        qDebug() << "No devices found!";
        return false;
    }
    else {
        qDebug() << "Number of devices:" << numberOfDevices;
    }

    devices.clear();
    QList<PaDeviceInfo> deviceInfo = portAudioControl->getPaDeviceInfo();
    if(deviceInfo.empty()) {
        return false;
    }

    for(int i=0; i<deviceInfo.size(); i++) {
        if(deviceInfo.at(i).maxInputChannels > 0) {
            device d;
            d.name = QString::fromStdString(deviceInfo[i].name);
            d.deviceIndex = i;
            d.hostApi = portAudioControl->getApiInfo(deviceInfo[i].hostApi).type;
            d.maxInputChannels = deviceInfo[i].maxInputChannels;
            d.supportedSampleRates = portAudioControl->getSupportedSampleRates(i);
            devices.append(d);
        }
    }

    return true;
}

bool MainWindow::setOptions() {
    QList<QString> hostList;
    QList<int> apiIds;
    for(int i=0; i<Pa_GetHostApiCount(); i++) {
        int apiTypeId = portAudioControl->getApiInfo(i).type;
        if(portAudioControl->getApiInfo(i).deviceCount > 0) {
            hostList.append(portAudioControl->getApiInfo(i).name);
            apiIds.append(apiTypeId);
        }
    }
    if(hostList.size() > 0) {
        optionsPanel->setHostApis(hostList, apiIds);
    }

    QList<int> bitDepths;
    bitDepths << 8 << 16 << 24;
    optionsPanel->setBitDepths(bitDepths);

    return true;
}

void MainWindow::connectUI() {
    connect(optionsPanel, SIGNAL(signalStartButtonPressed()), this, SLOT(start()));
    connect(optionsPanel, SIGNAL(signalStopButtonPressed()), this, SLOT(stop()));
    connect(optionsPanel, SIGNAL(signalBitDepthChanged(int)), this, SLOT(anotherBitDepthSelected(int)));
    connect(optionsPanel, SIGNAL(signalSampleRateChanged(int)), this, SLOT(anotherSampleRateSelected(int)));
    connect(optionsPanel, SIGNAL(signalBlockSizeChanged(int)), this, SLOT(anotherBlockSizeSelected(int)));
    connect(optionsPanel, SIGNAL(signalHostApiChanged(int)), this, SLOT(anotherApiSelected(int)));
    connect(optionsPanel, SIGNAL(signalInputDeviceChanged(int)), this, SLOT(anotherDeviceSelected(int)));
    connect(optionsPanel, SIGNAL(signalInputChannelChanged(int)), this, SLOT(anotherChannelSelected(int)));
    connect(peakMeter, SIGNAL(signalUpdatePeakMeter(double)), meterDisplay, SLOT(updatePeakMeter(double)));
    connect(peakMeter, SIGNAL(signalUpdatePeakHolder(double)), meterDisplay, SLOT(updatePeakHolder(double)));
    connect(rmsMeter, SIGNAL(signalUpdateRmsMeter(double)), meterDisplay, SLOT(updateRmsMeter(double)));
    connect(rmsMeter, SIGNAL(signalUpdateRmsHolder(double)), meterDisplay, SLOT(updateRmsHolder(double)));
    connect(portAudioControl, SIGNAL(signalSampleListReady(const QVector<qint32> &)), this, SLOT(updateEverything(const QVector<qint32> &)));
    connect(this, SIGNAL(signalUpdateEntropy(const QVector<qint32> &)), entropy, SLOT(countValues(const QVector<qint32> &)));
    connect(entropy, SIGNAL(finished(double)), entropyDisplay, SLOT(updateEntropy(double)));
    connect(entropyDisplay, SIGNAL(signalNumberOfBlocksChanged(int)), this, SLOT(setEntropyNumberOfBlocks(int)));
    connect(optionsPanel, SIGNAL(signalInfoButtonPressed()), this, SLOT(showInfoWindow()));
}

void MainWindow::anotherApiSelected(int api) {
    parameters.hostApiId = api;
    QList<QString> devicesList;
    QList<int> deviceIds;
    for(int i=0; i<devices.size(); i++) {
        if(devices[i].hostApi == api) {
            devicesList.append(devices[i].name);
            deviceIds.append(i);
        }
    }
    optionsPanel->setInputDevices(devicesList, deviceIds);
}

void MainWindow::anotherDeviceSelected(int device) {
    parameters.device = device;
    parameters.deviceIndex = devices.at(device).deviceIndex;
    optionsPanel->setSampleRates(devices.at(device).supportedSampleRates);
    optionsPanel->setChannels(devices.at(device).maxInputChannels);
}

void MainWindow::anotherSampleRateSelected(int sampleRate) {
    parameters.sampleRate = sampleRate;
    setEntropyNumberOfBlocks(entropyDisplay->getNumberOfBlocks());
    peakMeter->setReturnTimeValue(((double)parameters.blockSize/(double)sampleRate)*(20/1.7));
    rmsMeter->setReturnTimeValue(((double)parameters.blockSize/(double)sampleRate)*(20/1.7));
}

void MainWindow::anotherBlockSizeSelected(int blockSize) {
    parameters.blockSize = blockSize;
    setEntropyNumberOfBlocks(entropyDisplay->getNumberOfBlocks());
    peakMeter->setReturnTimeValue(((double)blockSize/(double)parameters.sampleRate)*(20/1.7));
    rmsMeter->setReturnTimeValue(((double)blockSize/(double)parameters.sampleRate)*(20/1.7));
    bitDisplay->setSampleMaximum(blockSize);
}

void MainWindow::anotherBitDepthSelected(int bits) {
    parameters.bitDepth = bits;
    bitDisplay->setNumberOfBits(bits);
    entropy->setNumberOfSymbols(bits);
    peakMeter->updateBitdepth(bits);
    rmsMeter->updateBitdepth(bits);
}

void MainWindow::anotherChannelSelected(int channel) {
    parameters.channel = channel;
}

void MainWindow::start() {
    optionsPanel->disableUI(true);
    entropyDisplay->disableUI(true);
    if(portAudioControl->openStream(parameters.deviceIndex, parameters.channel, parameters.bitDepth, parameters.sampleRate, parameters.blockSize) == false) {
        optionsPanel->disableUI(false);
        entropyDisplay->disableUI(false);
    }
}

void MainWindow::stop() {
    portAudioControl->closeStream();
    entropy->reset();
    optionsPanel->disableUI(false);
    entropyDisplay->disableUI(false);
}

void MainWindow::showAsioPanel() {
    //portAudioControl->showAsioPanel(devices[optionsPanel->boxAudioInputDevice->itemData(optionsPanel->boxAudioInputDevice->currentIndex()).toInt()].deviceIndex, this->winId());
}

void MainWindow::updateEverything(const QVector<qint32> & samples) {
    peakMeter->updateMeter(samples);
    rmsMeter->updateMeter(samples);
    bitDisplay->updateDisplay(samples, parameters.bitDepth);
    emit signalUpdateEntropy(samples);
}

void MainWindow::setEntropyNumberOfBlocks(int numberOfBlocks) {
    entropy->setNumberOfBlocks(numberOfBlocks);
    entropyDisplay->updateIntegrationTimeLabel((double)(parameters.blockSize)/(double)(parameters.sampleRate)*1000);
}

void MainWindow::showInfoWindow() {
    if(infoWindow->isHidden() == true) {
        infoWindow->setGeometry(this->geometry().center().x()-infoWindow->size().width()/2,
                                this->geometry().center().y()-infoWindow->size().height()/2,
                                infoWindow->size().width(),
                                infoWindow->size().height());
        infoWindow->show();
    }
    else {
        infoWindow->hide();
    }
}

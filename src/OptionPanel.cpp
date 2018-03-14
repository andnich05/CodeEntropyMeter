/*
 * OptionPanel: Representation of different options
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

#include <QLineEdit>
#include <QLabel>
#include <QLayout>
#include <QFormLayout>
#include <QComboBox>
#include <QSpinBox>
#include <QPushButton>
#include <QPainter>
#include <QDebug>

#include "OptionPanel.hpp"

const QColor colorFont(255,255,255);

OptionPanel::OptionPanel(QWidget *parent)
    : QWidget(parent) {

    this->setStyleSheet("QLabel {color: " + colorFont.name() + ";}");

    boxAudioInputDevice = new QComboBox(this);
    boxBitDepth = new QComboBox(this);
    boxBlockSize = new QSpinBox(this);
    boxBlockSize->setMinimum(128);
    boxBlockSize->setMaximum(65636);
    boxBlockSize->setValue(2048);
    boxHostAPI = new QComboBox(this);
    boxSampleRate = new QComboBox(this);
    boxInputChannel = new QComboBox(this);
    buttonStart = new QPushButton(trUtf8("Start"), this);
    buttonStop = new QPushButton(trUtf8("Stop"), this);
    buttonInfo = new QPushButton(trUtf8("?"), this);
    //buttonShowAsioPanel = new QPushButton(trUtf8("Show ASIO Panel"), this);

    formLayout = new QFormLayout();
    formLayout->addRow(trUtf8("Host API:"), boxHostAPI);
    formLayout->addRow(trUtf8("Input device:"), boxAudioInputDevice);
    formLayout->addRow(trUtf8("Input channel:"), boxInputChannel);
    formLayout->addRow(trUtf8("Bit depth:"), boxBitDepth);
    formLayout->addRow(trUtf8("Sample rate:"), boxSampleRate);
    formLayout->addRow(trUtf8("Block size:"), boxBlockSize);
    //formLayout->addRow(trUtf8(""), buttonShowAsioPanel);

    buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(buttonStart);
    buttonLayout->addWidget(buttonStop);

    QVBoxLayout *buttonInfoLayout = new QVBoxLayout();
    buttonInfoLayout->addWidget(buttonInfo);
    buttonInfoLayout->setAlignment(Qt::AlignLeft);
    buttonInfo->setMaximumWidth(30);

    QVBoxLayout *mainVLayout = new QVBoxLayout();
    mainLayout = new QVBoxLayout(this);
    mainVLayout->addLayout(formLayout);
    mainVLayout->addLayout(buttonLayout);
    mainVLayout->addLayout(buttonInfoLayout);
    mainVLayout->setAlignment(Qt::AlignTop);

    mainLayout->addLayout(mainVLayout);

    connect(boxHostAPI, SIGNAL(currentIndexChanged(int)), this, SLOT(emitHostApiChanged(int)));
    connect(boxAudioInputDevice, SIGNAL(currentIndexChanged(int)), this, SLOT(emitInputDeviceChanged(int)));
    connect(boxInputChannel, SIGNAL(currentIndexChanged(QString)), this, SLOT(emitInputChannelChanged(QString)));
    connect(boxBitDepth, SIGNAL(currentIndexChanged(QString)), this, SLOT(emitBitDepthChanged(QString)));
    connect(boxSampleRate, SIGNAL(currentIndexChanged(QString)), this, SLOT(emitSampleRateChanged(QString)));
    connect(boxBlockSize, SIGNAL(valueChanged(int)), this, SLOT(emitBlockSizeChanged(int)));
    connect(buttonStart, SIGNAL(clicked()), this, SLOT(emitStartButtonPressed()));
    connect(buttonStop, SIGNAL(clicked()), this, SLOT(emitStopButtonPressed()));
    connect(buttonInfo, SIGNAL(clicked()), this, SLOT(emitInfoButtonPressed()));
}

void OptionPanel::paintEvent(QPaintEvent *) {
     QStyleOption opt;
     opt.init(this);
     QPainter p(this);
     style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
 }

void OptionPanel::setHostApis(QList<QString> name, QList<int> apiIndex) {
    boxHostAPI->clear();
    for(int i=0; i<name.size(); i++) {
        boxHostAPI->addItem(name.at(i), apiIndex.at(i));
    }
}

void OptionPanel::setInputDevices(QList<QString> name, QList<int> boxIndex) {
    boxAudioInputDevice->clear();
    for(int i=0; i<name.size(); i++) {
        boxAudioInputDevice->addItem(name.at(i), boxIndex.at(i));
    }
}

void OptionPanel::setChannels(int numberOfChannels) {
    boxInputChannel->clear();
    for(int i=0; i<numberOfChannels; i++) {
        boxInputChannel->addItem(QString::number(i+1));
    }
}

void OptionPanel::setBitDepths(QList<int> bitDepths) {
    boxBitDepth->clear();
    for(int i=0; i<bitDepths.size(); i++) {
        boxBitDepth->addItem(QString::number(bitDepths.at(i)));
        if(bitDepths.at(i) == 16) {
            boxBitDepth->setCurrentIndex(i);
        }
    }
}

void OptionPanel::setSampleRates(const std::vector<uint32_t> & sampleRates) {
    boxSampleRate->clear();
    for(size_t i=0; i<sampleRates.size(); i++) {
        boxSampleRate->addItem(QString::number(sampleRates.at(i)));
        if(sampleRates.at(i) == 44100) {
            boxSampleRate->setCurrentIndex(static_cast<int>(i));
        }
    }
}

void OptionPanel::disableStartButton(bool disable) {
    buttonStart->setDisabled(disable);
}

void OptionPanel::disableStopButton(bool disable) {
    buttonStop->setDisabled(disable);
}

void OptionPanel::disableUI(bool disable) {
    boxAudioInputDevice->setDisabled(disable);
    boxBitDepth->setDisabled(disable);
    boxBlockSize->setDisabled(disable);
    boxHostAPI->setDisabled(disable);
    boxInputChannel->setDisabled(disable);
    boxSampleRate->setDisabled(disable);
    buttonStart->setDisabled(disable);
    buttonStop->setDisabled(!disable);
}

void OptionPanel::emitHostApiChanged(int index) {
    emit signalHostApiChanged(boxHostAPI->itemData(index).toInt());
}

void OptionPanel::emitInputDeviceChanged(int index) {
    emit signalInputDeviceChanged(boxAudioInputDevice->itemData(index).toInt());
}

void OptionPanel::emitInputChannelChanged(QString channel) {
    emit signalInputChannelChanged(channel.toInt());
}

void OptionPanel::emitBitDepthChanged(QString bitDepth) {
    emit signalBitDepthChanged(bitDepth.toInt());
}

void OptionPanel::emitSampleRateChanged(QString sampleRate) {
    emit signalSampleRateChanged(sampleRate.toInt());
}

void OptionPanel::emitBlockSizeChanged(int blockSize) {
    emit signalBlockSizeChanged(blockSize);
}

void OptionPanel::emitStartButtonPressed() {
    emit signalStartButtonPressed();
}

void OptionPanel::emitStopButtonPressed() {
    emit signalStopButtonPressed();
}

void OptionPanel::emitInfoButtonPressed() {
    emit signalInfoButtonPressed();
}

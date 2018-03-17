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

#include "OptionPanel.hpp"

//#include <QLineEdit>
//#include <QLabel>
#include <QLayout>
#include <QFormLayout>
#include <QComboBox>
#include <QSpinBox>
#include <QPushButton>
#include <QPainter>
//#include <QDebug>

const QColor colorFont(255,255,255);

OptionPanel::OptionPanel(QWidget *parent)
    : QWidget(parent)
{
    setStyleSheet("QLabel {color: " + colorFont.name() + ";}");

    m_boxAudioInputDevice = new QComboBox(this);
    m_boxBitDepth = new QComboBox(this);
    m_boxBlockSize = new QSpinBox(this);
    m_boxBlockSize->setMinimum(128);
    m_boxBlockSize->setMaximum(65636);
    m_boxBlockSize->setValue(2048);
    m_boxHostAPI = new QComboBox(this);
    m_boxSampleRate = new QComboBox(this);
    m_boxInputChannel = new QComboBox(this);
    m_buttonStart = new QPushButton(trUtf8("Start"), this);
    m_buttonStop = new QPushButton(trUtf8("Stop"), this);
    m_buttonInfo = new QPushButton(trUtf8("?"), this);
    //m_buttonShowAsioPanel = new QPushButton(trUtf8("Show ASIO Panel"), this);

    m_formLayout = new QFormLayout();
    m_formLayout->addRow(trUtf8("Host API:"), m_boxHostAPI);
    m_formLayout->addRow(trUtf8("Input device:"), m_boxAudioInputDevice);
    m_formLayout->addRow(trUtf8("Input channel:"), m_boxInputChannel);
    m_formLayout->addRow(trUtf8("Bit depth:"), m_boxBitDepth);
    m_formLayout->addRow(trUtf8("Sample rate:"), m_boxSampleRate);
    m_formLayout->addRow(trUtf8("Block size:"), m_boxBlockSize);
    //m_formLayout->addRow(trUtf8(""), m_buttonShowAsioPanel);

    m_buttonLayout = new QHBoxLayout();
    m_buttonLayout->addWidget(m_buttonStart);
    m_buttonLayout->addWidget(m_buttonStop);

    QVBoxLayout *buttonInfoLayout = new QVBoxLayout();
    buttonInfoLayout->addWidget(m_buttonInfo);
    buttonInfoLayout->setAlignment(Qt::AlignLeft);
    m_buttonInfo->setMaximumWidth(30);

    QVBoxLayout *mainVLayout = new QVBoxLayout();
    m_mainLayout = new QVBoxLayout(this);
    mainVLayout->addLayout(m_formLayout);
    mainVLayout->addLayout(m_buttonLayout);
    mainVLayout->addLayout(buttonInfoLayout);
    mainVLayout->setAlignment(Qt::AlignTop);

    m_mainLayout->addLayout(mainVLayout);

    connect(m_boxHostAPI, SIGNAL(currentIndexChanged(int)), this, SLOT(emitHostApiChanged(int)));
    connect(m_boxAudioInputDevice, SIGNAL(currentIndexChanged(int)), this, SLOT(emitInputDeviceChanged(int)));
    connect(m_boxInputChannel, SIGNAL(currentIndexChanged(QString)), this, SLOT(emitInputChannelChanged(QString)));
    connect(m_boxBitDepth, SIGNAL(currentIndexChanged(QString)), this, SLOT(emitBitDepthChanged(QString)));
    connect(m_boxSampleRate, SIGNAL(currentIndexChanged(QString)), this, SLOT(emitSampleRateChanged(QString)));
    connect(m_boxBlockSize, SIGNAL(valueChanged(int)), this, SLOT(emitBlockSizeChanged(int)));
    connect(m_buttonStart, SIGNAL(clicked()), this, SLOT(emitStartButtonPressed()));
    connect(m_buttonStop, SIGNAL(clicked()), this, SLOT(emitStopButtonPressed()));
    connect(m_buttonInfo, SIGNAL(clicked()), this, SLOT(emitInfoButtonPressed()));
}

void OptionPanel::paintEvent(QPaintEvent *)
{
     QStyleOption opt;
     opt.init(this);
     QPainter p(this);
     style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
 }

void OptionPanel::setHostApis(QList<QString> name, QList<int> apiIndex)
{
    m_boxHostAPI->clear();
    for(int i=0; i<name.size(); i++)
    {
        m_boxHostAPI->addItem(name.at(i), apiIndex.at(i));
    }
}

void OptionPanel::setInputDevices(QList<QString> name, QList<int> boxIndex)
{
    m_boxAudioInputDevice->clear();
    for(int i=0; i<name.size(); i++)
    {
        m_boxAudioInputDevice->addItem(name.at(i), boxIndex.at(i));
    }
}

void OptionPanel::setChannels(int numberOfChannels)
{
    m_boxInputChannel->clear();
    for(int i=0; i<numberOfChannels; i++)
    {
        m_boxInputChannel->addItem(QString::number(i+1));
    }
}

void OptionPanel::setBitDepths(QList<int> bitDepths)
{
    m_boxBitDepth->clear();
    for(int i=0; i<bitDepths.size(); i++)
    {
        m_boxBitDepth->addItem(QString::number(bitDepths.at(i)));
        if(bitDepths.at(i) == 16)
        {
            m_boxBitDepth->setCurrentIndex(i);
        }
    }
}

void OptionPanel::setSampleRates(const std::vector<uint32_t> & sampleRates)
{
    m_boxSampleRate->clear();
    for(size_t i=0; i<sampleRates.size(); i++)
    {
        m_boxSampleRate->addItem(QString::number(sampleRates.at(i)));
        if(sampleRates.at(i) == 44100)
        {
            m_boxSampleRate->setCurrentIndex(static_cast<int>(i));
        }
    }
}

void OptionPanel::disableStartButton(bool disable)
{
    m_buttonStart->setDisabled(disable);
}

void OptionPanel::disableStopButton(bool disable)
{
    m_buttonStop->setDisabled(disable);
}

void OptionPanel::disableUI(bool disable)
{
    m_boxAudioInputDevice->setDisabled(disable);
    m_boxBitDepth->setDisabled(disable);
    m_boxBlockSize->setDisabled(disable);
    m_boxHostAPI->setDisabled(disable);
    m_boxInputChannel->setDisabled(disable);
    m_boxSampleRate->setDisabled(disable);
    m_buttonStart->setDisabled(disable);
    m_buttonStop->setDisabled(!disable);
}

void OptionPanel::emitHostApiChanged(int index)
{
    emit signalHostApiChanged(m_boxHostAPI->itemData(index).toInt());
}

void OptionPanel::emitInputDeviceChanged(int index)
{
    emit signalInputDeviceChanged(m_boxAudioInputDevice->itemData(index).toInt());
}

void OptionPanel::emitInputChannelChanged(QString channel)
{
    emit signalInputChannelChanged(channel.toInt());
}

void OptionPanel::emitBitDepthChanged(QString bitDepth)
{
    emit signalBitDepthChanged(bitDepth.toInt());
}

void OptionPanel::emitSampleRateChanged(QString sampleRate)
{
    emit signalSampleRateChanged(sampleRate.toInt());
}

void OptionPanel::emitBlockSizeChanged(int blockSize)
{
    emit signalBlockSizeChanged(blockSize);
}

void OptionPanel::emitStartButtonPressed()
{
    emit signalStartButtonPressed();
}

void OptionPanel::emitStopButtonPressed()
{
    emit signalStopButtonPressed();
}

void OptionPanel::emitInfoButtonPressed()
{
    emit signalInfoButtonPressed();
}

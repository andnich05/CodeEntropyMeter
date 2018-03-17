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

#ifndef OPTIONPANEL_H
#define OPTIONPANEL_H

#include <QWidget>

class QVBoxLayout;
class QHBoxLayout;
class QFormLayout;
class QComboBox;
class QPushButton;
class QSpinBox;

class OptionPanel : public QWidget
{
    Q_OBJECT

public:
    OptionPanel(QWidget *parent = 0);

    void setHostApis(QList<QString> name, QList<int> apiIndex);
    void setInputDevices(QList<QString> name, QList<int> boxIndex);
    void setChannels(int numberOfChannels);
    void setBitDepths(QList<int> bitDepths);
    void setSampleRates(const std::vector<uint32_t> & sampleRates);

    // Disable or enable UI when stream is being opened or closed
    void disableUI(bool disable);
    void disableStartButton(bool disable);
    void disableStopButton(bool disable);

private:
    QVBoxLayout *m_mainLayout;
    QHBoxLayout *m_buttonLayout;
    QFormLayout *m_formLayout;

    QComboBox *m_boxAudioInputDevice;
    QComboBox *m_boxHostAPI;
    QComboBox *m_boxSampleRate;
    QComboBox *m_boxInputChannel;
    QComboBox *m_boxBitDepth;
    QSpinBox *m_boxBlockSize;
    QPushButton *m_buttonStart;
    QPushButton *m_buttonStop;
    QPushButton *m_buttonShowAsioPanel;
    QPushButton *m_buttonInfo;

protected:
    virtual void paintEvent(QPaintEvent *) override;

signals:
    void signalHostApiChanged(int apiId);
    void signalInputDeviceChanged(int deviceId);
    void signalInputChannelChanged(int channel);
    void signalBitDepthChanged(int bitDepth);
    void signalSampleRateChanged(int sampleRate);
    void signalBlockSizeChanged(int blockSize);
    void signalStartButtonPressed();
    void signalStopButtonPressed();
    void signalInfoButtonPressed();

private slots:
    void emitHostApiChanged(int index);
    void emitInputDeviceChanged(int index);
    void emitInputChannelChanged(QString channel);
    void emitBitDepthChanged(QString bitDepth);
    void emitSampleRateChanged(QString sampleRate);
    void emitBlockSizeChanged(int blockSize);
    void emitStartButtonPressed();
    void emitStopButtonPressed();
    void emitInfoButtonPressed();
};

#endif // OPTIONPANEL_H

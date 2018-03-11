/*
 * PeakMeter: Calculation of peak values
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

#ifndef PEAKMETER_H
#define PEAKMETER_H

#include <QWidget>

class PeakMeter : public QWidget {
    Q_OBJECT

public:
    PeakMeter(QWidget *parent = 0);
    // Set the time for meter return
    void setReturnTimeValue(double value);

private:

    double actualValue;
    double returnTimeValue;

    quint32 currentValue;
    quint32 referenceValue;
    quint32 maxValue;
    quint32 absoluteValue;

    // Get maximum value of all samples
    quint32 getMaximum(const QVector<qint32> & signalValues);
    // Convert to dB
    double calculatePeak(int currentValue, int referenceValue);
    // Pass the value to MeterDisplay
    void emitPeakValue(double peak);

    double maximumDynamicRange;

public slots:
    void updateMeter(const QVector<qint32> & signalValues);
    void updateBitdepth(int bitdepth);

signals:
    void signalUpdatePeakMeter(double value);
    void signalUpdatePeakHolder(double value);

};

#endif // PEAKMETER_H

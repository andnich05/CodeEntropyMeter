/*
 * Entropy: Calculation of entropy of audio samples
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

#ifndef ENTROPY_H
#define ENTROPY_H

#include <QVector>
#include <QObject>
#include <QMutex>

class Entropy : public QObject {
    Q_OBJECT

public:
    Entropy();
    virtual ~Entropy();

public slots:
    void countValues(QVector<qint32> signalValues);
    void calculateEntropy(int blockSize);
    // Set number of blocks to process
    void setNumberOfBlocks(int numberOfBlocks);
    // Set new numberOfSymbols if bitdepth has changed
    void setNumberOfSymbols(int bitdepth);
    // Clear arrays
    void clear();
    // Reset blockCounter if "Stop" has been pressed
    void reset();

signals:
    void finished(double entropy);

private:
    // Counter for positive-symbols
    quint32 *counterArrayPositive;
    // Counter for negatve-symbols
    quint32 *counterArrayNegative;

    double probabilityPositive;
    double probabilityNegative;
    quint32 numberOfSymbols;
    quint32 numberOfSamples;
    quint32 i;
    double entropy;
    int blockCounter;
    // Number of blocks to process
    int numberOfBlocks;
    int blockSize;
    QMutex mutex;
};

#endif // ENTROPY_H

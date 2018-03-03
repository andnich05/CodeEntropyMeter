/*
 * RingBuffer: Ring buffer fot storing callback samples
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

#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <QMutex>
#include <QVector>
#include <QWidget>

class RingBuffer : public QWidget {
    Q_OBJECT

public:
    RingBuffer(int capacity, QWidget *parent = 0);
    void clearAndResize(int capacity);
    // Insert sample at position
    void insertItem(qint32 item, quint32 position);

private:
    // Buffer to which is written by the callback function
    QVector<qint32> inBuffer;
    // Buffer to pass to further processing
    QVector<qint32> outBuffer;
    // Mutex to lock while copying from inBuffer to outBuffer
    QMutex mutex;

private slots:
    void emitBufferReadyToBeRead();

signals:
    void signalBufferReadyToBeRead(QVector<qint32> samples);

};

#endif // RINGBUFFER_H

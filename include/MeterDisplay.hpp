/*
 * MeterDisplay: Represantation of peak values, RMS and crest factor
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

#ifndef METERDISPLAY_H
#define METERDISPLAY_H

#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMouseEvent>

class QLabel;

// http://stackoverflow.com/questions/10591635/can-i-get-mouse-events-in-a-qgraphicsitem
// Custom view in order to get mouse clicks
class MeterView : public QGraphicsView {
    Q_OBJECT
protected:
    void mousePressEvent(QMouseEvent *event);
signals:
    void signalMouseEvent(QPoint pos);
};

class MeterDisplay : public QWidget {
    Q_OBJECT

public:
    MeterDisplay(QWidget *parent = 0);

private:
    QGraphicsScene scene;
    MeterView view;

    QGraphicsRectItem *rectPeakMeter;
    QGraphicsLineItem *rectPeakHolder;
    QGraphicsRectItem *rectPeakClip;
    QGraphicsRectItem *rectRmsMeter;
    QGraphicsLineItem *rectRmsHolder;

    void paintScale();
    void paintPeakMeterBackground();
    void paintRmsMeterBackground();

    QLabel *labelPeak;
    QLabel *labelRms;
    QLabel *labelCrest;
    QLabel *labelPeakValue;
    QLabel *labelRmsValue;
    QLabel *labelCrestValue;
    QLabel *labelMaxPeakValue;
    QLabel *labelMaxRmsValue;
    QLabel *labelMaxCrestValue;
    QLabel *labelCurrent;
    QLabel *labelMax;
    QLabel *labelDb;

    double actualValue;
    double returnTimeValue;

public slots:
    void updatePeakMeter(double peak);
    void updatePeakHolder(double peak);
    void updateRmsMeter(double rms);
    void updateRmsHolder(double rms);
    void updateCrestFactor(double crest);
    void updateMaxCrestFactor(double crest);
    // Reset the clip indicator by clicking on it
    void resetClip(QPoint pos);

protected:
    void paintEvent(QPaintEvent *);
};

#endif // METERDISPLAY_H

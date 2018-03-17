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

#include <QGraphicsView>

class QLabel;

// http://stackoverflow.com/questions/10591635/can-i-get-mouse-events-in-a-qgraphicsitem
// Custom view in order to get mouse clicks
class MeterView : public QGraphicsView
{
    Q_OBJECT
protected:
    virtual void mousePressEvent(QMouseEvent *event) override;
signals:
    void signalMouseEvent(QPoint pos);
};

class MeterDisplay : public QWidget
{
    Q_OBJECT

public:
    MeterDisplay(QWidget *parent = 0);

private:
    QGraphicsScene m_scene;
    MeterView m_view;

    QGraphicsRectItem *m_rectPeakMeter;
    QGraphicsLineItem *m_rectPeakHolder;
    QGraphicsRectItem *m_rectPeakClip;
    QGraphicsRectItem *m_rectRmsMeter;
    QGraphicsLineItem *m_rectRmsHolder;

    void paintScale();
    void paintPeakMeterBackground();
    void paintRmsMeterBackground();

    QLabel *m_labelPeak;
    QLabel *m_labelRms;
    QLabel *m_labelCrest;
    QLabel *m_labelPeakValue;
    QLabel *m_labelRmsValue;
    QLabel *m_labelCrestValue;
    QLabel *m_labelMaxPeakValue;
    QLabel *m_labelMaxRmsValue;
    QLabel *m_labelMaxCrestValue;
    QLabel *m_labelCurrent;
    QLabel *m_labelMax;
    QLabel *m_labelDb;

    double m_actualValue;
    double m_returnTimeValue;

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
    virtual void paintEvent(QPaintEvent *) override;
};

#endif // METERDISPLAY_H

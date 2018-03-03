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

#include "MeterDisplay.hpp"
#include <QLayout>
#include <QGraphicsRectItem>
#include <QStyleOption>
#include <QPainter>
#include <QLabel>
#include <QDebug>

const double INF = -999;

const QColor colorBackground(80,80,80);
const QColor colorEmptyMeter(50,50,50);
const QColor colorBorderBright(100,100,100);
const QColor colorBorderDark(30,30,30);
const QColor colorMeter(255,180,0);
const QColor colorFont(220,220,220);
const QColor colorHolder(0,220,0);
const QColor colorClip(220,0,0);

MeterDisplay::MeterDisplay(QWidget *parent)
    : QWidget(parent) {

    view.setScene(&scene);
    view.setAlignment(Qt::AlignTop | Qt::AlignLeft);
    view.setRenderHints(QPainter::Antialiasing);
    scene.setSceneRect(0,0,1,1);
    scene.setBackgroundBrush(QBrush(colorBackground));
    view.setStyleSheet("QGraphicsView { border-style: none }");

    //labelDb = new QLabel("dB");
    labelPeak = new QLabel("PEAK", this);
    labelRms = new QLabel("RMS", this);
    labelCrest = new QLabel("CREST", this);
    labelPeak->setAlignment(Qt::AlignHCenter);
    labelRms->setAlignment(Qt::AlignHCenter);
    labelCrest->setAlignment(Qt::AlignHCenter);

    labelMaxPeakValue = new QLabel("0.0", this);
    labelMaxRmsValue = new QLabel("0.0", this);
    labelMaxCrestValue = new QLabel("0.0", this);
    labelMaxPeakValue->setAlignment(Qt::AlignHCenter);
    labelMaxRmsValue->setAlignment(Qt::AlignHCenter);
    labelMaxCrestValue->setAlignment(Qt::AlignHCenter);

    labelPeakValue = new QLabel("0.0", this);
    labelRmsValue = new QLabel("0.0", this);
    labelCrestValue = new QLabel("0.0", this);
    labelPeakValue->setAlignment(Qt::AlignHCenter);
    labelRmsValue->setAlignment(Qt::AlignHCenter);
    labelCrestValue->setAlignment(Qt::AlignHCenter);

    QGridLayout *labelLayout = new QGridLayout();
    //labelLayout->addWidget(labelMax,1,0);
    //labelLayout->addWidget(labelCurrent,2,0);
    labelLayout->addWidget(labelPeak,0,1);
    labelLayout->addWidget(labelCrest,0,2);
    labelLayout->addWidget(labelRms,0,3);
    labelLayout->addWidget(labelMaxPeakValue,1,1);
    labelLayout->addWidget(labelMaxCrestValue,1,2);
    labelLayout->addWidget(labelMaxRmsValue,1,3);
    labelLayout->addWidget(labelPeakValue,2,1);
    labelLayout->addWidget(labelCrestValue,2,2);
    labelLayout->addWidget(labelRmsValue,2,3);
    labelLayout->setVerticalSpacing(1);
    labelLayout->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    labelLayout->setColumnMinimumWidth(0,4);
    labelLayout->setColumnMinimumWidth(1,40);
    labelLayout->setColumnMinimumWidth(2,40);
    labelLayout->setColumnMinimumWidth(3,40);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->addWidget(&view);
    mainLayout->addSpacing(4);
    mainLayout->addLayout(labelLayout);
    mainLayout->addSpacing(4);
    mainLayout->setSpacing(0);
    //mainLayout->addWidget(labelDb,0,Qt::AlignHCenter);
    mainLayout->addSpacing(5);

    // Paint the scale for the meters
    paintScale();

    paintPeakMeterBackground();
    paintRmsMeterBackground();

    this->setStyleSheet("color: " + colorFont.name()  + "; background-color: " + colorBackground.name() + ";");

    connect(&view, SIGNAL(signalMouseEvent(QPoint)), this, SLOT(resetClip(QPoint)));
}

void MeterDisplay::paintScale() {
    // By changing this variables you can move the graphical elements
    const int xShift = 45;
    const int yShift = 0;

    QPen pen(colorBorderDark);
    pen.setWidth(1);

    pen.setColor(colorFont);
    pen.setWidth(2);
    pen.setCapStyle(Qt::SquareCap);
    QPen pen2(colorFont);
    pen2.setWidth(1);
    pen2.setCapStyle(Qt::SquareCap);

    QGraphicsTextItem *clip = new QGraphicsTextItem("CLIP");
    clip->setDefaultTextColor(colorFont);
    clip->setPos(11+xShift,0+yShift);
    scene.addItem(clip);
    scene.addLine(1+xShift,11+yShift,8+xShift,11+yShift,pen);

    for(int i=0; i<13; i++) {
        scene.addLine(1+xShift,25+(i*36)+yShift,8+xShift,25+(i*36)+yShift,pen);
        scene.addLine(41+xShift,25+(i*36)+yShift,48+xShift,25+(i*36)+yShift,pen);
        QGraphicsTextItem *text = new QGraphicsTextItem(QString::number(-i*5));
        text->setDefaultTextColor(colorFont);
        if(i == 0) {
            text->setPos(18+xShift,14+i*36+yShift);
        }
        else if(i == 1) {
            text->setPos(15+xShift,14+i*36+yShift);
        }
        else {
            text->setPos(12+xShift,14+i*36+yShift);
        }
        scene.addItem(text);
        for(int j=1; j<5; j++) {
            if(i!=12) {
                scene.addLine(0+xShift,25+(i*36)+(j*7.2)+yShift,4+xShift,25+(i*36)+(j*7.2)+yShift,pen2);
                scene.addLine(43+xShift,25+(i*36)+(j*7.2)+yShift,47+xShift,25+(i*36)+(j*7.2)+yShift,pen2);
            }
        }
    }
}

void MeterDisplay::paintPeakMeterBackground() {
    const int xShift = 20;
    const int yShift = 0;

    QBrush brush(colorEmptyMeter);
    QPen pen(colorBorderDark);
    pen.setWidth(1);
    scene.addRect(2+xShift, 25+yShift, 15, 432, Qt::NoPen, QBrush(colorMeter));
    rectPeakMeter = new QGraphicsRectItem(2+xShift,25+yShift,15,432);
    rectPeakMeter->setBrush(brush);
    rectPeakMeter->setPen(Qt::NoPen);
    scene.addItem(rectPeakMeter);

    rectPeakClip = new QGraphicsRectItem(2+xShift, 6+yShift, 15, 10);
    rectPeakClip->setPen(Qt::NoPen);
    rectPeakClip->setBrush(brush);
    scene.addItem(rectPeakClip);

    brush.setColor(colorBorderDark);
    QPolygonF p;
    p << QPointF(0+xShift,23+yShift) << QPointF(19+xShift,23+yShift) << QPointF(17+xShift,25+yShift) << QPointF(2+xShift,25+yShift);
    QGraphicsPolygonItem *border1 = new QGraphicsPolygonItem(p);
    border1->setBrush(brush);
    border1->setPen(Qt::NoPen);
    scene.addItem(border1);

    p.clear();
    p << QPointF(0+xShift,23+yShift) << QPointF(0+xShift,434+25+yShift) << QPointF(2+xShift,432+25+yShift) << QPointF(2+xShift,25+yShift);
    QGraphicsPolygonItem *border2 = new QGraphicsPolygonItem(p);
    border2->setBrush(brush);
    border2->setPen(Qt::NoPen);
    scene.addItem(border2);

    brush.setColor(colorBorderBright);
    p.clear();
    p << QPointF(0+xShift,434+25+yShift) << QPointF(19+xShift,434+25+yShift) << QPointF(17+xShift,432+25+yShift) << QPointF(2+xShift,432+25+yShift);
    QGraphicsPolygonItem *border3 = new QGraphicsPolygonItem(p);
    border3->setBrush(brush);
    border3->setPen(Qt::NoPen);
    scene.addItem(border3);

    p.clear();
    p << QPointF(19+xShift,434+25+yShift) << QPointF(19+xShift,23+yShift) << QPointF(17+xShift,25+yShift) << QPointF(17+xShift,432+25+yShift);
    QGraphicsPolygonItem *border4 = new QGraphicsPolygonItem(p);
    border4->setBrush(brush);
    border4->setPen(Qt::NoPen);
    scene.addItem(border4);

    pen.setColor(colorHolder);
    pen.setWidth(2);
    pen.setCapStyle(Qt::SquareCap);
    rectPeakHolder = new QGraphicsLineItem(3+xShift,432+25+yShift,16+xShift,432+25+yShift);
    rectPeakHolder->setPen(pen);
    scene.addItem(rectPeakHolder);
}

void MeterDisplay::updatePeakMeter(double peak) {
    const int xShift = 20;
    const int yShift = 0;

    if(peak != INF) {
        if(peak >= -60) {
            rectPeakMeter->setRect(2+xShift,25+yShift,15,-((double)432/(double)60*peak));
        }
        else {
            rectPeakMeter->setRect(2+xShift,25+yShift,15,-((double)432/(double)60*(-60)));
        }

        if(peak >= 0) {
            rectPeakClip->setBrush(QBrush(colorClip));
            labelPeakValue->setText("CLIP");
        }
        else {
            labelPeakValue->setText(QString::number(peak,'f',1));
        }
        updateCrestFactor(peak-labelRmsValue->text().toDouble());
    }
    else {
        rectPeakMeter->setRect(2+xShift,25+yShift,15,-((double)432/(double)60*(-60)));
        labelPeakValue->setText("inf");
        updateCrestFactor(INF);
    }
}

void MeterDisplay::updatePeakHolder(double peak) {
    if(peak >= -60) {
        rectPeakHolder->setPos(0,(-1)*(double)432/(double)60*peak-432);
        if(peak < 0) {
            labelMaxPeakValue->setText(QString::number(peak,'f',1));
        }
        else {
            labelMaxPeakValue->setText("CLIP");
            updateCrestFactor(INF);
        }
        updateMaxCrestFactor(labelMaxPeakValue->text().toDouble()-labelMaxRmsValue->text().toDouble());
    }
}

void MeterDisplay::paintRmsMeterBackground() {
    const int xShift = 100;
    const int yShift = 0;

    QBrush brush(colorEmptyMeter);
    QPen pen(colorBorderDark);
    pen.setWidth(1);
    scene.addRect(2+xShift, 25+yShift, 15, 432, Qt::NoPen, QBrush(colorMeter));
    rectRmsMeter = new QGraphicsRectItem(2+xShift,25+yShift,15,432);
    rectRmsMeter->setBrush(brush);
    rectRmsMeter->setPen(Qt::NoPen);
    scene.addItem(rectRmsMeter);

    brush.setColor(colorBorderDark);
    QPolygonF p;
    p << QPointF(0+xShift,23+yShift) << QPointF(19+xShift,23+yShift) << QPointF(17+xShift,25+yShift) << QPointF(2+xShift,25+yShift);
    QGraphicsPolygonItem *border1 = new QGraphicsPolygonItem(p);
    border1->setBrush(brush);
    border1->setPen(Qt::NoPen);
    scene.addItem(border1);

    p.clear();
    p << QPointF(0+xShift,23+yShift) << QPointF(0+xShift,434+25+yShift) << QPointF(2+xShift,432+25+yShift) << QPointF(2+xShift,25+yShift);
    QGraphicsPolygonItem *border2 = new QGraphicsPolygonItem(p);
    border2->setBrush(brush);
    border2->setPen(Qt::NoPen);
    scene.addItem(border2);

    brush.setColor(colorBorderBright);
    p.clear();
    p << QPointF(0+xShift,434+25+yShift) << QPointF(19+xShift,434+25+yShift) << QPointF(17+xShift,432+25+yShift) << QPointF(2+xShift,432+25+yShift);
    QGraphicsPolygonItem *border3 = new QGraphicsPolygonItem(p);
    border3->setBrush(brush);
    border3->setPen(Qt::NoPen);
    scene.addItem(border3);

    p.clear();
    p << QPointF(19+xShift,434+25+yShift) << QPointF(19+xShift,23+yShift) << QPointF(17+xShift,25+yShift) << QPointF(17+xShift,432+25+yShift);
    QGraphicsPolygonItem *border4 = new QGraphicsPolygonItem(p);
    border4->setBrush(brush);
    border4->setPen(Qt::NoPen);
    scene.addItem(border4);

    pen.setColor(colorHolder);
    pen.setWidth(2);
    pen.setCapStyle(Qt::SquareCap);
    rectRmsHolder = new QGraphicsLineItem(3+xShift,432+25+yShift,16+xShift,432+25+yShift);
    rectRmsHolder->setPen(pen);
    scene.addItem(rectRmsHolder);
}

void MeterDisplay::updateRmsMeter(double rms) {
    const int xShift = 100;
    const int yShift = 0;

    if(rms != INF) {
        if(rms >= -60) {
            rectRmsMeter->setRect(2+xShift,25+yShift,15,-((double)432/(double)60*rms));
        }
        else {
            rectRmsMeter->setRect(2+xShift,25+yShift,15,-((double)432/(double)60*(-60)));
        }

        if(rms >= 0) {
            labelRmsValue->setText("CLIP");
        }
        else {
            labelRmsValue->setText(QString::number(rms,'f',1));
        }
        updateCrestFactor(labelPeakValue->text().toDouble()-rms);
    }
    else {
        rectRmsMeter->setRect(2+xShift,25+yShift,15,-((double)432/(double)60*(-60)));
        labelRmsValue->setText("inf");
        updateCrestFactor(INF);
    }
}

void MeterDisplay::updateRmsHolder(double rms) {
    if(rms >= -60) {
        rectRmsHolder->setPos(0,(-1)*(double)432/(double)60*rms-432);
        labelMaxRmsValue->setText(QString::number(rms,'f',1));
        updateMaxCrestFactor(labelMaxPeakValue->text().toDouble()-labelMaxRmsValue->text().toDouble());
    }
}

void MeterDisplay::resetClip(QPoint pos) {
    if(view.itemAt(pos) == rectPeakClip) {
        rectPeakClip->setBrush(QBrush(colorEmptyMeter));
    }
}

void MeterDisplay::updateCrestFactor(double crest) {
    if(crest >= 0.0) {
        labelCrestValue->setText(QString::number(crest,'f',1));
    }
    else if(crest == INF) {
        labelCrestValue->setText(" inf");
    }
}

void MeterDisplay::updateMaxCrestFactor(double crest) {
    if(crest >= 0.0) {
        labelMaxCrestValue->setText(QString::number(crest,'f',1));
    }
    else if(crest == INF) {
        labelMaxCrestValue->setText(" inf");
    }
}

void MeterDisplay::paintEvent(QPaintEvent *) {
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void MeterView::mousePressEvent(QMouseEvent *event) {
    emit signalMouseEvent(event->pos());
}

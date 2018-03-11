/*
 * BitDisplay: Representation of used bits
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
#include <QLayout>
#include <QGraphicsEllipseItem>
#include <QDebug>
#include <QLabel>
#include <QComboBox>
#include <QSpinBox>
#include <bitset>
#include <QGroupBox>
#include <QPushButton>
#include <QMouseEvent>
#include <QDebug>

const QColor colorBitSet(255,180,0);
const QColor colorBitNotSet(0,100,200);
const QColor colorBackground(80,80,80);
const QColor colorDarkerBackground(50,50,50);
const QColor colorBorderBright(70,70,70);
const QColor colorBorderDark(30,30,30);
const QColor colorFont(255,255,255);
const QColor colorFontDarker(220,220,220);
const QColor colorBitBorder(255,255,255);

const quint32 maxValue8bit = 255;
const quint32 maxValue16bit = 65535;
const quint32 maxValue24bit = 16777215;
const quint32 flipMsb8bit = 128;
const quint32 flipMsb16bit = 32768;
const quint32 flipMsb24bit = 8388608;

BitDisplay::BitDisplay(QWidget *parent)
    : QWidget(parent) {

    this->setStyleSheet("QLabel {color: " + colorFont.name() + "}");

    scene.setSceneRect(0,0,1,1);
    view.setRenderHints(QPainter::Antialiasing);
    view.setScene(&scene);
    view.setAlignment(Qt::AlignLeft);
    scene.setBackgroundBrush(QBrush(colorDarkerBackground));
    view.setStyleSheet("QGraphicsView { border-style: inset;"
                       "border-width: 2px;"
                       "border-top-color: " + colorBorderDark.name() + ";"
                       "border-left-color: " + colorBorderDark.name() + ";"
                       "border-right-color: " + colorBorderBright.name() + ";"
                       "border-bottom-color: " + colorBorderBright.name() + "; }");

    // Generate the circles which represent the bits
    for(int i=0; i<24; ++i) {
        QGraphicsEllipseItem *circle = new QGraphicsEllipseItem();
        circle->setRect(0,0,11,11);
        if(i < 8) {
            circle->setPos(315-i*13, 0);
        }
        else if(i < 16) {
            circle->setPos(310-i*13, 0);
        }
        else {
            circle->setPos(305-i*13, 0);
        }
        bitCircles.append(circle);
        scene.addItem(bitCircles[i]);
    }

    for(int i=0; i<24; i++) {
        setBits.append(false);
    }

    // UI elements
    tMsb = new QGraphicsTextItem("MSB");
    tMsb->setPos(0, -25);
    tMsb->setDefaultTextColor(colorFontDarker);
    scene.addItem(tMsb);

    tLsb = new QGraphicsTextItem("LSB");
    tLsb->setPos(308, -25);
    tLsb->setDefaultTextColor(colorFontDarker);
    scene.addItem(tLsb);

    comboBoxConversion = new QComboBox(this);
    comboBoxConversion->addItem("Original");
    comboBoxConversion->addItem("Absolute");
    comboBoxConversion->setCurrentIndex(1);
    labelSwitch = new QLabel(trUtf8("Conversion:"), this);
    labelDisplayMode = new QLabel(trUtf8("Display mode:"), this);
    comboBoxDisplayMode = new QComboBox(this);
    comboBoxDisplayMode->addItem("Block");
    comboBoxDisplayMode->addItem("Sample");
    buttonHold = new QPushButton("HOLD", this);
    buttonHold->setCheckable(true);
    buttonReset = new QPushButton("RESET", this);
    buttonReset->setEnabled(false);
    spinBoxSamplePosition = new QSpinBox(this);
    spinBoxSamplePosition->setValue(1);
    spinBoxSamplePosition->setMinimum(1);
    labelSamplePosition = new QLabel("Sample position:", this);
    spinBoxSamplePosition->setEnabled(false);

    QGridLayout *optionsLayout = new QGridLayout();
    optionsLayout->addWidget(labelSwitch,0,0);
    optionsLayout->addWidget(comboBoxConversion,0,1);
    optionsLayout->addWidget(buttonHold,0,2);
    optionsLayout->addWidget(buttonReset,0,3);
    optionsLayout->addWidget(labelDisplayMode,1,0);
    optionsLayout->addWidget(comboBoxDisplayMode,1,1);
    optionsLayout->addWidget(labelSamplePosition,1,2);
    optionsLayout->addWidget(spinBoxSamplePosition,1,3);

    QHBoxLayout *viewLayout = new QHBoxLayout();
    viewLayout->addWidget(&view);

    QVBoxLayout *mainVLayout = new QVBoxLayout();
    mainVLayout->addLayout(viewLayout);
    mainVLayout->addLayout(optionsLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addLayout(mainVLayout);

    brush.setStyle(Qt::SolidPattern);

    this->setLayout(mainLayout);
    setNumberOfBits(24);
    this->setHold(false);

    connect(this->buttonHold, SIGNAL(clicked(bool)), this, SLOT(setHold(bool)));
    connect(&view, SIGNAL(signalMouseEvent(QPoint)), this, SLOT(resetEllipseItem(QPoint)));
    connect(this->buttonReset, SIGNAL(clicked()), this, SLOT(resetAllItems()));
    connect(this->comboBoxDisplayMode, SIGNAL(currentIndexChanged(QString)), this, SLOT(enableSampleSpinBox(QString)));
}

void BitDisplay::updateDisplay(const QVector<qint32> & samples, int bitDepth) {

    // Shift the bits to the left if bitdepth is smaller than 24 bits
    int shift = 0;
    if(bitDepth == 8) {
        shift = 16;
    }
    else if(bitDepth == 16) {
        shift = 8;
    }

    // Reset all bits if "HOLD" isn't active
    if(holdBits == false) {
        brush.setColor(colorBackground);
        for(int i=0; i<bitDepth; i++) {
            setBits[i+shift] = false;
            bitCircles.at(i+shift)->setBrush(brush);
        }
    }

    // Display all bits which are set in a whole block
    if(comboBoxDisplayMode->currentText() == "Block") {
        brush.setColor(colorBitSet);
        bits.reset();
        for(int i=0; i<samples.size(); i++) {
            if(this->comboBoxConversion->currentText() == "Absolute") {
                bits = abs(samples.at(i));
            }
            else {
                bits = samples.at(i);
            }
            for(int j=0; j<bitDepth; j++) {
                if(bits[j] == true) {
                    if(setBits.at(j+shift) == false) {
                        bitCircles.at(j+shift)->setBrush(brush);
                        setBits[j+shift] = true;
                    }
                }
            }
        }
    }
    // Display only bits of the sample at "samplePosition" (useful when "Original")
    else {
        bits.reset();
        if(this->comboBoxConversion->currentText() == "Absolute") {
            bits = abs(samples.at(spinBoxSamplePosition->value()-1));
        }
        else {
            bits = samples.at(spinBoxSamplePosition->value()-1);
        }
        for(int j=0; j<bitDepth; j++) {
            if(bits[j] == true) {
                brush.setColor(colorBitSet);
                bitCircles.at(j+shift)->setBrush(brush);
            }
            else {
                if(holdBits == false) {
                    brush.setColor(colorBackground);
                    bitCircles.at(j+shift)->setBrush(brush);
                }
            }
        }
    }
}

void BitDisplay::setNumberOfBits(int numberOfBits) {
    // Move the bits to the right position
    int shift = 0;
    if(numberOfBits == 8) {
        shift = 16;
        tLsb->setPos(92,-25);
    }
    else if(numberOfBits == 16) {
        shift = 8;
        tLsb->setPos(200,-25);
    }
    else if(numberOfBits == 24) {
        shift = 0;
        tLsb->setPos(308,-25);
    }

    // Set inital colors
    for(int i=0; i<bitCircles.size(); i++) {
        if(i >= shift) {
            brush.setColor(colorBackground);
            bitCircles.at(i)->setPen(Qt::NoPen);
            bitCircles.at(i)->setBrush(brush);
        }
        else {
            brush.setColor(colorDarkerBackground);
            bitCircles.at(i)->setPen(Qt::NoPen);
            bitCircles.at(i)->setBrush(brush);
        }
    }
}

void BitDisplay::setHold(bool hold) {
    this->holdBits = hold;
    buttonReset->setEnabled(hold);
}

void BitDisplay::resetEllipseItem(QPoint pos) {
    brush.setColor(colorBackground);
    for(int i=0; i<bitCircles.size(); i++) {
        if(bitCircles[i] == view.itemAt(pos)) {
            if(holdBits == true) {
                // Check if bit is active
                if(bitCircles.at(i)->brush().color() != colorDarkerBackground) {
                    bitCircles.at(i)->setBrush(brush);
                    setBits[i] = false;
                }
            }
        }
    }
}

void BitDisplay::resetAllItems() {
    brush.setColor(colorBackground);
    for(int i=0; i<bitCircles.size(); i++) {
        if(bitCircles.at(i)->brush().color() != colorDarkerBackground) {
            bitCircles.at(i)->setBrush(brush);
            setBits[i] = false;
        }
    }
}

void BitDisplay::enableSampleSpinBox(QString mode) {
    if(mode == "Sample") {
        spinBoxSamplePosition->setEnabled(true);
    }
    else {
        spinBoxSamplePosition->setEnabled(false);
    }
}

void BitDisplay::setSampleMaximum(int max) {
    spinBoxSamplePosition->setMaximum(max);
}

void BitDisplay::paintEvent(QPaintEvent *) {
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void BitView::mousePressEvent(QMouseEvent *event) {
    emit signalMouseEvent(event->pos());
}

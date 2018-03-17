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
#include <QLabel>
#include <QComboBox>
#include <QSpinBox>
#include <QPushButton>
#include <QMouseEvent>

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
    : QWidget(parent)
    , m_holdBits(false)
{

    setStyleSheet("QLabel {color: " + colorFont.name() + "}");

    m_scene.setSceneRect(0,0,1,1);
    m_view.setRenderHints(QPainter::Antialiasing);
    m_view.setScene(&m_scene);
    m_view.setAlignment(Qt::AlignLeft);
    m_scene.setBackgroundBrush(QBrush(colorDarkerBackground));
    m_view.setStyleSheet("QGraphicsView { border-style: inset;"
                       "border-width: 2px;"
                       "border-top-color: " + colorBorderDark.name() + ";"
                       "border-left-color: " + colorBorderDark.name() + ";"
                       "border-right-color: " + colorBorderBright.name() + ";"
                       "border-bottom-color: " + colorBorderBright.name() + "; }");

    // Generate the circles which represent the bits
    for(int i=0; i<24; ++i)
    {
        QGraphicsEllipseItem *circle = new QGraphicsEllipseItem();
        circle->setRect(0,0,11,11);
        if(i < 8)
        {
            circle->setPos(315-i*13, 0);
        }
        else if(i < 16)
        {
            circle->setPos(310-i*13, 0);
        }
        else
        {
            circle->setPos(305-i*13, 0);
        }
        m_bitCircles.append(circle);
        m_scene.addItem(m_bitCircles[i]);
    }

    for(int i=0; i<24; i++)
    {
        m_setBits.push_back(false);
    }

    // UI elements
    m_tMsb = new QGraphicsTextItem("MSB");
    m_tMsb->setPos(0, -25);
    m_tMsb->setDefaultTextColor(colorFontDarker);
    m_scene.addItem(m_tMsb);

    m_tLsb = new QGraphicsTextItem("LSB");
    m_tLsb->setPos(308, -25);
    m_tLsb->setDefaultTextColor(colorFontDarker);
    m_scene.addItem(m_tLsb);

    m_comboBoxConversion = new QComboBox(this);
    m_comboBoxConversion->addItem("Original");
    m_comboBoxConversion->addItem("Absolute");
    m_comboBoxConversion->setCurrentIndex(1);
    m_labelSwitch = new QLabel(trUtf8("Conversion:"), this);
    m_labelDisplayMode = new QLabel(trUtf8("Display mode:"), this);
    m_comboBoxDisplayMode = new QComboBox(this);
    m_comboBoxDisplayMode->addItem("Block");
    m_comboBoxDisplayMode->addItem("Sample");
    m_buttonHold = new QPushButton("HOLD", this);
    m_buttonHold->setCheckable(true);
    m_buttonReset = new QPushButton("RESET", this);
    m_buttonReset->setEnabled(false);
    m_spinBoxSamplePosition = new QSpinBox(this);
    m_spinBoxSamplePosition->setValue(1);
    m_spinBoxSamplePosition->setMinimum(1);
    m_labelSamplePosition = new QLabel("Sample position:", this);
    m_spinBoxSamplePosition->setEnabled(false);

    QGridLayout *optionsLayout = new QGridLayout();
    optionsLayout->addWidget(m_labelSwitch,0,0);
    optionsLayout->addWidget(m_comboBoxConversion,0,1);
    optionsLayout->addWidget(m_buttonHold,0,2);
    optionsLayout->addWidget(m_buttonReset,0,3);
    optionsLayout->addWidget(m_labelDisplayMode,1,0);
    optionsLayout->addWidget(m_comboBoxDisplayMode,1,1);
    optionsLayout->addWidget(m_labelSamplePosition,1,2);
    optionsLayout->addWidget(m_spinBoxSamplePosition,1,3);

    QHBoxLayout *viewLayout = new QHBoxLayout();
    viewLayout->addWidget(&m_view);

    QVBoxLayout *mainVLayout = new QVBoxLayout();
    mainVLayout->addLayout(viewLayout);
    mainVLayout->addLayout(optionsLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addLayout(mainVLayout);

    brush.setStyle(Qt::SolidPattern);

    setLayout(mainLayout);
    setNumberOfBits(24);
    setHold(false);

    connect(m_buttonHold, SIGNAL(clicked(bool)), this, SLOT(setHold(bool)));
    connect(&m_view, SIGNAL(signalMouseEvent(QPoint)), this, SLOT(resetEllipseItem(QPoint)));
    connect(m_buttonReset, SIGNAL(clicked()), this, SLOT(resetAllItems()));
    connect(m_comboBoxDisplayMode, SIGNAL(currentIndexChanged(QString)), this, SLOT(enableSampleSpinBox(QString)));
}

void BitDisplay::updateDisplay(const std::vector<int32_t> & samples, int bitDepth)
{

    // Shift the bits to the left if bitdepth is smaller than 24 bits
    int shift = 0;
    if(bitDepth == 8)
    {
        shift = 16;
    }
    else if(bitDepth == 16)
    {
        shift = 8;
    }

    // Reset all bits if "HOLD" isn't active
    if(!m_holdBits)
    {
        brush.setColor(colorBackground);
        for(int i=0; i<bitDepth; i++)
        {
            m_setBits[i+shift] = false;
            m_bitCircles[i+shift]->setBrush(brush);
        }
    }

    // Display all bits which are set in a whole block
    if(m_comboBoxDisplayMode->currentText() == "Block")
    {
        brush.setColor(colorBitSet);
        m_bits.reset();
        for(size_t i=0; i<samples.size(); i++)
        {
            if(m_comboBoxConversion->currentText() == "Absolute")
            {
                m_bits = std::abs(samples.at(i));
            }
            else
            {
                m_bits = samples.at(i);
            }
            for(int j=0; j<bitDepth; j++)
            {
                if(m_bits[j] == true)
                {
                    if(m_setBits.at(j+shift) == false)
                    {
                        m_bitCircles.at(j+shift)->setBrush(brush);
                        m_setBits[j+shift] = true;
                    }
                }
            }
        }
    }
    // Display only bits of the sample at "samplePosition" (useful when "Original")
    else
    {
        m_bits.reset();
        if(m_comboBoxConversion->currentText() == "Absolute")
        {
            m_bits = std::abs(samples.at(m_spinBoxSamplePosition->value()-1));
        }
        else
        {
            m_bits = samples.at(m_spinBoxSamplePosition->value()-1);
        }
        for(int j=0; j<bitDepth; j++)
        {
            if(m_bits[j] == true)
            {
                brush.setColor(colorBitSet);
                m_bitCircles.at(j+shift)->setBrush(brush);
            }
            else
            {
                if(m_holdBits == false)
                {
                    brush.setColor(colorBackground);
                    m_bitCircles.at(j+shift)->setBrush(brush);
                }
            }
        }
    }
}

void BitDisplay::setNumberOfBits(int numberOfBits)
{
    // Move the bits to the right position
    int shift = 0;
    if(numberOfBits == 8)
    {
        shift = 16;
        m_tLsb->setPos(92,-25);
    }
    else if(numberOfBits == 16)
    {
        shift = 8;
        m_tLsb->setPos(200,-25);
    }
    else if(numberOfBits == 24)
    {
        shift = 0;
        m_tLsb->setPos(308,-25);
    }

    // Set inital colors
    for(int i=0; i<m_bitCircles.size(); i++)
    {
        if(i >= shift)
        {
            brush.setColor(colorBackground);
            m_bitCircles.at(i)->setPen(Qt::NoPen);
            m_bitCircles.at(i)->setBrush(brush);
        }
        else
        {
            brush.setColor(colorDarkerBackground);
            m_bitCircles.at(i)->setPen(Qt::NoPen);
            m_bitCircles.at(i)->setBrush(brush);
        }
    }
}

void BitDisplay::setHold(bool hold)
{
    m_holdBits = hold;
    m_buttonReset->setEnabled(hold);
}

void BitDisplay::resetEllipseItem(QPoint pos)
{
    brush.setColor(colorBackground);
    for(int i=0; i<m_bitCircles.size(); i++)
    {
        if(m_bitCircles[i] == m_view.itemAt(pos))
        {
            if(m_holdBits == true)
            {
                // Check if bit is active
                if(m_bitCircles.at(i)->brush().color() != colorDarkerBackground)
                {
                    m_bitCircles.at(i)->setBrush(brush);
                    m_setBits[i] = false;
                }
            }
        }
    }
}

void BitDisplay::resetAllItems()
{
    brush.setColor(colorBackground);
    for(int i=0; i<m_bitCircles.size(); i++)
    {
        if(m_bitCircles.at(i)->brush().color() != colorDarkerBackground)
        {
            m_bitCircles.at(i)->setBrush(brush);
            m_setBits[i] = false;
        }
    }
}

void BitDisplay::enableSampleSpinBox(QString mode)
{
    if(mode == "Sample")
    {
        m_spinBoxSamplePosition->setEnabled(true);
    }
    else
    {
        m_spinBoxSamplePosition->setEnabled(false);
    }
}

void BitDisplay::setSampleMaximum(int max)
{
    m_spinBoxSamplePosition->setMaximum(max);
}

void BitDisplay::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void BitView::mousePressEvent(QMouseEvent *event)
{
    emit signalMouseEvent(event->pos());
}

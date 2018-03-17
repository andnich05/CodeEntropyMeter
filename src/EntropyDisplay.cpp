/*
 * EntropyDisplay: Representation of calculated entropy
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

#include "EntropyDisplay.hpp"

#include <QLabel>
#include <QSpinBox>
#include <QLayout>
#include <QStyleOption>
#include <QPainter>

const QColor colorFont(255,255,255);

EntropyDisplay::EntropyDisplay(QWidget *parent)
    : QWidget(parent)
{
    m_labelEntropy = new QLabel(trUtf8("Entropy: 0.00000 bit"), this);
    m_labelEntropy->setFont(QFont("Sans", 20, QFont::Bold));
    m_labelNumberOfBlocks = new QLabel(trUtf8("Number of blocks to process: "), this);
    m_boxNumberOfBlocks = new QSpinBox(this);
    m_boxNumberOfBlocks->setMinimum(1);
    m_boxNumberOfBlocks->setMaximum(10000);
    m_boxNumberOfBlocks->setValue(50);
    m_labelIntegrationTime = new QLabel(trUtf8("[Corresponds to 0 ms integration time]"), this);

    setStyleSheet("QLabel { color: " + colorFont.name() + "}");

    QHBoxLayout *mainHLayout = new QHBoxLayout();
    mainHLayout->addWidget(m_labelNumberOfBlocks);
    mainHLayout->addWidget(m_boxNumberOfBlocks);
    mainHLayout->setAlignment(Qt::AlignLeft);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_labelEntropy);
    mainLayout->addLayout(mainHLayout);
    mainLayout->addWidget(m_labelIntegrationTime);

    connect(m_boxNumberOfBlocks, SIGNAL(valueChanged(int)), this, SLOT(emitNumberOfBlocksChanged(int)));
}

void EntropyDisplay::updateEntropy(double entropy)
{
    m_labelEntropy->setText("Entropy: " + QString::number(entropy,'f',5) + " bit");
}

void EntropyDisplay::updateIntegrationTimeLabel(double blockDuration)
{
    m_labelIntegrationTime->setText("[Corresponds to " + QString::number(blockDuration*m_boxNumberOfBlocks->value(),'f',2) + " ms integration time]");
}

void EntropyDisplay::emitNumberOfBlocksChanged(int value)
{
    emit signalNumberOfBlocksChanged(value);
}

quint32 EntropyDisplay::getNumberOfBlocks()
{
    return m_boxNumberOfBlocks->value();
}

void EntropyDisplay::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void EntropyDisplay::disableUI(bool disable)
{
    m_boxNumberOfBlocks->setDisabled(disable);
}

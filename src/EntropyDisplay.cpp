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
#include <QDebug>

const QColor colorFont(255,255,255);

EntropyDisplay::EntropyDisplay(QWidget *parent)
    : QWidget(parent) {

    labelEntropy = new QLabel(trUtf8("Entropy: 0.00000 bit"), this);
    labelEntropy->setFont(QFont("Serif", 20, QFont::Bold));
    labelNumberOfBlocks = new QLabel(trUtf8("Number of blocks to process: "), this);
    boxNumberOfBlocks = new QSpinBox(this);
    boxNumberOfBlocks->setMinimum(1);
    boxNumberOfBlocks->setMaximum(10000);
    boxNumberOfBlocks->setValue(50);
    labelIntegrationTime = new QLabel(trUtf8("[Corresponds to 0 ms integration time]"), this);

    this->setStyleSheet("QLabel { color: " + colorFont.name() + "}");

    QHBoxLayout *mainHLayout = new QHBoxLayout();
    mainHLayout->addWidget(labelNumberOfBlocks);
    mainHLayout->addWidget(boxNumberOfBlocks);
    mainHLayout->setAlignment(Qt::AlignLeft);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(labelEntropy);
    mainLayout->addLayout(mainHLayout);
    mainLayout->addWidget(labelIntegrationTime);

    connect(this->boxNumberOfBlocks, SIGNAL(valueChanged(int)), this, SLOT(emitNumberOfBlocksChanged(int)));
}

void EntropyDisplay::updateEntropy(double entropy) {
    labelEntropy->setText("Entropy: " + QString::number(entropy,'f',5) + " bit");
}

void EntropyDisplay::updateIntegrationTimeLabel(double blockDuration) {
    labelIntegrationTime->setText("[Corresponds to " + QString::number(blockDuration*boxNumberOfBlocks->value(),'f',2) + " ms integration time]");
}

void EntropyDisplay::emitNumberOfBlocksChanged(int value) {
    emit signalNumberOfBlocksChanged(value);
}

quint32 EntropyDisplay::getNumberOfBlocks() {
    return boxNumberOfBlocks->value();
}

void EntropyDisplay::paintEvent(QPaintEvent *) {
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void EntropyDisplay::disableUI(bool disable) {
    boxNumberOfBlocks->setDisabled(disable);
}

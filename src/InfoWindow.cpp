/*
 * InfoWindow: Display a window with version and copyright information
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

#include "InfoWindow.hpp"

#include <QLabel>
#include <QPushButton>
#include <QLayout>
#include <QStyleOption>
#include <QPainter>

const QColor colorFont(255,255,255);

InfoWindow::InfoWindow(QWidget *parent)
    : QWidget(parent)
{
    m_labelVersion = new QLabel(trUtf8("Code Entropy Meter 1.00"), this);
    m_labelCopyright = new QLabel(trUtf8("Copyright (C) 2014 by Andrej Nichelmann and Klaus Michael Indlekofer \n\n"
                                       "This program is free software: you can redistribute it and/or modify "
                                       "it under the terms of the GNU General Public License as published by "
                                       "the Free Software Foundation, either version 3 of the License, or "
                                       "(at your option) any later version.\n\n"

                                       "This program is distributed in the hope that it will be useful, "
                                       "but WITHOUT ANY WARRANTY; without even the implied warranty of "
                                       "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the "
                                       "GNU General Public License for more details.\n\n"

                                       "You should have received a copy of the GNU General Public License "
                                       "along with this program.  If not, see <http://www.gnu.org/licenses/>."), this);
    m_labelCopyright->setWordWrap(true);
    m_buttonClose = new QPushButton(trUtf8("Close"), this);

    QVBoxLayout *mainVLayout = new QVBoxLayout(this);
    mainVLayout->addWidget(m_labelVersion);
    mainVLayout->addWidget(m_labelCopyright);
    mainVLayout->addWidget(m_buttonClose);

    connect(m_buttonClose, SIGNAL(clicked()), this, SLOT(hide()));
}

void InfoWindow::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

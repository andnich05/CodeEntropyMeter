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

#ifndef INFOWINDOW_H
#define INFOWINDOW_H

#include <QWidget>

class QLabel;
class QPushButton;

class InfoWindow : public QWidget
{
    Q_OBJECT

public:
    InfoWindow(QWidget *parent = 0);

private:
    QLabel *m_labelVersion;
    QLabel *m_labelCopyright;
    QPushButton *m_buttonClose;

signals:
    void signalConversionChanged(bool showOriginal);

protected:
    // Enable background-color painting of this widget
    virtual void paintEvent(QPaintEvent *) override;
};

#endif // INFOWINDOW_H

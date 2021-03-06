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

#ifndef BITDISPLAY_H
#define BITDISPLAY_H

#include <QGraphicsView>

#include <bitset>

class QLabel;
class QSpinBox;
class QComboBox;
class QPushButton;

// http://stackoverflow.com/questions/10591635/can-i-get-mouse-events-in-a-qgraphicsitem
// Custom view in order to get mouse clicks
class BitView : public QGraphicsView
{
    Q_OBJECT

signals:
    void signalMouseEvent(QPoint pos);

protected:
    // Get the position of the mouse click
    virtual void mousePressEvent(QMouseEvent *event) override;
};


class BitDisplay : public QWidget
{
    Q_OBJECT

public:
    BitDisplay(QWidget *parent = 0);

public:
    void updateDisplay(const std::vector< int32_t > & samples, int bitDepth);
    // Sets initial number of bits to display
    void setNumberOfBits(int numberOfBits);
    // Set sample position maximum when another block size has been selected
    void setSampleMaximum(int max);

signals:
    void signalConversionChanged(bool showOriginal);

protected:
    // Enable background-color painting of this widget
    virtual void paintEvent(QPaintEvent *) override;

private slots:
    // Connected to QPushButton "HOLD" signal
    void setHold(bool hold);
    // Reset one bit by clicking on it
    void resetEllipseItem(QPoint pos);
    // Reset all bits
    void resetAllItems();
    void enableSampleSpinBox(QString mode);

private:
    // Painting area
    QGraphicsScene m_scene;
    BitView m_view;
    // The bits
    QList<QGraphicsEllipseItem*> m_bitCircles;
    // Labels MSB and LSB
    QGraphicsTextItem *m_tMsb;
    QGraphicsTextItem *m_tLsb;

    // UI
    QComboBox *m_comboBoxConversion;
    QComboBox *m_comboBoxDisplayMode;
    QLabel *m_labelSwitch;
    QLabel *m_labelDisplayMode;
    QPushButton *m_buttonHold;
    QPushButton *m_buttonReset;
    QLabel *m_labelSamplePosition;
    QSpinBox *m_spinBoxSamplePosition;

    // Is set when "HOLD" button is pressed
    bool m_holdBits;
    // bit array
    std::bitset<24> m_bits;
    // Indicates whether a bit has been set in the actual block already
    std::vector<bool> m_setBits;

    QBrush brush;
    QPen pen;
};

#endif // BITDISPLAY_H

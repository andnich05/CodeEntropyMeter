/*
 * EntropyDisplay: Represantation of calculated entropy
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

#ifndef ENTROPYDISPLAY_H
#define ENTROPYDISPLAY_H

#include <QWidget>

class QLabel;
class QSpinBox;

class EntropyDisplay : public QWidget
{
    Q_OBJECT

public:
    EntropyDisplay(QWidget *parent = 0);

private:
    QLabel *m_labelEntropy;
    QLabel *m_labelNumberOfBlocks;
    QSpinBox *m_boxNumberOfBlocks;
    QLabel *m_labelIntegrationTime;

signals:
    void signalNumberOfBlocksChanged(int value);

public slots:
    void updateEntropy(double entropy);
    void updateIntegrationTimeLabel(double blockDuration);
    void emitNumberOfBlocksChanged(int value);
    quint32 getNumberOfBlocks();
    void disableUI(bool disable);

protected:
    virtual void paintEvent(QPaintEvent *) override;
};

#endif // ENTROPYDISPLAY_H

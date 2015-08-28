/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtSerialBus module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "libmodbusslave.h"
#include "libmodbusmaster.h"

#include <QtSerialBus/qmodbus.h>
#include <QtSerialBus/qmodbusfactory.h>

#include <QtSerialPort>

#include <QtCore/qfile.h>
#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

class ModBusPlugin : public QObject, public QModBusFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QModBusFactory" FILE "plugin.json")
    Q_INTERFACES(QModBusFactory)


public:
    QModBusSlave *createSlave(QSerialPort *transport) const
    {
        QModBusSlave *device = new LibModBusSlave(transport);
        return device;
    }

    QModBusMaster *createMaster(QSerialPort *transport) const
    {
        QModBusMaster *device = new LibModBusMaster(transport);
        return device;
    }
};

QT_END_NAMESPACE

#include "main.moc"
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

#include "qmodbuspdu.h"

#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

/*!
    \class QModbusPdu
    \inmodule QtSerialBus
    \since 5.6

    \brief QModbusPdu is a abstract container class containing the function code and
        payload that is stored inside a Modbus ADU.
 */

/*!
    \enum QModbusPdu::ExceptionCode

    This enum describes all the possible error conditions as defined by Modbus Exception Codes.
    They are set by the server after checking the appropriate error conditions in the reply to a
    request and must be decoded by the client to operate on the exception code.

    \value IllegalFunction                      Function code is not supported by device.
    \value IllegalDataAddress                   The received data address in the query is not an
                                                allowable address for the Modbus server.
    \value IllegalDataValue                     The contained value in the request data field is
                                                not an allowable value for the Modbus server.
    \value ServerDeviceFailure                  An irrecoverable error occurred while the server
                                                was attempting to perform the requested action.
    \value Acknowledge                          Specialized use in conjunction with programming
                                                commands.
    \value ServerDeviceBusy                     The slave is engaged in processing a long�duration
                                                program command.
    \value MemoryParityError                    Indicates that the extended file area failed to
                                                pass a consistency check. Used in conjunction with
                                                function codes 20 and 21. The exception code does
                                                not refer to any parity settings of the
                                                transmission line but only to the servers' internal
                                                memory of file records.
    \value GatewayPathUnavailable               Indicates that the gateway was unable to allocate
                                                an internal communication path from the input port
                                                to the output port for processing the request.
    \value GatewayTargetDeviceFailedToRespond   Indicates that no response was obtained from the
                                                target device behind a gateway.
                                                Usually this means the target device is not online
                                                on the network.
*/

/*!
    \fn QModbusPdu::QModbusPdu()

    Constructs an invalid QModbusPdu.
*/

/*!
    \internal
    \fn QModbusPdu::QModbusPdu(FunctionCode code, const QByteArray &data)

    Constructs a QModbusPdu with function code set to \a code and payload set to \a data.
    The data is expected to be stored in big-endian byte order already.
*/

/*!
    \internal
    \fn QModbusPdu::QModbusPdu(FunctionCode code, Args... data)

    Constructs a QModbusPdu with function code set to \a code and payload set to \a data.
    The data is converted and stored in big-endian byte order.

    \note Usage should be limited the POD types only. This is because \c QDataStream stream
        operators will not only append raw data, but also e.g. size, count etc. for complex types.
*/

/*!
    \fn bool QModbusPdu::isValid() const

    Returns true if the PDU is valid; otherwise false.

    A PDU is considered valid if the message code is in the range of 1 to 255 decimal and the
    PDU's compound size (function code + data) does not exceed 253 bytes. A default constructed
    PDU is invalid.
*/

/*!
    \fn FunctionCode QModbusPdu::functionCode() const

    Returns the PDU's function code.
*/

/*!
    void QModbusPdu::setFunctionCode(FunctionCode code)

    Sets the PDU's function code to \a code.
*/

/*!
    QByteArray QModbusPdu::data() const

    Returns the PDU's payload. The payload is stored in big-endian byte order.
*/

/*!
    void QModbusPdu::setData(const QByteArray &data)

    Sets the PDU's function payload to \a data. The data is expected to be stored in big-endian
    byte order already.
*/

/*!
    \fn void QModbusPdu::decodeData(Args *... data)

    Converts the payload into host endianness and reads it into \a data. Data can be a variable
    length argument list.

    \code
        QModbusResponsePdu response(QModbusPdu::ReportServerId);
        response.encodeData(quint8(0x02), quint8(0x01), quint8(0xff));
        quint8 count, id, run;
        response.decodeData(&count, &id, &run);
    \endcode

    \note Usage should be limited the POD types only. This is because \c QDataStream stream
        operators will not only read raw data, but also e.g. size, count etc. for complex types.
*/

/*!
    \fn void QModbusPdu::encodeData(Args... data)

    Sets the payload to \a data. The data is converted and stored in big-endian byte order.

    \code
        QModbusRequestPdu request(QModbusPdu::ReadCoils);
        // starting address and quantity of coils
        request.encodeData(quint16(0x0c), quint16(0x0a));
    \endcode

    \note Usage should be limited the POD types only. This is because \c QDataStream stream
        operators will not only append raw data, but also e.g. size, count etc. for complex types.
*/

/*!
    \relates QModbusPdu

    Writes a \a pdu to the stream \a out and returns a reference to the stream.
*/
QDataStream &operator<<(QDataStream &stream, const QModbusPdu &pdu)
{
    stream << static_cast<quint8> (pdu.functionCode());
    if (!pdu.data().isEmpty())
        stream.writeRawData(pdu.data().constData(), pdu.data().size());

    return stream;
}

/*!
    \relates QModbusPdu

    Reads a \a pdu from the stream \a in and returns a reference to the stream.
*/
QDataStream &operator>>(QDataStream &stream, QModbusPdu &pdu)
{
    quint8 code;
    stream >> code;
    pdu.setFunctionCode(static_cast<QModbusPdu::FunctionCode> (code));

    QByteArray data(252, Qt::Uninitialized);
    stream.readRawData(data.data(), 252);
    pdu.setData(data);

    return stream;
}

/*!
    Writes the Modbus \a pdu to the \a debug stream.
*/
QDebug operator<<(QDebug debug, const QModbusPdu &pdu)
{
    QDebugStateSaver _(debug);
    debug.nospace().noquote() << "0x" << QByteArray(1, pdu.functionCode()).toHex() << pdu.data()
        .toHex();
    return debug;
}

/*!
    \class QModbusRequest
    \inmodule QtSerialBus
    \since 5.6

    \brief QModbusRequest is a container class containing the function code and payload that is
        stored inside a Modbus ADU.

    A Modbus request usually consists of a single byte describing the \c FunctionCode and N bytes
    of payload

    A typical Modbus request can looks like this:
    \code
        QModbusRequest request(QModbusRequest::WriteMultipleCoils,
            QByteArray::fromHex("0013000a02cd01"));
    \endcode
    \note When using the constructor taking the \c QByteArray, please make sure to convert the
        containing data to big-endian byte order before creating the request.

    The same request can be created like this, if the values are know at compile time:
    \code
        quint16 startAddress = 19, numberOfCoils = 10;
        quint8 payloadInBytes = 2, outputHigh = 0xcd, outputLow = 0x01;
        QModbusRequest request(QModbusRequest::WriteMultipleCoils, startAddress, numberOfCoils,
            payloadInBytes, outputHigh, outputLow);
    \endcode
*/

/*!
    \fn QModbusRequest::QModbusRequest()

    Constructs an invalid QModbusRequest.
*/

/*!
    \fn QModbusRequest::QModbusRequest(FunctionCode code, Args... data)

    Constructs a QModbusRequest with function code set to \a code and payload set to \a data.
    The data is converted and stored in big-endian byte order.

    \note Usage should be limited the POD types only. This is because \c QDataStream stream
        operators will not only append raw data, but also e.g. size, count etc. for complex types.
*/

/*!
    \fn QModbusRequest::QModbusRequest(FunctionCode code, const QByteArray &data = QByteArray)

    Constructs a QModbusResponse with function code set to \a code and payload set to \a data.
    The data is expected to be stored in big-endian byte order already.
*/

/*!
    \class QModbusResponse
    \inmodule QtSerialBus
    \since 5.6

    \brief QModbusResponse is a container class containing the function code and payload that is
        stored inside a Modbus ADU.

    A typical Modbus response can looks like this:
    \code
        QModbusResponse response(QModbusResponse::ReadCoils, QByteArray::fromHex("02cd01"));
    \endcode
    \note When using the constructor taking the \c QByteArray, please make sure to convert the
        containing data to big-endian byte order before creating the request.

    The same response can be created like this, if the values are know at compile time:
    \code
        quint8 payloadInBytes = 2, outputHigh = 0xcd, outputLow = 0x01;
        QModbusResponse response(QModbusResponse::ReadCoils, payloadInBytes, outputHigh, outputLow);
    \endcode
*/


/*!
    \fn QModbusResponse::QModbusResponse()

    Constructs an invalid QModbusResponse.
*/

/*!
    \fn QModbusResponse::QModbusResponse(FunctionCode code, Args... data)

    Constructs a QModbusResponse with function code set to \a code and payload set to \a data.
    The data is converted and stored in big-endian byte order.

    \note Usage should be limited the POD types only. This is because \c QDataStream stream
        operators will not only append raw data, but also e.g. size, count etc. for complex types.
*/

/*!
    \fn QModbusResponse::QModbusResponse(FunctionCode code, Args... data)

    Constructs a QModbusResponse with function code set to \a code and exception error
    code set to \ec.
*/

/*!
    \fn QModbusResponse::QModbusResponse(FunctionCode code, const QByteArray &data = QByteArray)

    Constructs a QModbusResponse with function code set to \a code and payload set to \a data.
    The data is expected to be stored in big-endian byte order already.
*/

/*!
    \class QModbusPdu
    \inmodule QtSerialBus
    \since 5.6

    \brief QModbusExceptionResponse is a container class containing the function and error code
        inside a Modbus ADU.

    A typical QModbusExceptionResponse response can looks like this:
    \code
        QModbusExceptionResponse exception(QModbusExceptionResponse::ReportServerId,
            QModbusExceptionResponse::ServerDeviceFailure);
    \endcode
*/

/*!
    \fn QModbusExceptionResponse::QModbusExceptionResponse(FunctionCode code, ExceptionCode ec)

    Constructs a QModbusExceptionResponse with function code set to \a code and exception error
    code set to \ec.
*/

/*!
    void QModbusExceptionResponse::setFunctionCode(FunctionCode c)

    Sets the response's function code to \c.
*/

/*!
    void QModbusExceptionResponse::setExeceptionCode(ExceptionCode ec)

    Sets the response's exception code to \ec.
*/

QT_END_NAMESPACE

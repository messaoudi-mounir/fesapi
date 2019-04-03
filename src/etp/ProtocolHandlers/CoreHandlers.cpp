/*-----------------------------------------------------------------------
Licensed to the Apache Software Foundation (ASF) under one
or more contributor license agreements.  See the NOTICE file
distributed with this work for additional information
regarding copyright ownership.  The ASF licenses this file
to you under the Apache License, Version 2.0 (the
"License"; you may not use this file except in compliance
with the License.  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing,
software distributed under the License is distributed on an
"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
KIND, either express or implied.  See the License for the
specific language governing permissions and limitations
under the License.
-----------------------------------------------------------------------*/
#include  "etp/ProtocolHandlers/CoreHandlers.h"

#include "etp/AbstractSession.h"

using namespace ETP_NS;

void CoreHandlers::decodeMessageBody(const Energistics::Etp::v12::Datatypes::MessageHeader & mh, avro::DecoderPtr d)
{
	if (mh.m_messageType != Energistics::Etp::v12::Protocol::Core::Acknowledge::messageTypeId && mh.m_protocol != Energistics::Etp::v12::Datatypes::Protocol::Core) {
		std::cerr << "Error : This message header does not belong to the protocol Core" << std::endl;
		return;
	}

	if (mh.m_messageType == Energistics::Etp::v12::Protocol::Core::RequestSession::messageTypeId) {
		Energistics::Etp::v12::Protocol::Core::RequestSession rs;
		avro::decode(*d, rs);
		session->flushReceivingBuffer();
		session->setEtpSessionClosed(false);
		on_RequestSession(rs, mh.m_messageId);
	}
	else if (mh.m_messageType == Energistics::Etp::v12::Protocol::Core::OpenSession::messageTypeId) {
		Energistics::Etp::v12::Protocol::Core::OpenSession os;
		avro::decode(*d, os);
		session->flushReceivingBuffer();
		session->setEtpSessionClosed(false);
		on_OpenSession(os, mh.m_messageId);
	}
	else if (mh.m_messageType == Energistics::Etp::v12::Protocol::Core::CloseSession::messageTypeId) {
		Energistics::Etp::v12::Protocol::Core::CloseSession cs;
		avro::decode(*d, cs);
		session->flushReceivingBuffer();
		on_CloseSession(cs, mh.m_messageId);
	}
	else if (mh.m_messageType == Energistics::Etp::v12::Protocol::Core::ProtocolException::messageTypeId) {
		Energistics::Etp::v12::Protocol::Core::ProtocolException pe;
		avro::decode(*d, pe);
		session->flushReceivingBuffer();
		on_ProtocolException(pe, mh.m_messageId);
	}
	else if (mh.m_messageType == Energistics::Etp::v12::Protocol::Core::Acknowledge::messageTypeId) {
		Energistics::Etp::v12::Protocol::Core::Acknowledge ack;
		avro::decode(*d, ack);
		session->flushReceivingBuffer();
		on_Acknowledge(ack, mh.m_messageId);
	}
	else {
		session->flushReceivingBuffer();
		sendExceptionCode3();
	}
}

void CoreHandlers::on_RequestSession(const Energistics::Etp::v12::Protocol::Core::RequestSession & rs, int64_t correlationId)
{
	Energistics::Etp::v12::Protocol::Core::ProtocolException error;
	error.m_errorCode = 2;
	error.m_errorMessage = "The Core::on_RequestSession method has not been overriden by the agent.";

	session->send(error);
}

void CoreHandlers::on_OpenSession(const Energistics::Etp::v12::Protocol::Core::OpenSession & os, int64_t correlationId)
{
}

void CoreHandlers::on_CloseSession(const Energistics::Etp::v12::Protocol::Core::CloseSession & cs, int64_t correlationId)
{
	std::cout << "Close session after received request." << std::endl;
	session->sendCloseFrame();
}

void CoreHandlers::on_ProtocolException(const Energistics::Etp::v12::Protocol::Core::ProtocolException & pe, int64_t correlationId)
{
	std::cout << "EXCEPTION for message_id " << correlationId << " with error code "  << pe.m_errorCode << " : " << pe.m_errorMessage << std::endl;
}

void CoreHandlers::on_Acknowledge(const Energistics::Etp::v12::Protocol::Core::Acknowledge & ack, int64_t correlationId)
{
	std::cout << "Acknowledge message_id " << correlationId << std::endl;
}
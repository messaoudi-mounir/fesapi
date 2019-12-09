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
#include "StoreHandlers.h"

#include "../AbstractSession.h"
#include "../EtpHelpers.h"

using namespace ETP_NS;

void StoreHandlers::decodeMessageBody(const Energistics::Etp::v12::Datatypes::MessageHeader & mh, avro::DecoderPtr d)
{
	if (mh.m_protocol != Energistics::Etp::v12::Datatypes::Protocol::Store) {
		std::cerr << "Error : This message header does not belong to the protocol Store" << std::endl;
		return;
	}

	if (mh.m_messageType == Energistics::Etp::v12::Protocol::Store::GetDataObjects::messageTypeId) {
		Energistics::Etp::v12::Protocol::Store::GetDataObjects getO;
		avro::decode(*d, getO);
		session->flushReceivingBuffer();
		on_GetDataObjects(getO, mh.m_messageId);
	}
	else if (mh.m_messageType == Energistics::Etp::v12::Protocol::Store::PutDataObjects::messageTypeId) {
		Energistics::Etp::v12::Protocol::Store::PutDataObjects putO;
		avro::decode(*d, putO);
		session->flushReceivingBuffer();
		on_PutDataObjects(putO, mh.m_messageId);
	}
	else if (mh.m_messageType == Energistics::Etp::v12::Protocol::Store::DeleteDataObjects::messageTypeId) {
		Energistics::Etp::v12::Protocol::Store::DeleteDataObjects deleteO;
		avro::decode(*d, deleteO);
		session->flushReceivingBuffer();
		on_DeleteDataObjects(deleteO, mh.m_messageId);
	}
	else if (mh.m_messageType == Energistics::Etp::v12::Protocol::Store::GetDataObjectsResponse::messageTypeId) {
		Energistics::Etp::v12::Protocol::Store::GetDataObjectsResponse obj;
		avro::decode(*d, obj);
		session->flushReceivingBuffer();
		on_GetDataObjectsResponse(obj, mh.m_correlationId);
	}
	else {
		session->flushReceivingBuffer();
		sendExceptionCode3();
	}
}

void StoreHandlers::on_GetDataObjects(const Energistics::Etp::v12::Protocol::Store::GetDataObjects & msg, int64_t correlationId)
{
	std::cout << "on_GetDataObjects" << std::endl;

	session->send(ETP_NS::EtpHelpers::buildSingleMessageProtocolException(7, "The StoreHandlers::on_GetDataObjects method has not been overriden by the agent."));
}

void StoreHandlers::on_PutDataObjects(const Energistics::Etp::v12::Protocol::Store::PutDataObjects & msg, int64_t correlationId)
{
	std::cout << "on_PutDataObject" << std::endl;

	session->send(ETP_NS::EtpHelpers::buildSingleMessageProtocolException(7, "The StoreHandlers::on_PutDataObject method has not been overriden by the agent."));
}

void StoreHandlers::on_DeleteDataObjects(const Energistics::Etp::v12::Protocol::Store::DeleteDataObjects & msg, int64_t correlationId)
{
	std::cout << "on_DeleteDataObject" << std::endl;

	session->send(ETP_NS::EtpHelpers::buildSingleMessageProtocolException(7, "The StoreHandlers::on_DeleteDataObject method has not been overriden by the agent."));
}

void StoreHandlers::on_GetDataObjectsResponse(const Energistics::Etp::v12::Protocol::Store::GetDataObjectsResponse & msg, int64_t correlationId)
{
	for (const auto& graphResource : msg.m_dataObjects) {
		std::cout << "Resource received : " << std::endl;
		printDataObject(graphResource.second);
	}
}

/*-----------------------------------------------------------------------
Licensed to the Apache Software Foundation (ASF) under one
or more contributor license agceements.  See the NOTICE file
distributed with this work for additional information
regarding copyright ownership.  The ASF licenses this file
to you under the Apache License, Version 2.0 (the
"License"; you may not use this file except in compliance
with the License.  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agceed to in writing,
software distributed under the License is distributed on an
"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
KIND, either express or implied.  See the License for the
specific language governing permissions and limitations
under the License.
-----------------------------------------------------------------------*/
#include "MyOwnStoreProtocolHandlers.h"

#include "etp/EtpHelpers.h"

#include "MyOwnEtpServerSession.h"

MyOwnStoreProtocolHandlers::MyOwnStoreProtocolHandlers(MyOwnEtpServerSession* mySession) : ETP_NS::StoreHandlers(mySession) {}

void MyOwnStoreProtocolHandlers::on_GetDataObjects(const Energistics::Etp::v12::Protocol::Store::GetDataObjects & getO, int64_t correlationId)
{
	Energistics::Etp::v12::Protocol::Store::GetDataObjectsResponse objResponse;

	for (const auto & uri : getO.m_uris) {
		std::cout << "Store received URI : " << uri << std::endl;

		COMMON_NS::AbstractObject* obj = static_cast<MyOwnEtpServerSession*>(session)->getObjectFromUri(uri);
		if (obj == nullptr) {
			continue;
		}

		Energistics::Etp::v12::Datatypes::Object::DataObject dataObject = ETP_NS::EtpHelpers::buildEtpDataObjectFromEnergisticsObject(obj);
		objResponse.m_dataObjects.push_back(dataObject);
	}
	session->send(objResponse, correlationId, 0x01 | 0x02);
}

void MyOwnStoreProtocolHandlers::on_PutDataObjects(const Energistics::Etp::v12::Protocol::Store::PutDataObjects & putDataObjects, int64_t correlationId)
{
	MyOwnEtpServerSession* mySession = static_cast<MyOwnEtpServerSession*>(session);
	for (const auto & dataObject : putDataObjects.m_dataObjects) {
		std::cout << "Store received data object : " << dataObject.m_resource.m_contentType << " (" << dataObject.m_resource.m_uri << ")" << std::endl;

		COMMON_NS::AbstractObject* importedObj = mySession->epcDoc.addOrReplaceGsoapProxy(dataObject.m_data, dataObject.m_resource.m_contentType);

		importedObj->resolveTargetRelationships(&mySession->epcDoc);
	}
}

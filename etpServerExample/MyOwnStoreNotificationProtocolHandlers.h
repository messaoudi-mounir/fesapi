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
#pragma once

#include "etp/ProtocolHandlers/StoreNotificationHandlers.h"

class MyDataObjectRepository;

class MyOwnStoreNotificationProtocolHandlers : public ETP_NS::StoreNotificationHandlers
{
private:

	MyDataObjectRepository* repo;

public:
	MyOwnStoreNotificationProtocolHandlers(std::shared_ptr<ETP_NS::AbstractSession> mySession, MyDataObjectRepository* repo_);
	~MyOwnStoreNotificationProtocolHandlers() {}

	void on_SubscribeNotifications(const Energistics::Etp::v12::Protocol::StoreNotification::SubscribeNotifications & msg, int64_t messageId);
	void on_UnsubscribeNotifications(const Energistics::Etp::v12::Protocol::StoreNotification::UnsubscribeNotifications & msg, int64_t messageId, int64_t correlationId);
};

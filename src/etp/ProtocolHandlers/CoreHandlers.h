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

#include "etp/ProtocolHandlers/ProtocolHandlers.h"

namespace ETP_NS
{
	class CoreHandlers : public ProtocolHandlers
	{
	public:
		CoreHandlers(AbstractSession* mySession): ProtocolHandlers(mySession) {}
		virtual ~CoreHandlers() {}

	    void decodeMessageBody(const Energistics::Etp::v12::Datatypes::MessageHeader & mh, avro::DecoderPtr d) override;

	    virtual void on_RequestSession(const Energistics::Etp::v12::Protocol::Core::RequestSession & rs, int64_t correlationId);
	    virtual void on_OpenSession(const Energistics::Etp::v12::Protocol::Core::OpenSession & os);
	    virtual void on_CloseSession(const Energistics::Etp::v12::Protocol::Core::CloseSession & cs);
	    virtual void on_ProtocolException(const Energistics::Etp::v12::Protocol::Core::ProtocolException & pe);

	};
}
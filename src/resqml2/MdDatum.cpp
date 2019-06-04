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
#include "resqml2/MdDatum.h"

#include <stdexcept>

#include "resqml2/AbstractLocal3dCrs.h"
#include "resqml2_0_1/WellboreTrajectoryRepresentation.h"

using namespace std;
using namespace RESQML2_NS;
using namespace gsoap_resqml2_0_1;
using namespace epc;

const char* MdDatum::XML_TAG = "MdDatum";

void MdDatum::resolveTargetRelationships(COMMON_NS::EpcDocument* epcDoc)
{
	_resqml2__MdDatum* mdInfo = static_cast<_resqml2__MdDatum*>(gsoapProxy2_0_1);

	updateXml = false;
	setLocalCrs(getOrCreateObjectFromDor<AbstractLocal3dCrs>(mdInfo->LocalCrs));
	updateXml = true;
}

vector<Relationship> MdDatum::getAllSourceRelationships() const
{
	vector<Relationship> result = common::AbstractObject::getAllSourceRelationships();

	// WellboreFeature trajectories
	for (size_t i = 0; i < wellboreTrajectoryRepresentationSet.size(); ++i)
	{
		Relationship rel(wellboreTrajectoryRepresentationSet[i]->getPartNameInEpcDocument(), "", wellboreTrajectoryRepresentationSet[i]->getUuid());
		rel.setSourceObjectType();
		result.push_back(rel);
	}

	return result;
}

vector<Relationship> MdDatum::getAllTargetRelationships() const
{
	vector<Relationship> result;

	// local 3d CRS
	AbstractLocal3dCrs* localCrs = getLocalCrs();
	if (localCrs != nullptr)
	{
		Relationship relLocalCrs(localCrs->getPartNameInEpcDocument(), "", getLocalCrsUuid());
		relLocalCrs.setDestinationObjectType();
		result.push_back(relLocalCrs);
	}
	else {
		throw domain_error("The local CRS associated to the MD information cannot be nullptr.");
	}

	return result;
}


void MdDatum::setLocalCrs(AbstractLocal3dCrs * localCrs)
{
	localCrs->addMdDatum(this);

	if (updateXml) {
		setXmlLocalCrs(localCrs);
	}
}

std::string MdDatum::getLocalCrsUuid() const
{
	return getLocalCrsDor()->UUID;
}

AbstractLocal3dCrs * MdDatum::getLocalCrs() const
{
	const string uuidLocalCrs = getLocalCrsUuid();
	return static_cast<AbstractLocal3dCrs*>(epcDocument->getDataObjectByUuid(uuidLocalCrs));
}


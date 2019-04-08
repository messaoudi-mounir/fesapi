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
#include "resqml2_0_1/WellboreMarkerFrameRepresentation.h"

#include <stdexcept>

#if defined(__gnu_linux__) || defined(__APPLE__)
#include <unistd.h>
#include <pwd.h>
#endif

#include "H5Tpublic.h"

#include "resqml2_0_1/WellboreInterpretation.h"
#include "resqml2_0_1/WellboreTrajectoryRepresentation.h"
#include "resqml2/AbstractLocal3dCrs.h"
#include "tools/GuidTools.h"
#include "resqml2_0_1/WellboreMarker.h"
#include "resqml2_0_1/StratigraphicOccurrenceInterpretation.h"
#include "resqml2_0_1/HorizonInterpretation.h"
#include "common/AbstractHdfProxy.h"

using namespace std;
using namespace RESQML2_0_1_NS;
using namespace gsoap_resqml2_0_1;
using namespace epc;

const char* WellboreMarkerFrameRepresentation::XML_TAG = "WellboreMarkerFrameRepresentation";

WellboreMarkerFrameRepresentation::WellboreMarkerFrameRepresentation(WellboreInterpretation* interp, const std::string & guid, const std::string & title, WellboreTrajectoryRepresentation * traj):
	WellboreFrameRepresentation(interp, nullptr), stratigraphicOccurrenceInterpretation(nullptr)
{
	gsoapProxy2_0_1 = soap_new_resqml2__obj_USCOREWellboreMarkerFrameRepresentation(interp->getGsoapContext(), 1);	
	_resqml2__WellboreMarkerFrameRepresentation* frame = static_cast<_resqml2__WellboreMarkerFrameRepresentation*>(gsoapProxy2_0_1);

	setInterpretation(interp);

	frame->Trajectory = traj->newResqmlReference();
	traj->addWellboreFrameRepresentation(this);

	initMandatoryMetadata();
	setMetadata(guid, title, std::string(), -1, std::string(), std::string(), -1, std::string());
}

WellboreMarkerFrameRepresentation::~WellboreMarkerFrameRepresentation()
{
	for (size_t i = 0; i < markerSet.size(); ++i) {
		delete markerSet[i];
	}
}

WellboreMarker* WellboreMarkerFrameRepresentation::pushBackNewWellboreMarker(const std::string & guid, const std::string & title)
{
	WellboreMarker* marker = new WellboreMarker(this, guid, title);
	markerSet.push_back(marker);

	_resqml2__WellboreMarkerFrameRepresentation* frame = static_cast<_resqml2__WellboreMarkerFrameRepresentation*>(gsoapProxy2_0_1);
	frame->WellboreMarker.push_back(static_cast<resqml2__WellboreMarker*>(marker->getGsoapProxy()));	

	return marker;
}

WellboreMarker* WellboreMarkerFrameRepresentation::pushBackNewWellboreMarker(const std::string & guid, const std::string & title, const gsoap_resqml2_0_1::resqml2__GeologicBoundaryKind & geologicBoundaryKind)
{
	WellboreMarker* marker = new WellboreMarker(this, guid, title, geologicBoundaryKind);
	markerSet.push_back(marker);

	_resqml2__WellboreMarkerFrameRepresentation* frame = static_cast<_resqml2__WellboreMarkerFrameRepresentation*>(gsoapProxy2_0_1);
	frame->WellboreMarker.push_back(static_cast<resqml2__WellboreMarker*>(marker->getGsoapProxy()));	

	return marker;
}

unsigned int WellboreMarkerFrameRepresentation::getWellboreMarkerCount()
{
	return static_cast<_resqml2__WellboreMarkerFrameRepresentation*>(gsoapProxy2_0_1)->WellboreMarker.size();
}

void WellboreMarkerFrameRepresentation::setStratigraphicOccurrenceInterpretation( StratigraphicOccurrenceInterpretation * stratiOccurenceInterp)
{
	// EPC
	stratigraphicOccurrenceInterpretation = stratiOccurenceInterp;
	stratiOccurenceInterp->wellboreMarkerFrameRepresentationSet.push_back(this);

	// XML
	if (updateXml)
	{
		_resqml2__WellboreMarkerFrameRepresentation* frame = static_cast<_resqml2__WellboreMarkerFrameRepresentation*>(gsoapProxy2_0_1);
		frame->IntervalStratigraphiUnits = soap_new_resqml2__IntervalStratigraphicUnits(frame->soap, 1);
		frame->IntervalStratigraphiUnits->StratigraphicOrganization = stratiOccurenceInterp->newResqmlReference();
	}
}

void WellboreMarkerFrameRepresentation::setIntervalStratigraphicUnits(unsigned int * stratiUnitIndices, const unsigned int & nullValue, StratigraphicOccurrenceInterpretation* stratiOccurenceInterp)
{
	if (stratiUnitIndices == nullptr) {
		throw invalid_argument("The strati unit indices cannot be null.");
	}

	setStratigraphicOccurrenceInterpretation(stratiOccurenceInterp);

	_resqml2__WellboreMarkerFrameRepresentation* frame = static_cast<_resqml2__WellboreMarkerFrameRepresentation*>(gsoapProxy2_0_1);

	resqml2__IntegerHdf5Array* xmlDataset = soap_new_resqml2__IntegerHdf5Array(frame->soap, 1);
	xmlDataset->NullValue = nullValue;
	xmlDataset->Values = soap_new_eml20__Hdf5Dataset(gsoapProxy2_0_1->soap, 1);
	xmlDataset->Values->HdfProxy = hdfProxy->newResqmlReference();
	xmlDataset->Values->PathInHdfFile = "/RESQML/" + frame->uuid + "/IntervalStratigraphicUnits";
	frame->IntervalStratigraphiUnits->UnitIndices = xmlDataset;

	// ************ HDF *************
	hsize_t dim = frame->NodeCount - 1;
	hdfProxy->writeArrayNd(frame->uuid, "IntervalStratigraphicUnits", H5T_NATIVE_UINT, stratiUnitIndices, &dim, 1);
}

vector<Relationship> WellboreMarkerFrameRepresentation::getAllTargetRelationships() const
{
	vector<Relationship> result = WellboreFrameRepresentation::getAllTargetRelationships();

	// XML forward relationship
	if (stratigraphicOccurrenceInterpretation != nullptr)
	{
		Relationship relStratiRank(stratigraphicOccurrenceInterpretation->getPartNameInEpcDocument(), "", stratigraphicOccurrenceInterpretation->getUuid());
		relStratiRank.setDestinationObjectType();
		result.push_back(relStratiRank);
	}

	for (size_t i = 0; i < markerSet.size(); ++i)
	{
		if (markerSet[i]->getBoundaryFeatureInterpretation() != nullptr)
		{
			Relationship relBoundaryFeature(markerSet[i]->getBoundaryFeatureInterpretation()->getPartNameInEpcDocument(), "", markerSet[i]->getBoundaryFeatureInterpretation()->getUuid());
			relBoundaryFeature.setDestinationObjectType();
			result.push_back(relBoundaryFeature);
		}
	}

	return result;
}

void WellboreMarkerFrameRepresentation::resolveTargetRelationships(COMMON_NS::EpcDocument* epcDoc)
{
	WellboreFrameRepresentation::resolveTargetRelationships(epcDoc);

	_resqml2__WellboreMarkerFrameRepresentation* rep = static_cast<_resqml2__WellboreMarkerFrameRepresentation*>(gsoapProxy2_0_1);

	updateXml = false;

	if (rep->IntervalStratigraphiUnits != nullptr)
	{
		setStratigraphicOccurrenceInterpretation(epcDoc->getDataObjectByUuid<RESQML2_0_1_NS::StratigraphicOccurrenceInterpretation>(rep->IntervalStratigraphiUnits->StratigraphicOrganization->UUID));
	}

	for (size_t i = 0; i < rep->WellboreMarker.size(); ++i)
	{
		WellboreMarker* marker = new WellboreMarker(rep->WellboreMarker[i], this);
		if (rep->WellboreMarker[i]->Interpretation != nullptr)
		{
			marker->setBoundaryFeatureInterpretation(static_cast<BoundaryFeatureInterpretation*>(epcDoc->getDataObjectByUuid(rep->WellboreMarker[i]->Interpretation->UUID)));
		}
		markerSet.push_back(marker);
	}

	updateXml = true;
}

const std::vector<class WellboreMarker*> & WellboreMarkerFrameRepresentation::getWellboreMarkerSet() const
{
	return markerSet;
}

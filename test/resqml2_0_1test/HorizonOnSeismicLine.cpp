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
#include "resqml2_0_1test/HorizonOnSeismicLine.h"
#include "../catch.hpp"

#include "resqml2/AbstractLocal3dCrs.h"
#include "resqml2_0_1/PolylineRepresentation.h"
#include "resqml2_0_1/HorizonInterpretation.h"
#include "resqml2_0_1/LocalDepth3dCrs.h"
#include "common/HdfProxy.h"
#include "resqml2_0_1test/HorizonInterpretationTest.h"
#include "resqml2_0_1test/SeismicLineRepresentationTest.h"
#include "LocalDepth3dCrsTest.h"

using namespace std;
using namespace resqml2_0_1test;
using namespace COMMON_NS;
using namespace RESQML2_0_1_NS;

const char* HorizonOnSeismicLine::defaultUuid = "c9bea300-6231-4acb-8b7c-77e4099224d4";
const char* HorizonOnSeismicLine::defaultTitle = "Horizon on seismic line";
double HorizonOnSeismicLine::defaultXyzPoints[12] = { 0, 100, 300, 150, 110, 300, 450, 130, 350, 600, 140, 350 };

HorizonOnSeismicLine::HorizonOnSeismicLine(const string & epcDocPath)
	: commontest::AbstractObjectTest(epcDocPath)
{
}

HorizonOnSeismicLine::HorizonOnSeismicLine(EpcDocument * epcDocument, bool init)
	: commontest::AbstractObjectTest(epcDocument)
{
	if (init)
		initEpcDoc();
	else
		readEpcDoc();
}

void HorizonOnSeismicLine::initEpcDocHandler()
{
	COMMON_NS::AbstractHdfProxy * hdfProxy = epcDoc->getHdfProxy(0);

	HorizonInterpretation* horizonInterp = epcDoc->getDataObjectByUuid<HorizonInterpretation>(HorizonInterpretationTest::defaultUuid);
	if (horizonInterp == nullptr) {
		HorizonInterpretationTest horizonInterpTest(epcDoc, true);
		horizonInterp = epcDoc->getDataObjectByUuid<HorizonInterpretation>(HorizonInterpretationTest::defaultUuid);
	}
	
	PolylineRepresentation* seismicLineRep = epcDoc->getDataObjectByUuid<PolylineRepresentation>(SeismicLineRepresentationTest::defaultUuid);
	if (seismicLineRep == nullptr) {
		SeismicLineRepresentationTest seismicLineRepresentationTest(epcDoc, true);
		seismicLineRep = epcDoc->getDataObjectByUuid<PolylineRepresentation>(SeismicLineRepresentationTest::defaultUuid);
	}

	RESQML2_NS::AbstractLocal3dCrs * crs = seismicLineRep->getLocalCrs();;

	PolylineRepresentation* h1i1SinglePolylineRep = epcDoc->createPolylineRepresentation(horizonInterp, crs, defaultUuid, defaultTitle);
	h1i1SinglePolylineRep->setGeometry(defaultXyzPoints, 4, hdfProxy);
	double seismicLineAbscissa[4] = { 0.0, 1.0, 3.0, 4.0 };
	h1i1SinglePolylineRep->addSeismic2dCoordinatesToPatch(0, seismicLineAbscissa, seismicLineRep, hdfProxy);
}

void HorizonOnSeismicLine::readEpcDocHandler()
{
	PolylineRepresentation* h1i1SinglePolylineRep = epcDoc->getDataObjectByUuid<PolylineRepresentation>(defaultUuid);
	REQUIRE(h1i1SinglePolylineRep->getSeismicSupportOfPatch(0) != nullptr);

	double seismicLineAbscissa[4] = { -1.0, -1.0, -1.0, -1.0 };
	h1i1SinglePolylineRep->getSeismicLineAbscissaOfPointsOfPatch(0, seismicLineAbscissa);
	REQUIRE(seismicLineAbscissa[0] == 0.0);
	REQUIRE(seismicLineAbscissa[1] == 1.0);
	REQUIRE(seismicLineAbscissa[2] == 3.0);
	REQUIRE(seismicLineAbscissa[3] == 4.0);
}

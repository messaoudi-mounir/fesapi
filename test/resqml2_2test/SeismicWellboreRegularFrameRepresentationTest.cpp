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
#include "resqml2_2test/SeismicWellboreRegularFrameRepresentationTest.h"

#include "catch.hpp"

#include "resqml2_0_1test/WellboreInterpretationTest.h"
#include "resqml2_0_1test/WellboreTrajectoryRepresentationTest.h"
#include "resqml2_0_1test/LocalTime3dCrs.h"

#include "resqml2_0_1/WellboreInterpretation.h"
#include "resqml2_0_1/WellboreTrajectoryRepresentation.h"
#include "common/AbstractHdfProxy.h"
#include "resqml2_2/SeismicWellboreFrameRepresentation.h"
#include "resqml2_0_1/LocalTime3dCrs.h"

using namespace std;
using namespace COMMON_NS;
using namespace RESQML2_2_NS;
using namespace resqml2_2test;

const char* SeismicWellboreRegularFrameRepresentationTest::defaultUuid = "04a13a04-c22e-4da5-9a33-a1de5c5e4d58";
const char* SeismicWellboreRegularFrameRepresentationTest::defaultTitle = "SeismicWellbore Frame Representation";

SeismicWellboreRegularFrameRepresentationTest::SeismicWellboreRegularFrameRepresentationTest(const string & epcDocPath)
	: commontest::AbstractObjectTest(epcDocPath) {
}

SeismicWellboreRegularFrameRepresentationTest::SeismicWellboreRegularFrameRepresentationTest(DataObjectRepository * repo, bool init)
	: commontest::AbstractObjectTest(repo) {
	if (init)
		initRepo();
	else
		readRepo();
}

void SeismicWellboreRegularFrameRepresentationTest::initRepoHandler() {
	// creating dependencies
	resqml2_0_1test::WellboreTrajectoryRepresentationTest trajTest(repo, true);

	RESQML2_0_1_NS::WellboreInterpretation * interp = static_cast<RESQML2_0_1_NS::WellboreInterpretation*>(repo->getDataObjectByUuid(resqml2_0_1test::WellboreInterpretationTest::defaultUuid));
	RESQML2_0_1_NS::WellboreTrajectoryRepresentation * traj = static_cast<RESQML2_0_1_NS::WellboreTrajectoryRepresentation*>(repo->getDataObjectByUuid(resqml2_0_1test::WellboreTrajectoryRepresentationTest::defaultUuid));

	// getting the hdf proxy
	COMMON_NS::AbstractHdfProxy* hdfProxy = repo->getHdfProxySet()[0];
	REQUIRE(hdfProxy != nullptr);

	// getting the local time crs
	RESQML2_0_1_NS::LocalTime3dCrs* crs = repo->getDataObjectByUuid<RESQML2_0_1_NS::LocalTime3dCrs>(resqml2_0_1test::LocalTime3dCrs::defaultUuid);
	if (crs == nullptr) {
		resqml2_0_1test::LocalTime3dCrs crsTest(repo, true);
		crs = repo->getDataObjectByUuid<RESQML2_0_1_NS::LocalTime3dCrs>(resqml2_0_1test::LocalTime3dCrs::defaultUuid);
	}

	// WellboreFeature frame
	SeismicWellboreFrameRepresentation* w1i1RegularSeismicFrameRep = repo->createSeismicWellboreFrameRepresentation(interp, defaultUuid, defaultTitle, traj, 0., 0., crs);
	w1i1RegularSeismicFrameRep->setMdValues(0, 200, 6);
	w1i1RegularSeismicFrameRep->setTimeValues(0., 10., 6);
}

void SeismicWellboreRegularFrameRepresentationTest::readRepoHandler() {
	SeismicWellboreFrameRepresentation* w1i1RegularSeismicFrameRep = repo->getDataObjectByUuid<SeismicWellboreFrameRepresentation>(defaultUuid);
	REQUIRE(w1i1RegularSeismicFrameRep != nullptr);

	REQUIRE(w1i1RegularSeismicFrameRep->areMdValuesRegularlySpaced());
	REQUIRE(w1i1RegularSeismicFrameRep->getMdValuesCount() == 6);

	REQUIRE(w1i1RegularSeismicFrameRep->getMdFirstValue() == .0);
	REQUIRE(w1i1RegularSeismicFrameRep->getMdConstantIncrementValue() == 200);

	REQUIRE(w1i1RegularSeismicFrameRep->areTimeValuesRegularlySpaced());
	REQUIRE(w1i1RegularSeismicFrameRep->getTimeValuesCount() == 6);

	REQUIRE(w1i1RegularSeismicFrameRep->getTimeFirstValue() == .0);
	REQUIRE(w1i1RegularSeismicFrameRep->getTimeConstantIncrementValue() == 10.);

	REQUIRE(w1i1RegularSeismicFrameRep->getSeismicReferenceDatum() == 0.);
	REQUIRE(w1i1RegularSeismicFrameRep->getWeatheringVelocity() == 0.);
}

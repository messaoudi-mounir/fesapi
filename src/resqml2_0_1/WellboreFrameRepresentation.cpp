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
#include "resqml2_0_1/WellboreFrameRepresentation.h"

#include <stdexcept>

#include "hdf5.h"

#include "resqml2_0_1/WellboreInterpretation.h"
#include "resqml2_0_1/WellboreTrajectoryRepresentation.h"
#include "resqml2/AbstractLocal3dCrs.h"
#include "common/AbstractHdfProxy.h"

using namespace std;
using namespace RESQML2_0_1_NS;
using namespace gsoap_resqml2_0_1;

const char* WellboreFrameRepresentation::XML_TAG = "WellboreFrameRepresentation";

WellboreFrameRepresentation::WellboreFrameRepresentation(WellboreInterpretation * interp, const string & guid, const std::string & title, WellboreTrajectoryRepresentation * traj)
{
	gsoapProxy2_0_1 = soap_new_resqml2__obj_USCOREWellboreFrameRepresentation(interp->getGsoapContext(), 1);	
	_resqml2__WellboreFrameRepresentation* frame = static_cast<_resqml2__WellboreFrameRepresentation*>(gsoapProxy2_0_1);

	initMandatoryMetadata();
	setMetadata(guid, title, std::string(), -1, std::string(), std::string(), -1, std::string());

	setInterpretation(interp);

	frame->Trajectory = traj->newResqmlReference();
	getRepository()->addRelationship(this, traj);
}

void WellboreFrameRepresentation::getXyzPointsOfPatch(const unsigned int & patchIndex, double * xyzPoints) const
{
	if (patchIndex >= getPatchCount())
		throw range_error("The index of the patch is not in the allowed range of patch.");

	resqml2__PointGeometry* pointGeom = getPointGeometry2_0_1(patchIndex);
	if (pointGeom != nullptr && pointGeom->Points->soap_type() == SOAP_TYPE_gsoap_resqml2_0_1_resqml2__Point3dHdf5Array)
	{
		eml20__Hdf5Dataset const * dataset = static_cast<resqml2__Point3dHdf5Array*>(pointGeom->Points)->Coordinates;
		COMMON_NS::AbstractHdfProxy * hdfProxy = getRepository()->getDataObjectByUuid<COMMON_NS::AbstractHdfProxy>(dataset->HdfProxy->UUID);
		if (hdfProxy == nullptr) {
			throw invalid_argument("The HDF proxy is missing.");
		}
		hdfProxy->readArrayNdOfDoubleValues(dataset->PathInHdfFile, xyzPoints);
	}
	else
		throw invalid_argument("The geometry of the representation either does not exist or it is not an explicit one.");
}

void WellboreFrameRepresentation::setMdValues(double * mdValues, unsigned int mdValueCount, COMMON_NS::AbstractHdfProxy * proxy)
{
	getRepository()->addRelationship(this, proxy);

	_resqml2__WellboreFrameRepresentation* frame = static_cast<_resqml2__WellboreFrameRepresentation*>(gsoapProxy2_0_1);

	// XML
	resqml2__DoubleHdf5Array* xmlMdValues = soap_new_resqml2__DoubleHdf5Array(gsoapProxy2_0_1->soap, 1);
	xmlMdValues->Values = soap_new_eml20__Hdf5Dataset(gsoapProxy2_0_1->soap, 1);
	xmlMdValues->Values->HdfProxy = proxy->newResqmlReference();
	xmlMdValues->Values->PathInHdfFile = "/RESQML/" + frame->uuid + "/mdValues";

	frame->NodeMd = xmlMdValues;

	frame->NodeCount = mdValueCount;

	// HDF
	hsize_t dim[] = {mdValueCount};
	proxy->writeArrayNd(frame->uuid,
		"mdValues",
		H5T_NATIVE_DOUBLE,
		mdValues,
		dim, 1);
}

void WellboreFrameRepresentation::setMdValues(const double & firstMdValue, const double & incrementMdValue, const unsigned int & mdValueCount)
{
	_resqml2__WellboreFrameRepresentation* frame = static_cast<_resqml2__WellboreFrameRepresentation*>(gsoapProxy2_0_1);

	// XML
	resqml2__DoubleLatticeArray* xmlMdValues = soap_new_resqml2__DoubleLatticeArray(gsoapProxy2_0_1->soap, 1);
	xmlMdValues->StartValue = firstMdValue;
	xmlMdValues->Offset.push_back(soap_new_resqml2__DoubleConstantArray(gsoapProxy2_0_1->soap, 1));
	xmlMdValues->Offset[0]->Count = mdValueCount - 1;
	xmlMdValues->Offset[0]->Value = incrementMdValue;

	frame->NodeMd = xmlMdValues;

	frame->NodeCount = mdValueCount;
}

bool WellboreFrameRepresentation::areMdValuesRegularlySpaced() const
{
	return static_cast<_resqml2__WellboreFrameRepresentation*>(gsoapProxy2_0_1)->NodeMd->soap_type() == SOAP_TYPE_gsoap_resqml2_0_1_resqml2__DoubleLatticeArray;
}

double WellboreFrameRepresentation::getMdConstantIncrementValue() const
{
	if (!areMdValuesRegularlySpaced())
		throw invalid_argument("The MD values are not regularly spaced.");

	return static_cast<resqml2__DoubleLatticeArray*>(static_cast<_resqml2__WellboreFrameRepresentation*>(gsoapProxy2_0_1)->NodeMd)->Offset[0]->Value;
}

double WellboreFrameRepresentation::getMdFirstValue() const
{
	_resqml2__WellboreFrameRepresentation* frame = static_cast<_resqml2__WellboreFrameRepresentation*>(gsoapProxy2_0_1);
	if (frame->NodeMd->soap_type() == SOAP_TYPE_gsoap_resqml2_0_1_resqml2__DoubleHdf5Array)
	{
		eml20__Hdf5Dataset const * dataset = static_cast<resqml2__DoubleHdf5Array*>(frame->NodeMd)->Values;
		COMMON_NS::AbstractHdfProxy * hdfProxy = getRepository()->getDataObjectByUuid<COMMON_NS::AbstractHdfProxy>(dataset->HdfProxy->UUID);
		if (hdfProxy == nullptr) {
			throw invalid_argument("The HDF proxy is missing.");
		}
		double* values = new double[getMdValuesCount()];
		hdfProxy->readArrayNdOfDoubleValues(dataset->PathInHdfFile, values);
		double result = values[0];
		delete [] values;

		return result;
	}
	else if (frame->NodeMd->soap_type() == SOAP_TYPE_gsoap_resqml2_0_1_resqml2__DoubleLatticeArray)
	{
		return static_cast<resqml2__DoubleLatticeArray*>(frame->NodeMd)->StartValue;
	}
	else
		throw logic_error("The array structure of MD is not supported?");
}

unsigned int WellboreFrameRepresentation::getMdValuesCount() const
{
	return static_cast<_resqml2__WellboreFrameRepresentation*>(gsoapProxy2_0_1)->NodeCount;
}

COMMON_NS::AbstractObject::hdfDatatypeEnum WellboreFrameRepresentation::getMdHdfDatatype() const
{
	_resqml2__WellboreFrameRepresentation* frame = static_cast<_resqml2__WellboreFrameRepresentation*>(gsoapProxy2_0_1);
	if (frame->NodeMd->soap_type() == SOAP_TYPE_gsoap_resqml2_0_1_resqml2__DoubleHdf5Array) {
		eml20__Hdf5Dataset const * dataset = static_cast<resqml2__DoubleHdf5Array*>(frame->NodeMd)->Values;
		COMMON_NS::AbstractHdfProxy * hdfProxy = getRepository()->getDataObjectByUuid<COMMON_NS::AbstractHdfProxy>(dataset->HdfProxy->UUID);
		if (hdfProxy == nullptr) {
			return COMMON_NS::AbstractObject::UNKNOWN;
		}

		return hdfProxy->getHdfDatatypeInDataset(static_cast<resqml2__DoubleHdf5Array*>(frame->NodeMd)->Values->PathInHdfFile);
	}
	else if (frame->NodeMd->soap_type() == SOAP_TYPE_gsoap_resqml2_0_1_resqml2__DoubleLatticeArray) {
		return COMMON_NS::AbstractObject::DOUBLE;
	}

	return COMMON_NS::AbstractObject::UNKNOWN;
}

void WellboreFrameRepresentation::getMdAsDoubleValues(double * values) const
{	_resqml2__WellboreFrameRepresentation* frame = static_cast<_resqml2__WellboreFrameRepresentation*>(gsoapProxy2_0_1);
	if (frame->NodeMd->soap_type() == SOAP_TYPE_gsoap_resqml2_0_1_resqml2__DoubleHdf5Array)
	{
		eml20__Hdf5Dataset const * dataset = static_cast<resqml2__DoubleHdf5Array*>(frame->NodeMd)->Values;
		COMMON_NS::AbstractHdfProxy * hdfProxy = getRepository()->getDataObjectByUuid<COMMON_NS::AbstractHdfProxy>(dataset->HdfProxy->UUID);
		if (hdfProxy == nullptr) {
			throw invalid_argument("The HDF proxy is missing.");
		}
		hdfProxy->readArrayNdOfDoubleValues(dataset->PathInHdfFile, values);
	}
	else if (frame->NodeMd->soap_type() == SOAP_TYPE_gsoap_resqml2_0_1_resqml2__DoubleLatticeArray)
	{
		values[0] = static_cast<resqml2__DoubleLatticeArray*>(frame->NodeMd)->StartValue;
		resqml2__DoubleConstantArray* constantArray = static_cast<resqml2__DoubleLatticeArray*>(frame->NodeMd)->Offset[0];
		for (ULONG64 inc = 1; inc <= constantArray->Count; ++inc)
			values[inc] = values[0] + (inc * constantArray->Value);
	}
	else
		throw logic_error("The array structure of MD is not supported?");
}

void WellboreFrameRepresentation::getMdAsFloatValues(float *  values) const
{
	_resqml2__WellboreFrameRepresentation* frame = static_cast<_resqml2__WellboreFrameRepresentation*>(gsoapProxy2_0_1);
	if (frame->NodeMd->soap_type() == SOAP_TYPE_gsoap_resqml2_0_1_resqml2__DoubleHdf5Array)
	{
		eml20__Hdf5Dataset const * dataset = static_cast<resqml2__DoubleHdf5Array*>(frame->NodeMd)->Values;
		COMMON_NS::AbstractHdfProxy * hdfProxy = getRepository()->getDataObjectByUuid<COMMON_NS::AbstractHdfProxy>(dataset->HdfProxy->UUID);
		if (hdfProxy == nullptr) {
			throw invalid_argument("The HDF proxy is missing.");
		}
		hdfProxy->readArrayNdOfFloatValues(dataset->PathInHdfFile, values);
	}
	else if (frame->NodeMd->soap_type() == SOAP_TYPE_gsoap_resqml2_0_1_resqml2__DoubleLatticeArray)
	{
		values[0] = static_cast<resqml2__DoubleLatticeArray*>(frame->NodeMd)->StartValue;
		resqml2__DoubleConstantArray* constantArray = static_cast<resqml2__DoubleLatticeArray*>(frame->NodeMd)->Offset[0];
		for (ULONG64 inc = 1; inc <= constantArray->Count; ++inc)
			values[inc] = values[0] + (inc * constantArray->Value);
	}
	else
		throw logic_error("The array structure of MD is not supported?");
}

std::string WellboreFrameRepresentation::getWellboreTrajectoryUuid() const
{
	return static_cast<_resqml2__WellboreFrameRepresentation*>(gsoapProxy2_0_1)->Trajectory->UUID;
}

gsoap_resqml2_0_1::eml20__DataObjectReference* WellboreFrameRepresentation::getLocalCrsDor() const
{
	return getWellboreTrajectory()->getLocalCrsDor();
}

gsoap_resqml2_0_1::eml20__DataObjectReference* WellboreFrameRepresentation::getHdfProxyDor() const
{
	_resqml2__WellboreFrameRepresentation* frame = static_cast<_resqml2__WellboreFrameRepresentation*>(gsoapProxy2_0_1);
	if (frame->NodeMd->soap_type() == SOAP_TYPE_gsoap_resqml2_0_1_resqml2__DoubleHdf5Array)
	{
		return static_cast<resqml2__DoubleHdf5Array*>(frame->NodeMd)->Values->HdfProxy;
	}

	return nullptr;
}

ULONG64 WellboreFrameRepresentation::getXyzPointCountOfPatch(const unsigned int & patchIndex) const
{
	if (patchIndex >= getPatchCount()) {
		throw range_error("The patch index is out of range");
	}

	return getMdValuesCount();
}

WellboreTrajectoryRepresentation* WellboreFrameRepresentation::getWellboreTrajectory() const
{
	return getRepository()->getDataObjectByUuid<WellboreTrajectoryRepresentation>(static_cast<_resqml2__WellboreFrameRepresentation*>(gsoapProxy2_0_1)->Trajectory->UUID);
}

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
#include "resqml2_0_1/SealedSurfaceFrameworkRepresentation.h"

#include <stdexcept>
#include <sstream>

#include "H5Tpublic.h"

#include "resqml2_0_1/StructuralOrganizationInterpretation.h"
#include "common/AbstractHdfProxy.h"

using namespace std;
using namespace epc;
using namespace RESQML2_0_1_NS;
using namespace gsoap_resqml2_0_1;

const char* SealedSurfaceFrameworkRepresentation::XML_TAG = "SealedSurfaceFrameworkRepresentation";

SealedSurfaceFrameworkRepresentation::SealedSurfaceFrameworkRepresentation(
        StructuralOrganizationInterpretation* interp,
        const std::string & guid,
        const std::string & title
        ):
	AbstractSurfaceFrameworkRepresentation(interp)
{
    if (interp == nullptr)
        throw invalid_argument("The structural organization interpretation cannot be null.");

    // proxy constructor
    gsoapProxy2_0_1 = soap_new_resqml2__obj_USCORESealedSurfaceFrameworkRepresentation(interp->getGsoapContext(), 1);
    _resqml2__SealedSurfaceFrameworkRepresentation* orgRep = static_cast<_resqml2__SealedSurfaceFrameworkRepresentation*>(gsoapProxy2_0_1);

    orgRep->RepresentedInterpretation = soap_new_eml20__DataObjectReference(gsoapProxy2_0_1->soap, 1);
    orgRep->RepresentedInterpretation->UUID.assign(interp->getUuid());

    initMandatoryMetadata();
    setMetadata(guid, title, std::string(), -1, std::string(), std::string(), -1, std::string());

	// XML relationships
    setInterpretation(interp);
}

void SealedSurfaceFrameworkRepresentation::pushBackSealedContactRepresentation(const gsoap_resqml2_0_1::resqml2__IdentityKind & kind)
{
    _resqml2__SealedSurfaceFrameworkRepresentation* orgRep = static_cast<_resqml2__SealedSurfaceFrameworkRepresentation*>(gsoapProxy2_0_1);

    resqml2__SealedContactRepresentationPart* contactRep = soap_new_resqml2__SealedContactRepresentationPart(gsoapProxy2_0_1->soap, 1);
    contactRep->Index = orgRep->SealedContactRepresentation.size();
    contactRep->IdentityKind = kind;
    orgRep->SealedContactRepresentation.push_back(contactRep);
}

void SealedSurfaceFrameworkRepresentation::pushBackSealedContactRepresentation(
        const gsoap_resqml2_0_1::resqml2__IdentityKind & kind,
        const unsigned int & patchCount,
        const unsigned int & identicalNodesCount,
        int * identicalNodes,
		COMMON_NS::AbstractHdfProxy * proxy)
{
    if (patchCount < 2)
        throw invalid_argument("Contact point count cannot be less than two.");
    if (identicalNodesCount <= 0)
        throw invalid_argument("The identical nodes count cannot be lesser or equal to zero.");
    if (!identicalNodes)
        throw invalid_argument("The array of identical nodes cannot be null.");
    if (!proxy)
        throw invalid_argument("The HDF proxy cannot be null.");

    setHdfProxy(proxy);
    _resqml2__SealedSurfaceFrameworkRepresentation* orgRep = static_cast<_resqml2__SealedSurfaceFrameworkRepresentation*>(gsoapProxy2_0_1);

    resqml2__SealedContactRepresentationPart* contactRep = soap_new_resqml2__SealedContactRepresentationPart(gsoapProxy2_0_1->soap, 1);
    contactRep->Index = orgRep->SealedContactRepresentation.size();
    orgRep->SealedContactRepresentation.push_back(contactRep);

    resqml2__IntegerHdf5Array * xmlListOfIdenticalNodes = soap_new_resqml2__IntegerHdf5Array(gsoapProxy2_0_1->soap, 1);
    xmlListOfIdenticalNodes->NullValue = (std::numeric_limits<int>::max)();
    xmlListOfIdenticalNodes->Values = soap_new_eml20__Hdf5Dataset(gsoapProxy2_0_1->soap, 1);
    xmlListOfIdenticalNodes->Values->HdfProxy = proxy->newResqmlReference();
    ostringstream ossForHdf;
    ossForHdf << "listOfIdenticalNodes_contact" << contactRep->Index;
    xmlListOfIdenticalNodes->Values->PathInHdfFile = "/RESQML/" + gsoapProxy2_0_1->uuid + "/" + ossForHdf.str();
    contactRep->IdenticalNodeIndices = xmlListOfIdenticalNodes;
    // ************ HDF *************
    hsize_t dim[2] = {identicalNodesCount, patchCount};
    proxy->writeArrayNd(gsoapProxy2_0_1->uuid,
        ossForHdf.str(), H5T_NATIVE_UINT,
        identicalNodes,
        dim, 2);
}

void SealedSurfaceFrameworkRepresentation::pushBackContactPatchInSealedContactRepresentation(
        const unsigned int & contactIndex,
        int * nodeIndicesOnSupportingRepresentation, const unsigned int & NodeCount,
        AbstractRepresentation * supportingRepresentation,
		COMMON_NS::AbstractHdfProxy * proxy)
{
	if (nodeIndicesOnSupportingRepresentation == nullptr) {
		throw invalid_argument("The array of node indices cannot be null.");
	}
	if (NodeCount <= 0) {
		throw invalid_argument("The nodes count cannot be lesser or equal to zero.");
	}
	if (supportingRepresentation == nullptr) {
		throw invalid_argument("The supporting representation cannot be null.");
	}

    setHdfProxy(proxy);
    _resqml2__SealedSurfaceFrameworkRepresentation* orgRep = static_cast<_resqml2__SealedSurfaceFrameworkRepresentation*>(gsoapProxy2_0_1);

	if (contactIndex >= orgRep->SealedContactRepresentation.size()) {
		throw invalid_argument("Invalid contact index.");
	}

    resqml2__SealedContactRepresentationPart* contactRep = static_cast<resqml2__SealedContactRepresentationPart*>(orgRep->SealedContactRepresentation[contactIndex]);

    // we look for the supporting representation index
    int representationIndex = -1;
	const unsigned int representationCount = getRepresentationCount();
	for (unsigned int i = 0; i < representationCount; ++i) {
		if (getRepresentation(i)->getUuid() == supportingRepresentation->getUuid()) {
			representationIndex = i;
			break;
		}
	}
	if (representationIndex == -1) {
		throw invalid_argument("The supporting representation is not referenced by the sealed surface framework");
	}

    resqml2__ContactPatch* contactPatch = soap_new_resqml2__ContactPatch(gsoapProxy2_0_1->soap, 1);
    contactPatch->PatchIndex = contactRep->Contact.size();
    contactPatch->Count = NodeCount;
    contactPatch->RepresentationIndex = representationIndex;

    resqml2__IntegerHdf5Array* xmlSupportingRepresentationNodes = soap_new_resqml2__IntegerHdf5Array(gsoapProxy2_0_1->soap, 1);
    xmlSupportingRepresentationNodes->NullValue = (std::numeric_limits<int>::max)();
    xmlSupportingRepresentationNodes->Values = soap_new_eml20__Hdf5Dataset(gsoapProxy2_0_1->soap, 1);
    xmlSupportingRepresentationNodes->Values->HdfProxy = proxy->newResqmlReference();
    ostringstream ossForHdf;
    ossForHdf << "SupportingRepresentationNodes_contact" << contactIndex << "_patch" << contactPatch->PatchIndex;
    xmlSupportingRepresentationNodes->Values->PathInHdfFile = "/RESQML/" + gsoapProxy2_0_1->uuid + "/" + ossForHdf.str();
    contactPatch->SupportingRepresentationNodes = xmlSupportingRepresentationNodes;
    // ************ HDF *************
    hsize_t dim[1] = {NodeCount};
    proxy->writeArrayNd(gsoapProxy2_0_1->uuid,
                        ossForHdf.str(), H5T_NATIVE_UINT,
                        nodeIndicesOnSupportingRepresentation,
                        dim, 1);

    // adding the contact patch to the contact representation
    contactRep->Contact.push_back(contactPatch);
}

std::string SealedSurfaceFrameworkRepresentation::getHdfProxyUuid() const
{
    string result;
    _resqml2__SealedSurfaceFrameworkRepresentation* orgRep = static_cast<_resqml2__SealedSurfaceFrameworkRepresentation*>(gsoapProxy2_0_1);

    if (!orgRep->SealedContactRepresentation.empty() && static_cast<resqml2__SealedContactRepresentationPart*>(orgRep->SealedContactRepresentation[0])->IdenticalNodeIndices != nullptr)
    {
        resqml2__SealedContactRepresentationPart *sealedContactRep = static_cast<resqml2__SealedContactRepresentationPart*>(orgRep->SealedContactRepresentation[0]);
        result = static_cast<resqml2__IntegerHdf5Array *>(sealedContactRep->IdenticalNodeIndices)->Values->HdfProxy->UUID;
    }

    return result;
}

unsigned int SealedSurfaceFrameworkRepresentation::getContactRepCount() const
{
	_resqml2__SealedSurfaceFrameworkRepresentation* orgRep = static_cast<_resqml2__SealedSurfaceFrameworkRepresentation*>(gsoapProxy2_0_1);

	if (orgRep->SealedContactRepresentation.size() > (std::numeric_limits<unsigned int>::max)()) {
		throw range_error("There are too much contact representations for fesapi");
	}

	return static_cast<unsigned int>(orgRep->SealedContactRepresentation.size());
}

gsoap_resqml2_0_1::resqml2__SealedContactRepresentationPart* SealedSurfaceFrameworkRepresentation::getContactRepresentation(unsigned int crIndex) const
{
	if (crIndex >= getContactRepCount()) {
		throw range_error("The index of the contact represenation is out of range.");
	}

	return static_cast<_resqml2__SealedSurfaceFrameworkRepresentation*>(gsoapProxy2_0_1)->SealedContactRepresentation[crIndex];
}

gsoap_resqml2_0_1::resqml2__IdentityKind SealedSurfaceFrameworkRepresentation::getContactPatchIdentityKind(unsigned int crIndex) const
{
	return getContactRepresentation(crIndex)->IdentityKind;
}

bool SealedSurfaceFrameworkRepresentation::areAllContactPatchNodesIdenticalInContactRep(unsigned int crIndex) const
{
	return getContactRepresentation(crIndex)->IdenticalNodeIndices == nullptr;
}

unsigned int SealedSurfaceFrameworkRepresentation::getIdenticalNodeCountOfContactRep(unsigned int crIndex) const
{
	if (areAllContactPatchNodesIdenticalInContactRep(crIndex)) {
		throw invalid_argument("The nodes are all identical");
	}

	ULONG64 result = getCountOfIntegerArray(getContactRepresentation(crIndex)->IdenticalNodeIndices);
	if (result > (std::numeric_limits<unsigned int>::max)()) {
		throw range_error("There are too much identical nodes for fesapi");
	}

	return static_cast<unsigned int>(result);
}

void SealedSurfaceFrameworkRepresentation::getIdenticalNodeIndicesOfContactRep(unsigned int crIndex, unsigned int * nodeIndices) const
{
	if (areAllContactPatchNodesIdenticalInContactRep(crIndex)) {
		throw invalid_argument("The nodes are all identical");
	}

	readArrayNdOfUIntValues(getContactRepresentation(crIndex)->IdenticalNodeIndices, nodeIndices);
}

unsigned int SealedSurfaceFrameworkRepresentation::getContactPatchCount(unsigned int crIndex) const
{
	resqml2__SealedContactRepresentationPart* contactRep = getContactRepresentation(crIndex);

	if (contactRep->Contact.size() > (std::numeric_limits<unsigned int>::max)()) {
		throw range_error("There are too much contact patches for fesapi");
	}

	return static_cast<unsigned int>(contactRep->Contact.size());
}

gsoap_resqml2_0_1::resqml2__ContactPatch* SealedSurfaceFrameworkRepresentation::getContactPatch(unsigned int crIndex, unsigned int cpIndex) const
{
	resqml2__SealedContactRepresentationPart* contactRep = getContactRepresentation(crIndex);

	if (cpIndex >= getContactPatchCount(crIndex)) {
		throw range_error("The index of the contact patch is out of range.");
	}

	return contactRep->Contact[cpIndex];
}

RESQML2_NS::AbstractRepresentation* SealedSurfaceFrameworkRepresentation::getRepresentationOfContactPatch(unsigned int crIndex, unsigned int cpIndex) const
{
	return getRepresentation(getContactPatch(crIndex, cpIndex)->RepresentationIndex);
}

unsigned int SealedSurfaceFrameworkRepresentation::getNodeCountOfContactPatch(unsigned int crIndex, unsigned int cpIndex) const
{
	ULONG64 result = getCountOfIntegerArray(getContactPatch(crIndex, cpIndex)->SupportingRepresentationNodes);
	if (result > (std::numeric_limits<unsigned int>::max)()) {
		throw range_error("There are too much nodes in this contact patch for fesapi");
	}

	return static_cast<unsigned int>(result);
}

void SealedSurfaceFrameworkRepresentation::getNodeIndicesOfContactPatch(unsigned int crIndex, unsigned int cpIndex, unsigned int * nodeIndices) const
{
	readArrayNdOfUIntValues(getContactPatch(crIndex, cpIndex)->SupportingRepresentationNodes, nodeIndices);
}

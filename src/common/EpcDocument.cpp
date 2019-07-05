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
#include "common/EpcDocument.h"

#include <sstream>
#include <stdexcept>

#include "H5Epublic.h"
#include "H5Fpublic.h"

#include "version_config.h"

#include "epc/Relationship.h"
#include "epc/FilePart.h"

#include "resqml2_0_1/PropertyKindMapper.h"

#include "resqml2_0_1/LocalDepth3dCrs.h"
#include "resqml2_0_1/LocalTime3dCrs.h"
#include "resqml2_0_1/Horizon.h"
#include "resqml2_0_1/FluidBoundaryFeature.h"
#include "resqml2_0_1/TectonicBoundaryFeature.h"
#include "resqml2_0_1/FrontierFeature.h"
#include "resqml2_0_1/GeobodyFeature.h"
#include "resqml2_0_1/GenericFeatureInterpretation.h"
#include "resqml2_0_1/FaultInterpretation.h"
#include "resqml2_0_1/HorizonInterpretation.h"
#include "resqml2_0_1/GeobodyBoundaryInterpretation.h"
#include "resqml2_0_1/GeobodyInterpretation.h"
#include "resqml2_0_1/PolylineSetRepresentation.h"
#include "resqml2_0_1/PointSetRepresentation.h"
#include "resqml2_0_1/PlaneSetRepresentation.h"
#include "resqml2_0_1/SeismicLatticeFeature.h"
#include "resqml2_0_1/Grid2dRepresentation.h"
#include "resqml2_0_1/HdfProxy.h"
#include "resqml2_0_1/TriangulatedSetRepresentation.h"
#include "resqml2_0_1/WellboreFeature.h"
#include "resqml2_0_1/WellboreInterpretation.h"
#include "resqml2_0_1/WellboreMarkerFrameRepresentation.h"
#include "resqml2_0_1/WellboreTrajectoryRepresentation.h"
#include "resqml2_0_1/DeviationSurveyRepresentation.h"
#include "resqml2_0_1/MdDatum.h"
#include "resqml2_0_1/PolylineRepresentation.h"
#include "resqml2_0_1/SubRepresentation.h"
#include "resqml2_0_1/GridConnectionSetRepresentation.h"
#include "resqml2_0_1/TimeSeries.h"
#include "resqml2_0_1/PropertyKind.h"
#include "resqml2_0_1/ContinuousProperty.h"
#include "resqml2_0_1/CategoricalProperty.h"
#include "resqml2_0_1/DiscreteProperty.h"
#include "resqml2_0_1/CommentProperty.h"
#include "resqml2_0_1/StringTableLookup.h"
#include "resqml2_0_1/SeismicLineFeature.h"
#include "resqml2_0_1/SeismicLineSetFeature.h"
#include "resqml2_0_1/OrganizationFeature.h"

#include "resqml2_0_1/BlockedWellboreRepresentation.h"

#include "resqml2_0_1/EarthModelInterpretation.h"
#include "resqml2_0_1/RepresentationSetRepresentation.h"
#include "resqml2_0_1/StructuralOrganizationInterpretation.h"
#include "resqml2_0_1/NonSealedSurfaceFrameworkRepresentation.h"
#include "resqml2_0_1/SealedSurfaceFrameworkRepresentation.h"
#include "resqml2_0_1/SealedVolumeFrameworkRepresentation.h"

#include "resqml2_0_1/RockFluidUnitFeature.h"
#include "resqml2_0_1/RockFluidUnitInterpretation.h"
#include "resqml2_0_1/RockFluidOrganizationInterpretation.h"

#include "resqml2_0_1/StratigraphicUnitFeature.h"
#include "resqml2_0_1/StratigraphicUnitInterpretation.h"
#include "resqml2_0_1/StratigraphicColumn.h"
#include "resqml2_0_1/StratigraphicColumnRankInterpretation.h"
#include "resqml2_0_1/StratigraphicOccurrenceInterpretation.h"

#include "resqml2_0_1/IjkGridExplicitRepresentation.h"
#include "resqml2_0_1/IjkGridParametricRepresentation.h"
#include "resqml2_0_1/IjkGridLatticeRepresentation.h"
#include "resqml2_0_1/IjkGridNoGeometryRepresentation.h"
#include "resqml2_0_1/UnstructuredGridRepresentation.h"

#include "resqml2_0_1/Activity.h"
#include "resqml2_0_1/ActivityTemplate.h"
#include "resqml2_0_1/ContinuousPropertySeries.h"
#include "resqml2_0_1/CategoricalPropertySeries.h"
#include "resqml2_0_1/DiscretePropertySeries.h"

#include "witsml2_0/Well.h"

#include "tools/GuidTools.h"

using namespace std;
using namespace epc;
using namespace gsoap_resqml2_0_1;
using namespace COMMON_NS;
using namespace RESQML2_0_1_NS;
using namespace WITSML2_0_NS;

const char* EpcDocument::DOCUMENT_EXTENSION = ".epc";

EpcDocument::EpcDocument(const string& fileName, DataObjectRepository::openingMode hdfPermissionAccess_) :
	package(nullptr), hdfPermissionAccess(hdfPermissionAccess_)
{
	open(fileName);
}

EpcDocument::~EpcDocument()
{
	close();
}

void  EpcDocument::open(const std::string & fileName)
{
	if (fileName.empty()) {
		throw invalid_argument("The epc document name cannot be empty.");
	}
	if (package != nullptr) {
		throw invalid_argument("The epc document must be closed before to be opened again.");
	}

	setFilePath(fileName);

	package = new Package();
}

void EpcDocument::close()
{
	if (package != nullptr) {
		delete package;
		package = nullptr;
	}

	filePath = "";
}

void EpcDocument::setFilePath(const std::string & fp)
{
	filePath = fp;

	// Turn off HDF5 diagnostic messages
	herr_t hdf5Err = H5Eset_auto(H5E_DEFAULT, nullptr, nullptr);
	if (hdf5Err < 0) {
		throw invalid_argument("The HDF5 error handling could not have been disabled.");
	}

	// Add .epc extension if it is not already done in parameter
	size_t dotPos = filePath.find_last_of('.');
	if (dotPos != string::npos) {
		if (filePath.substr(dotPos) != DOCUMENT_EXTENSION) {
			filePath += DOCUMENT_EXTENSION;
		}
	}
	else {
		filePath += DOCUMENT_EXTENSION;
	}
}

namespace {
	std::vector<epc::Relationship> getAllEpcRelationships(const DataObjectRepository & repo, COMMON_NS::AbstractObject const * dataObj) {
		std::vector<epc::Relationship> result;

		const std::vector<COMMON_NS::AbstractObject const *>& srcObj = repo.getSourceObjects(dataObj);
		for (size_t index = 0; index < srcObj.size(); ++index) {
			Relationship relRep(srcObj[index]->getPartNameInEpcDocument(), "", srcObj[index]->getUuid());
			relRep.setSourceObjectType();
			result.push_back(relRep);
		}

		const std::vector<COMMON_NS::AbstractObject const *>& targetObj = repo.getTargetObjects(dataObj);
		for (size_t index = 0; index < targetObj.size(); ++index) {
			Relationship relRep(targetObj[index]->getPartNameInEpcDocument(), "", targetObj[index]->getUuid());
			relRep.setDestinationObjectType();
			result.push_back(relRep);
		}

		// External part
		COMMON_NS::AbstractHdfProxy const * hdfProxy = dynamic_cast<COMMON_NS::AbstractHdfProxy const *>(dataObj);
		if (hdfProxy != nullptr) {
			epc::Relationship relExt(hdfProxy->getRelativePath(), "", "Hdf5File", false);
			relExt.setExternalResourceType();
			result.push_back(relExt);
		}

		return result;
	}
}

void EpcDocument::serializeFrom(const DataObjectRepository & repo, bool useZip64)
{
	// Cannot include zip.h for some macro conflict reasons with beast which also includes a port of zlib. Consequently cannot use macros below.
	// 0 means APPEND_STATUS_CREATE
	package->openForWriting(filePath, 0, useZip64);

	const std::unordered_map< std::string, std::vector< COMMON_NS::AbstractObject* > > dataObjects = repo.getDataObjects();
	for (std::unordered_map< std::string, std::vector< COMMON_NS::AbstractObject* > >::const_iterator it = dataObjects.begin(); it != dataObjects.end(); ++it)
	{
		for (size_t i = 0; i < it->second.size(); ++i) {
			if (!it->second[i]->isPartial() && dynamic_cast<WellboreMarker*>(it->second[i]) == nullptr) {
				const string str = it->second[i]->serializeIntoString();

				epc::FilePart* const fp = package->createPart(str, it->second[i]->getPartNameInEpcDocument());
				const std::vector<epc::Relationship>& relSet = getAllEpcRelationships(repo, it->second[i]);
				for (size_t relIndex = 0; relIndex < relSet.size(); ++relIndex) {
					fp->addRelationship(relSet[relIndex]);
				}

				package->addContentType(epc::ContentType(false, it->second[i]->getContentType(), it->second[i]->getPartNameInEpcDocument()));
			}
		}
	}

	package->writePackage();
}

string EpcDocument::deserializeInto(DataObjectRepository & repo)
{
	std::string result;
	vector<string> warningsDuringReading = package->openForReading(filePath);
	for (size_t i = 0; i < warningsDuringReading.size(); ++i) {
		result += warningsDuringReading[i] + "\n";
	}

	// Read all RESQML objects
	const FileContentType::ContentTypeMap contentTypes = package->getFileContentType().getAllContentType();
	// 14 equals "application/x-".size()
	for (FileContentType::ContentTypeMap::const_iterator it=contentTypes.begin(); it != contentTypes.end(); ++it)
	{
		std::string contentType = it->second.getContentTypeString();
		if (contentType.find("resqml", 14) != std::string::npos ||
			contentType.find("eml", 14) != std::string::npos ||
			contentType.find("witsml", 14) != std::string::npos)
		{
			if (contentType == "application/x-resqml+xml;version=2.0;type=obj_EpcExternalPartReference") {
				result += "The content type " + contentType + " should belong to eml and not to resqml since obj_EpcExternalPartReference is part of COMMON and not part of RESQML.\n";
				contentType = "application/x-eml+xml;version=2.0;type=obj_EpcExternalPartReference";
			}
			COMMON_NS::AbstractObject* wrapper = repo.addOrReplaceGsoapProxy(package->extractFile(it->second.getExtensionOrPartName().substr(1)), it->second.getContentTypeString());
			if (it->second.getContentTypeString() == "application/x-eml+xml;version=2.0;type=obj_EpcExternalPartReference") {
				static_cast<RESQML2_0_1_NS::HdfProxy*>(wrapper)->setRootPath(getStorageDirectory());
				static_cast<RESQML2_0_1_NS::HdfProxy*>(wrapper)->setOpeningMode(hdfPermissionAccess);

				// Look for the relative path of the HDF file
				string relFilePath = "";
				const size_t slashPos = it->second.getExtensionOrPartName().substr(1).find_last_of("/\\");
				if (slashPos != string::npos) {
					relFilePath = it->second.getExtensionOrPartName().substr(1).substr(0, slashPos + 1);
				}
				relFilePath += "_rels" + it->second.getExtensionOrPartName().substr(it->second.getExtensionOrPartName().find_last_of("/\\")) + ".rels";
				if (!package->fileExists(relFilePath)) {
					result += "The HDF proxy " + it->second.getExtensionOrPartName() + " does not look to be associated to any HDF files : there is no rel file for this object. It is going to be withdrawn.\n";
					continue;
				}
				FileRelationship relFile;
				relFile.readFromString(package->extractFile(relFilePath));
				const vector<Relationship> allRels = relFile.getAllRelationship();
				for (size_t relIndex = 0; relIndex < allRels.size(); ++relIndex) {
					if (allRels[relIndex].getType().compare("http://schemas.energistics.org/package/2012/relationships/externalResource") == 0) {
						static_cast<RESQML2_0_1_NS::HdfProxy*>(wrapper)->setRelativePath(allRels[relIndex].getTarget());
						break;
					}
				}
			}
		}
	}

	repo.updateAllRelationships();

	// Validate properties
	const vector<RESQML2_NS::AbstractProperty*> allprops = repo.getDataObjects<RESQML2_NS::AbstractProperty>();
	for (size_t propIndex = 0; propIndex < allprops.size(); ++propIndex) {
		allprops[propIndex]->validate();
	}

	return result;
}

std::string EpcDocument::deserializePartiallyInto(DataObjectRepository & repo)
{
	std::string result;
	vector<string> warningsDuringReading = package->openForReading(filePath);
	for (size_t i = 0; i < warningsDuringReading.size(); ++i) {
		result += warningsDuringReading[i] + "\n";
	}

	// Read all RESQML objects
	const FileContentType::ContentTypeMap contentTypes = package->getFileContentType().getAllContentType();
	// 14 equals "application/x-".size()
	for (FileContentType::ContentTypeMap::const_iterator it = contentTypes.begin(); it != contentTypes.end(); ++it)
	{
		std::string contentType = it->second.getContentTypeString();
		if (contentType.find("resqml", 14) != std::string::npos ||
			contentType.find("eml", 14) != std::string::npos ||
			contentType.find("witsml", 14) != std::string::npos)
		{
			if (contentType == "application/x-resqml+xml;version=2.0;type=obj_EpcExternalPartReference") {
				result += "The content type " + contentType + " should belong to eml and not to resqml since obj_EpcExternalPartReference is part of COMMON and not part of RESQML.\n";
				contentType = "application/x-eml+xml;version=2.0;type=obj_EpcExternalPartReference";
			}
			COMMON_NS::AbstractObject* wrapper = repo.createPartial(it->first.substr(it->first.size() - 40, 36), "Unknown Title", contentType);
			if (it->second.getContentTypeString() == "application/x-eml+xml;version=2.0;type=obj_EpcExternalPartReference") {
				static_cast<RESQML2_0_1_NS::HdfProxy*>(wrapper)->setRootPath(getStorageDirectory());
				static_cast<RESQML2_0_1_NS::HdfProxy*>(wrapper)->setOpeningMode(hdfPermissionAccess);

				// Look for the relative path of the HDF file
				string relFilePath = "";
				const size_t slashPos = it->second.getExtensionOrPartName().substr(1).find_last_of("/\\");
				if (slashPos != string::npos) {
					relFilePath = it->second.getExtensionOrPartName().substr(1).substr(0, slashPos + 1);
				}
				relFilePath += "_rels" + it->second.getExtensionOrPartName().substr(it->second.getExtensionOrPartName().find_last_of("/\\")) + ".rels";
				if (!package->fileExists(relFilePath)) {
					result += "The HDF proxy " + it->second.getExtensionOrPartName() + " does not look to be associated to any HDF files : there is no rel file for this object. It is going to be withdrawn.\n";
					continue;
				}
				FileRelationship relFile;
				relFile.readFromString(package->extractFile(relFilePath));
				const vector<Relationship> allRels = relFile.getAllRelationship();
				for (size_t relIndex = 0; relIndex < allRels.size(); ++relIndex) {
					if (allRels[relIndex].getType().compare("http://schemas.energistics.org/package/2012/relationships/externalResource") == 0) {
						static_cast<RESQML2_0_1_NS::HdfProxy*>(wrapper)->setRelativePath(allRels[relIndex].getTarget());
						break;
					}
				}
			}
		}
	}

	repo.updateAllRelationships();

	// Validate properties
	const vector<RESQML2_NS::AbstractProperty*> allprops = repo.getDataObjects<RESQML2_NS::AbstractProperty>();
	for (size_t propIndex = 0; propIndex < allprops.size(); ++propIndex) {
		allprops[propIndex]->validate();
	}

	return result;
}

string EpcDocument::getStorageDirectory() const
{
	const size_t slashPos = filePath.find_last_of("/\\");
	return slashPos != string::npos ? filePath.substr(0, slashPos + 1) : string();
}

string EpcDocument::getName() const
{
	// Remove the directories from the file path
	const size_t slashPos = filePath.find_last_of("/\\");
	const string nameSuffixed = slashPos != string::npos ? filePath.substr(slashPos + 1, filePath.size()) : filePath;

	// Remove the extension
	return nameSuffixed.substr(0, nameSuffixed.find_last_of("."));
}

unordered_map< string, string > & EpcDocument::getExtendedCoreProperty()
{
	return package->getExtendedCoreProperty();
}

void EpcDocument::setExtendedCoreProperty(const std::string & key, const std::string & value)
{
	(package->getExtendedCoreProperty())[key] = value;
}

std::string EpcDocument::getExtendedCoreProperty(const std::string & key)
{
	if (package->getExtendedCoreProperty().find(key) != package->getExtendedCoreProperty().end()) {
		return (package->getExtendedCoreProperty())[key];
	}

	return string();
}

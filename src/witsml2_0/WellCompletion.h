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

#include "AbstractObject.h"

namespace WITSML2_0_NS
{
	class WellboreCompletion;

	class WellCompletion : public WITSML2_0_NS::AbstractObject
	{
	public:

		/**
		* Only to be used in partial transfer context
		*/
		DLL_IMPORT_OR_EXPORT WellCompletion(gsoap_resqml2_0_1::eml20__DataObjectReference* partialObject) : WITSML2_0_NS::AbstractObject(partialObject) {}

		/**
		* Constructor
		*/
		WellCompletion(class Well* witsmlWell,
			const std::string & guid,
			const std::string & title);

		/**
		* Creates an instance of this class by wrapping a gsoap instance.
		*/
		WellCompletion(gsoap_eml2_1::witsml20__WellCompletion* fromGsoap) :AbstractObject(fromGsoap) {}

		/**
		* Destructor does nothing since the memory is managed by the gsoap context.
		*/
		~WellCompletion() {}

		/**
		* Get the Data Object Reference of the well linked with this data object.
		*/
		gsoap_eml2_1::eml21__DataObjectReference* getWellDor() const;
		
		/**
		* Get the well of this well completion.
		*/
		DLL_IMPORT_OR_EXPORT class Well* getWell() const;

		/**
		* Set the well of this well completion.
		*/
		DLL_IMPORT_OR_EXPORT void setWell(class Well* witsmlWell);

		/**
		* Get all welbore completions linked with this well completion.
		*/
		DLL_IMPORT_OR_EXPORT std::vector<WellboreCompletion *> getWellboreCompletions() const;

		/**
		* The standard XML tag without XML namespace for serializing this data object.
		*/
		DLL_IMPORT_OR_EXPORT static const char* XML_TAG;

		/**
		* Get the standard XML tag without XML namespace for serializing this data object.
		*/
		DLL_IMPORT_OR_EXPORT virtual std::string getXmlTag() const { return XML_TAG; }

	protected:
		/**
		* Resolve all relationships of the object in the repository.
		*/
		void loadTargetRelationships();
	};
}

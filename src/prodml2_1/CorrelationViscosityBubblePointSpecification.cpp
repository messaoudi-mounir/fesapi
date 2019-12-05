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
#include "CorrelationViscosityBubblePointSpecification.h"

using namespace PRODML2_1_NS;
using namespace gsoap_eml2_2;

SETTER_MEASURE_ATTRIBUTE_IMPL(CorrelationViscosityBubblePointSpecification, gsoap_eml2_2::prodml21__AbstractCorrelationViscosityBubblePointModel, gsoapProxy, DeadOilViscosity, gsoap_eml2_2::eml22__DynamicViscosityUom, gsoap_eml2_2::soap_new_eml22__DynamicViscosityMeasure)
SETTER_MEASURE_ATTRIBUTE_IMPL(CorrelationViscosityBubblePointSpecification, gsoap_eml2_2::prodml21__AbstractCorrelationViscosityBubblePointModel, gsoapProxy, BubblePointOilViscosity, gsoap_eml2_2::eml22__DynamicViscosityUom, gsoap_eml2_2::soap_new_eml22__DynamicViscosityMeasure)
SETTER_MEASURE_ATTRIBUTE_IMPL(CorrelationViscosityBubblePointSpecification, gsoap_eml2_2::prodml21__AbstractCorrelationViscosityBubblePointModel, gsoapProxy, SolutionGasOilRatio, std::string, gsoap_eml2_2::soap_new_eml22__VolumePerVolumeMeasure)

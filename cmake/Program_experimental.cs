﻿using System;

using f2i.energisticsStandardsApi;
using f2i.energisticsStandardsApi.${FESAPI_COMMON_NS};
using f2i.energisticsStandardsApi.${FESAPI_RESQML2_NS};
using f2i.energisticsStandardsApi.${FESAPI_RESQML2_0_1_NS};
using f2i.energisticsStandardsApi.${FESAPI_RESQML2_2_NS};
using f2i.energisticsStandardsApi.${FESAPI_WITSML2_0_NS};

// Add the fesapi cpp and hdf5, szip, zlib dll into the executable directory.

namespace example
{
    static class Program
    {
        private static void serialize()
        {
            using (EpcDocument epc_file = new EpcDocument("../../../TestingPackageCs.epc"))
            using (DataObjectRepository repo = new DataObjectRepository())
            {
                LocalDepth3dCrs crs = repo.createLocalDepth3dCrs(Guid.NewGuid().ToString(), "UTF8 Crs title : éàç : олег1", 0.0, 0.0, 0.0, 0.0, eml20__LengthUom.eml20__LengthUom__m, 5215, eml20__LengthUom.eml20__LengthUom__m, "Unknown", false);
                repo.setDefaultCrs(crs);
                System.Console.WriteLine("Serialize : CRS title is " + crs.getTitle());
                Well well = repo.createWell("1425632e-3c22-4845-b431-ecd36da0671e", "Well");
                well.setNameLegal("Legal Name");
                well.setWaterDepth(0.0, eml21__LengthUom.eml21__LengthUom__0_x002e1_x0020ft);
                well.setTimeZone(true, 0);
                System.Console.WriteLine("Serialize : Well title is " + well.getTitle());

                // HdfProxy
                AbstractHdfProxy hdfProxy = repo.createHdfProxy("", "Hdf Proxy", epc_file.getStorageDirectory(), epc_file.getName() + ".h5", DataObjectRepository.openingMode.OVERWRITE);
                repo.setDefaultHdfProxy(hdfProxy);

                // Features
                WellboreFeature wellbore1 = repo.createWellboreFeature("22d5b48f-f789-46e7-a454-6d8bd05afd0b", "Wellbore1");

                // Interpretations
                WellboreInterpretation wellbore1Interp1 = repo.createWellboreInterpretation(wellbore1, "dc7840fe-e5a3-4b53-a1df-18040bc4d0c0", "Wellbore1 Interp1", false);

                // Representation
                f2i.energisticsStandardsApi.${FESAPI_RESQML2_NS}.MdDatum mdInfo = repo.createMdDatum("36e91de5-7833-4b6d-90d0-1d643c0adece", "md Info", repo.getDefaultCrs(), resqml20__MdReference.resqml20__MdReference__mean_x0020sea_x0020level, 275, 75, 0);

                //Geometry	
                WellboreTrajectoryRepresentation w1i1TrajRep = repo.createWellboreTrajectoryRepresentation(wellbore1Interp1, "acd2cdcf-bb5d-48da-bd0e-9aeff3e52180", "Wellbore1 Interp1 TrajRep", mdInfo);
                DoubleArray controlPoints = new DoubleArray(12);
                controlPoints.setitem(0, 275);
                controlPoints.setitem(1, 75);
                controlPoints.setitem(2, 0);
                controlPoints.setitem(3, 275);
                controlPoints.setitem(4, 75);
                controlPoints.setitem(5, 325);
                controlPoints.setitem(6, 275);
                controlPoints.setitem(7, 75);
                controlPoints.setitem(8, 500);
                controlPoints.setitem(9, 275);
                controlPoints.setitem(10, 75);
                controlPoints.setitem(11, 1000);
                DoubleArray trajectoryTangentVectors = new DoubleArray(12);
                trajectoryTangentVectors.setitem(0, 0);
                trajectoryTangentVectors.setitem(1, 0);
                trajectoryTangentVectors.setitem(2, 1);
                trajectoryTangentVectors.setitem(3, 0);
                trajectoryTangentVectors.setitem(4, 0);
                trajectoryTangentVectors.setitem(5, 1);
                trajectoryTangentVectors.setitem(6, 0);
                trajectoryTangentVectors.setitem(7, 0);
                trajectoryTangentVectors.setitem(8, 1);
                trajectoryTangentVectors.setitem(9, 0);
                trajectoryTangentVectors.setitem(10, 0);
                trajectoryTangentVectors.setitem(11, 1);
                DoubleArray trajectoryMds = new DoubleArray(4);
                trajectoryMds.setitem(0, 0);
                trajectoryMds.setitem(1, 325);
                trajectoryMds.setitem(2, 500);
                trajectoryMds.setitem(3, 1000);
                w1i1TrajRep.setGeometry(controlPoints.cast(), trajectoryTangentVectors.cast(), trajectoryMds.cast(), 4, 0, hdfProxy);

                // WellboreFeature frame
                f2i.energisticsStandardsApi.${FESAPI_RESQML2_NS}.WellboreFrameRepresentation w1i1FrameRep = repo.createWellboreFrameRepresentation(wellbore1Interp1, "d873e243-d893-41ab-9a3e-d20b851c099f", "Wellbore1 Interp1 FrameRep", w1i1TrajRep);
                DoubleArray logMds = new DoubleArray(5);
                logMds.setitem(0, 0);
                logMds.setitem(1, 250);
                logMds.setitem(2, 500);
                logMds.setitem(3, 750);
                logMds.setitem(4, 1000);
                w1i1FrameRep.setMdValues(logMds.cast(), 5, hdfProxy);

                f2i.energisticsStandardsApi.${FESAPI_RESQML2_NS}.WellboreFrameRepresentation w1i1RegularFrameRep = repo.createWellboreFrameRepresentation(wellbore1Interp1, "a54b8399-d3ba-4d4b-b215-8d4f8f537e66", "Wellbore1 Interp1 Regular FrameRep", w1i1TrajRep);
                w1i1RegularFrameRep.setMdValues(0, 200, 6);

                // WellboreFeature seismic frame
                LocalTime3dCrs localTime3dCrs = repo.createLocalTime3dCrs("", "Default local time CRS", 1.0, 0.1, 15, .0, eml20__LengthUom.eml20__LengthUom__m, 23031, eml20__TimeUom.eml20__TimeUom__s, eml20__LengthUom.eml20__LengthUom__m, "Unknown", false);

                SeismicWellboreFrameRepresentation w1i1SeismicFrameRep = repo.createSeismicWellboreFrameRepresentation(
                    wellbore1Interp1, "dcbeea2e-8327-4c5b-97e3-bdced0680de5", "Wellbore1 Interp1 SeismicFrameRep",
                    w1i1TrajRep,
                    0,
                    0,
                    localTime3dCrs);
                w1i1SeismicFrameRep.setMdValues(logMds.cast(), 5, hdfProxy);
                DoubleArray logTimes = new DoubleArray(5);
                logTimes.setitem(0, 0);
                logTimes.setitem(1, 10);
                logTimes.setitem(2, 20);
                logTimes.setitem(3, 25);
                logTimes.setitem(4, 30);
                w1i1SeismicFrameRep.setTimeValues(logTimes.cast(), 5, hdfProxy);

                SeismicWellboreFrameRepresentation w1i1RegularSeismicFrameRep = repo.createSeismicWellboreFrameRepresentation(
                    wellbore1Interp1, "7f1b75ff-1226-4c0a-a531-8f71661da419", "Wellbore1 Interp1 Regular SeismicFrameRep",
                    w1i1TrajRep,
                    0,
                    0,
                    localTime3dCrs);
                w1i1RegularSeismicFrameRep.setMdValues(0, 200, 6);
                w1i1RegularSeismicFrameRep.setTimeValues(0, 10, 6);

                epc_file.serializeFrom(repo);
                epc_file.close();
            }
        }

        private static void deserialize()
        {
            using (EpcDocument epc_file = new EpcDocument("../../../TestingPackageCs.epc"))
            using (DataObjectRepository repo = new DataObjectRepository())
            {
                string status = epc_file.deserializeInto(repo);
                if (status.Length > 0)
                {
                    System.Console.WriteLine("Warnings are " + status);
                }
                LocalDepth3dCrsVector crs_set = repo.getLocalDepth3dCrsSet();
                System.Console.WriteLine("Deserialize : CRS title is " + crs_set[0].getTitle());
                Well well = repo.getDataObjectByUuid("1425632e-3c22-4845-b431-ecd36da0671e") as Well;
                System.Console.WriteLine("Deserialize : Well title is " + well.getTitle());
                System.Console.WriteLine("\tnameLegal : " + well.getNameLegal());
                System.Console.WriteLine("\twaterDepth : " + well.getWaterDepthValue() + " (" + well.getWaterDepthUom() + ")");
                if (well.hasTimeZone())
                {
                    System.Console.WriteLine("\ttimeZone : " + well.getTimeZoneHours() + " hours");
                }

                WellboreTrajectoryRepresentationVector wellboreCubicTrajSet = repo.getWellboreTrajectoryRepresentationSet();
                foreach (WellboreTrajectoryRepresentation traj in wellboreCubicTrajSet)
                {
                    for (uint wbfIndex = 0; wbfIndex < traj.getWellboreFrameRepresentationCount(); wbfIndex++)
                    {
                        f2i.energisticsStandardsApi.${FESAPI_RESQML2_NS}.WellboreFrameRepresentation wbf = traj.getWellboreFrameRepresentation(wbfIndex);
                        System.Console.WriteLine("Deserialize : WellboreFrameRepresentation title is " + wbf.getTitle());
                        System.Console.WriteLine("Deserialize : WellboreFrameRepresentation uuid is " + wbf.getUuid());
                        System.Console.WriteLine("Value Count : " + wbf.getMdValuesCount());
                        if (wbf.areMdValuesRegularlySpaced())
                        {
                            System.Console.WriteLine("Regularly spaced");
                            System.Console.WriteLine("First Value : " + wbf.getMdFirstValue());
                            System.Console.WriteLine("Increment : " + wbf.getMdConstantIncrementValue());
                        }
                        else
                        {
                            System.Console.WriteLine("Iregularly spaced");
                        }
                        if (wbf.getMdHdfDatatype() == AbstractValuesProperty.hdfDatatypeEnum.DOUBLE)
						{
                            System.Console.WriteLine("Hdf datatype is NATIVE DOUBLE");
						}
                        if (wbf.getMdHdfDatatype() == AbstractValuesProperty.hdfDatatypeEnum.FLOAT)
						{
                            System.Console.WriteLine("Hdf datatype is NATIVE FLOAT");
						}
                        if (wbf.getMdHdfDatatype() == AbstractValuesProperty.hdfDatatypeEnum.UNKNOWN)
						{
                            System.Console.WriteLine("Hdf datatype is NATIVE UNKNOWN");
						}
						if (wbf.getXmlTag() == "SeismicWellboreFrameRepresentation")
						{
							SeismicWellboreFrameRepresentation swbf = (SeismicWellboreFrameRepresentation) wbf;
							
							System.Console.WriteLine("Seismic reference datum : " + swbf.getSeismicReferenceDatum());
							System.Console.WriteLine("Weathering velocity : " + swbf.getWeatheringVelocity());
							if (swbf.areTimeValuesRegularlySpaced())
							{
								System.Console.WriteLine("Time values regularly spaced");
								System.Console.WriteLine("First Value : " + swbf.getTimeFirstValue());
								System.Console.WriteLine("Increment : " + swbf.getTimeConstantIncrementValue());
							}
							else
							{
								System.Console.WriteLine("Time values iregularly spaced");
							}
							if (swbf.getTimeHdfDatatype() == AbstractValuesProperty.hdfDatatypeEnum.DOUBLE)
							{
								System.Console.WriteLine("Hdf datatype is NATIVE DOUBLE");
							}
							else if (swbf.getTimeHdfDatatype() == AbstractValuesProperty.hdfDatatypeEnum.FLOAT)
							{
								System.Console.WriteLine("Hdf datatype is NATIVE FLOAT");
							}
							else if (swbf.getTimeHdfDatatype() == AbstractValuesProperty.hdfDatatypeEnum.UNKNOWN)
							{
								System.Console.WriteLine("Hdf datatype is UNKNOWN");
							}
						}
                    }
                }
            }
        }

        static void Main(string[] args)
        {
            serialize();
            deserialize();
        }
    }
}

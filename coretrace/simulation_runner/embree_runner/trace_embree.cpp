#include "trace_embree.hpp"

#include <algorithm>
#include <cmath>
#include <sstream>
#include <string>
#include <vector>

// Embree header
#include <embree4/rtcore.h>

// SimulationData header
#include <simulation_data_export.hpp>

// SimulationRunner header
#include <simulation_runner.hpp>

// NativeRunner header(s)
#include <generate_ray.hpp>
#include <mtrand.hpp>
#include <native_runner_types.hpp>
#include <process_interaction.hpp>
#include <sun_to_primary_stage.hpp>

#include "embree_helper.hpp"
#include "find_element_hit_embree.hpp"
#include "trace_logger.hpp"

namespace SolTrace::EmbreeRunner
{
    using SolTrace::NativeRunner::GlobalRay_refactored;
    using SolTrace::NativeRunner::MTRand;
    using SolTrace::NativeRunner::TElement;
    using SolTrace::NativeRunner::telement_ptr;
    using SolTrace::NativeRunner::trace_logger_ptr;
    using SolTrace::NativeRunner::tstage_ptr;
    using SolTrace::NativeRunner::TSystem;

    using SolTrace::Runner::RunnerStatus;

    using SolTrace::Result::RayEvent;

    RunnerStatus make_embree_scene(trace_logger_ptr logger,
                                   TSystem *System,
                                   RTCDevice &embree_device,
                                   RTCScene &embree_scene)
    {
        RunnerStatus sts = RunnerStatus::SUCCESS;
        // // Initialize Embree vars
        // RTCDevice embree_device = nullptr;
        // RTCScene embree_scene = nullptr;
        // bool use_shared_embree = false;

        // Make device
        // std::cout << "Making embree device..." << std::endl;
        embree_device = rtcNewDevice(NULL);

        // std::cout << "Setting error function..." << std::endl;
        rtcSetDeviceErrorFunction(embree_device, error_function, NULL);

        // Convert st stages into scene
        // std::cout << "Making scene..." << std::endl;
        embree_scene = make_scene(embree_device, *System);

        // std::cout << "Committing scene..." << std::endl;
        rtcCommitScene(embree_scene);

        // Validate bounds
        RTCError err = rtcGetDeviceError(embree_device);
        if (err != RTC_ERROR_NONE)
        {
            // int asdg = 0;
            // return RunnerStatus::ERROR;
            sts = RunnerStatus::ERROR;
            logger->error_log("Error setting up Embree scene");
        }

        return sts;
    }

    RunnerStatus trace_embree(
        trace_logger_ptr logger,
        TSystem *System,
        unsigned int seed,
        uint_fast64_t NumberOfRays,
        uint_fast64_t MaxNumberOfRays,
        bool IncludeSunShape,
        bool IncludeErrors,
        const RTCScene &embree_scene)
    {

        // std::cout << "Trace embree start..." << std::endl;

        // Initialize Internal State Variables
        MTRand myrng(seed);
        uint_fast64_t RayNumber = 1; // Ray Number of current ray
        bool PreviousStageHasRays = false;
        uint_fast64_t LastRayNumberInPreviousStage = NumberOfRays;

        // Check Inputs
        if (NumberOfRays < 1)
        {
            // System->errlog("invalid number of rays: %d", NumberOfRays);
            return RunnerStatus::ERROR;
        }
        if (System->StageList.size() < 1)
        {
            // System->errlog("no stages defined.");
            return RunnerStatus::ERROR;
        }

        System->RayData.SetUp(1, NumberOfRays);
        System->SunRayCount = 0;

        // std::cout << "Setting up embree stuff..." << std::endl;
        // std::cout << "Embree Scene: " << embree_scene << std::endl;

        // RTCScene embree_scene = static_cast<RTCScene>(embree_scene_shared);

        // // Initialize Embree vars
        // RTCDevice embree_device = nullptr;
        // RTCScene embree_scene = nullptr;
        // bool use_shared_embree = false;

        // if (embree_scene_shared == nullptr)
        // {
        //     // Make device
        //     // std::cout << "Making embree device..." << std::endl;
        //     embree_device = rtcNewDevice(NULL);

        //     // std::cout << "Setting error function..." << std::endl;
        //     rtcSetDeviceErrorFunction(embree_device, error_function, NULL);

        //     // Convert st stages into scene
        //     // std::cout << "Making scene..." << std::endl;
        //     embree_scene = make_scene(embree_device, *System);

        //     // std::cout << "Committing scene..." << std::endl;
        //     rtcCommitScene(embree_scene);

        //     // Validate bounds
        //     RTCError err = rtcGetDeviceError(embree_device);
        //     if (err != RTC_ERROR_NONE)
        //     {
        //         // int asdg = 0;
        //         return RunnerStatus::ERROR;
        //     }
        // }
        // else
        // {
        //     embree_scene = static_cast<RTCScene>(embree_scene_shared);
        //     use_shared_embree = true;
        // }

        // std::cout << "Setting up sun stuff..." << std::endl;

        // Initialize Sun
        double PosSunStage[3] = {0.0, 0.0, 0.0};
        bool status = SolTrace::NativeRunner::SunToPrimaryStage(
            logger, System, System->StageList[0].get(),
            &System->Sun, PosSunStage);
        if (!status)
            return RunnerStatus::ERROR;

        // Define IncomingRays
        std::vector<GlobalRay_refactored> IncomingRays;
        IncomingRays.resize(NumberOfRays);

        // Initialize stage variables
        uint_fast64_t StageDataArrayIndex = 0;
        uint_fast64_t PreviousStageDataArrayIndex = 0;
        uint_fast64_t n_rays_active = NumberOfRays;

        // std::cout << "Starting ray tracing..." << std::endl;

        // Loop through stages
        for (uint_fast64_t i = 0; i < System->StageList.size(); i++)
        {
            // Check if previous stage has rays
            bool StageHasRays = true;
            if (i > 0 && PreviousStageHasRays == false)
            {
                StageHasRays = false;
            }

            // Get Current Stage
            tstage_ptr Stage = System->StageList[i];

            // Initialize stage variables
            StageDataArrayIndex = 0;
            PreviousStageDataArrayIndex = 0;

            // Loop through rays
            while (StageHasRays)
            {
                // Initialize Global Coordinates
                double PosRayGlob[3] = {0.0, 0.0, 0.0};
                double CosRayGlob[3] = {0.0, 0.0, 0.0};

                // Initialize Stage Coordinates
                double PosRayStage[3] = {0.0, 0.0, 0.0};
                double CosRayStage[3] = {0.0, 0.0, 0.0};

                // Get Ray
                if (i == 0)
                {
                    // Make ray (if first stage)
                    double PosRaySun[3];
                    SolTrace::NativeRunner::GenerateRay(
                        myrng, PosSunStage, Stage->Origin,
                        Stage->RLocToRef, &System->Sun,
                        PosRayGlob, CosRayGlob, PosRaySun);
                    System->SunRayCount++;
                }
                else
                {
                    // Get ray from previous stage
                    RayNumber = IncomingRays[StageDataArrayIndex].Num;
                    CopyVec3(PosRayGlob, IncomingRays[StageDataArrayIndex].Pos);
                    CopyVec3(CosRayGlob, IncomingRays[StageDataArrayIndex].Cos);
                    StageDataArrayIndex++;
                }

                // transform the global incoming ray to local stage coordinates
                TransformToLocal(PosRayGlob, CosRayGlob,
                                 Stage->Origin, Stage->RRefToLoc,
                                 PosRayStage, CosRayStage);

                // Initialize internal variables for ray intersection tracing
                bool RayInStage = true;
                bool in_multi_hit_loop = false;
                double LastPosRaySurfElement[3] = {0.0, 0.0, 0.0};
                double LastCosRaySurfElement[3] = {0.0, 0.0, 0.0};
                double LastPosRaySurfStage[3] = {0.0, 0.0, 0.0};
                double LastCosRaySurfStage[3] = {0.0, 0.0, 0.0};
                double LastDFXYZ[3] = {0.0, 0.0, 0.0};
                uint_fast64_t LastElementNumber = 0;
                uint_fast64_t LastRayNumber = 0;
                int ErrorFlag;
                int LastHitBackSide;
                bool StageHit;
                int MultipleHitCount = 0;
                double PosRayOutElement[3] = {0.0, 0.0, 0.0};
                double CosRayOutElement[3] = {0.0, 0.0, 0.0};

                // Start Loop to trace ray until it leaves stage
                bool RayIsAbsorbed = false;
                while (RayInStage)
                {

                    FindElementHit(embree_scene, i, RayNumber,
                                   PosRayGlob, CosRayGlob,
                                   LastPosRaySurfElement, LastCosRaySurfElement,
                                   LastDFXYZ, LastElementNumber, LastRayNumber,
                                   LastPosRaySurfStage, LastCosRaySurfStage,
                                   ErrorFlag, LastHitBackSide, StageHit);

                    // Breakout if ray left stage
                    if (!StageHit)
                    {
                        RayInStage = false;
                        break;
                    }

                    // Increment MultipleHitCount?
                    MultipleHitCount++;

                    if (i == 0 && MultipleHitCount == 1)
                    {
                        // Add ray to Stage RayData
                        auto r = System->RayData.Append(0,
                                                        PosRayGlob,
                                                        CosRayGlob,
                                                        ELEMENT_NULL,
                                                        i + 1,
                                                        LastRayNumber,
                                                        RayEvent::CREATE);

                        if (r == nullptr)
                        {
                            std::stringstream ss;
                            ss << "Failed to record ray data.\n";
                            logger->error_log(ss.str());
                        }
                    }

                    // TODO: Move interaction type determination to
                    // helper function

                    // Get optics and check for absorption
                    const OpticalProperties *optics = 0;
                    RayEvent rev = RayEvent::VIRTUAL;
                    if (Stage->Virtual)
                    {
                        // If stage is virtual, there is no interaction
                        CopyVec3(PosRayOutElement, LastPosRaySurfElement);
                        CopyVec3(CosRayOutElement, LastCosRaySurfElement);
                    }
                    else
                    {
                        // trace through the interaction
                        telement_ptr optelm = Stage->ElementList[LastElementNumber - 1];

                        if (LastHitBackSide)
                            optics = &optelm->Optics.Back;
                        else
                            optics = &optelm->Optics.Front;

                        double TestValue;
                        double UnitLastDFXYZ[3] = {0.0, 0.0, 0.0};
                        double IncidentAngle = 0;
                        switch (optics->my_type)
                        {
                        case InteractionType::REFRACTION:
                            // if (optics->UseTransmissivityTable)
                            // {
                            //     int npoints = optics->TransmissivityTable.size();
                            //     int m = 0;

                            //     UnitLastDFXYZ[0] = -LastDFXYZ[0] / sqrt(DOT(LastDFXYZ, LastDFXYZ));
                            //     UnitLastDFXYZ[1] = -LastDFXYZ[1] / sqrt(DOT(LastDFXYZ, LastDFXYZ));
                            //     UnitLastDFXYZ[2] = -LastDFXYZ[2] / sqrt(DOT(LastDFXYZ, LastDFXYZ));
                            //     IncidentAngle = acos(DOT(LastCosRaySurfElement, UnitLastDFXYZ)) * 1000.; //[mrad]
                            //     if (IncidentAngle >= optics->TransmissivityTable[npoints - 1].angle)
                            //     {
                            //         TestValue = optics->TransmissivityTable[npoints - 1].trans;
                            //     }
                            //     else
                            //     {
                            //         while (optics->TransmissivityTable[m].angle < IncidentAngle)
                            //             m++;

                            //         if (m == 0)
                            //             TestValue = optics->TransmissivityTable[m].trans;
                            //         else
                            //             TestValue = (optics->TransmissivityTable[m].trans + optics->TransmissivityTable[m - 1].trans) / 2.0;
                            //     }
                            // }
                            // else
                            TestValue = optics->transmitivity;
                            rev = RayEvent::TRANSMIT;
                            break;
                        case InteractionType::REFLECTION:

                            // if (optics->UseReflectivityTable)
                            // {
                            //     int npoints = optics->ReflectivityTable.size();
                            //     int m = 0;
                            //     UnitLastDFXYZ[0] = -LastDFXYZ[0] / sqrt(DOT(LastDFXYZ, LastDFXYZ));
                            //     UnitLastDFXYZ[1] = -LastDFXYZ[1] / sqrt(DOT(LastDFXYZ, LastDFXYZ));
                            //     UnitLastDFXYZ[2] = -LastDFXYZ[2] / sqrt(DOT(LastDFXYZ, LastDFXYZ));
                            //     IncidentAngle = acos(DOT(LastCosRaySurfElement, UnitLastDFXYZ)) * 1000.; //[mrad]
                            //     if (IncidentAngle >= optics->ReflectivityTable[npoints - 1].angle)
                            //     {
                            //         TestValue = optics->ReflectivityTable[npoints - 1].refl;
                            //     }
                            //     else
                            //     {
                            //         while (optics->ReflectivityTable[m].angle < IncidentAngle)
                            //             m++;

                            //         if (m == 0)
                            //             TestValue = optics->ReflectivityTable[m].refl;
                            //         else
                            //             TestValue = (optics->ReflectivityTable[m].refl + optics->ReflectivityTable[m - 1].refl) / 2.0;
                            //     }
                            // }
                            // else
                            TestValue = optics->reflectivity;
                            rev = RayEvent::REFLECT;
                            break;
                        default:
                            // System->errlog("Bad optical interaction type = %d (stage %d)", i, optelm->InteractionType);
                            return RunnerStatus::ERROR;
                        }

                        // Apply MonteCarlo probability of absorption. Limited
                        // for now, but can make more complex later on if desired
                        if (TestValue <= myrng())
                        {
                            // ray was fully absorbed
                            RayIsAbsorbed = true;
                            break;
                        }
                    }

                    // Process Interaction
                    int k = LastElementNumber - 1;
                    SolTrace::NativeRunner::ProcessInteraction(
                        System,
                        myrng,
                        IncludeSunShape,
                        optics,
                        IncludeErrors,
                        i,
                        Stage,
                        MultipleHitCount,
                        LastDFXYZ,
                        LastCosRaySurfElement,
                        ErrorFlag,
                        CosRayOutElement,
                        LastPosRaySurfElement,
                        PosRayOutElement);

                    // Transform ray back to stage coordinate system
                    TransformToReference(PosRayOutElement,
                                         CosRayOutElement,
                                         Stage->ElementList[k]->Origin,
                                         Stage->ElementList[k]->RLocToRef,
                                         PosRayStage,
                                         CosRayStage);
                    TransformToReference(PosRayStage,
                                         CosRayStage,
                                         Stage->Origin,
                                         Stage->RLocToRef,
                                         PosRayGlob,
                                         CosRayGlob);

                    System->RayData.Append(0,
                                           PosRayGlob,
                                           CosRayGlob,
                                           LastElementNumber,
                                           i + 1,
                                           LastRayNumber,
                                           rev);

                    // Break out if multiple hits are not allowed
                    if (!Stage->MultiHitsPerRay)
                    {
                        StageHit = false;
                        break;
                    }
                    else
                    {
                        in_multi_hit_loop = true;
                    }
                }

                // Handle if Ray was absorbed
                if (RayIsAbsorbed)
                {
                    TransformToReference(LastPosRaySurfStage,
                                         LastCosRaySurfStage,
                                         Stage->Origin,
                                         Stage->RLocToRef,
                                         PosRayGlob,
                                         CosRayGlob);

                    System->RayData.Append(0,
                                           PosRayGlob,
                                           CosRayGlob,
                                           LastElementNumber,
                                           i + 1,
                                           LastRayNumber,
                                           RayEvent::ABSORB);

                    n_rays_active--;

                    // ray was fully absorbed
                    if (RayNumber == LastRayNumberInPreviousStage)
                    {
                        PreviousStageHasRays = false;
                        if (PreviousStageDataArrayIndex > 0)
                        {
                            PreviousStageDataArrayIndex--;
                            PreviousStageHasRays = true;
                        }
                        // goto Label_EndStageLoop;
                        break;
                    }
                    else
                    {
                        if (i == 0)
                        {
                            if (RayNumber == NumberOfRays)
                                // goto Label_EndStageLoop;
                                break;
                            else
                                RayNumber++;
                        }

                        // Next ray in loop
                        continue;
                    }
                }

                // Ray has left the stage
                bool FlagMiss = false;
                if (i == 0)
                {
                    if (MultipleHitCount == 0)
                    {
                        // Ray in first stage missed stage entirely
                        // Generate new ray
                        continue;
                    }
                    else
                    {
                        // Ray hit an element, so save it for next stage
                        CopyVec3(IncomingRays[PreviousStageDataArrayIndex].Pos, PosRayGlob);
                        CopyVec3(IncomingRays[PreviousStageDataArrayIndex].Cos, CosRayGlob);
                        IncomingRays[PreviousStageDataArrayIndex].Num = RayNumber;

                        // Is Ray the last in the stage?
                        if (RayNumber == NumberOfRays)
                        {
                            StageHasRays = false;
                            break;
                        }

                        PreviousStageDataArrayIndex++;
                        PreviousStageHasRays = true;

                        // Move on to next ray
                        RayNumber++;
                        continue;
                    }
                }
                else
                {
                    // After the first stage
                    // Ray hit element OR is traced through stage
                    if (Stage->TraceThrough || MultipleHitCount > 0)
                    {
                        // Ray is saved for the next stage
                        CopyVec3(IncomingRays[PreviousStageDataArrayIndex].Pos, PosRayGlob);
                        CopyVec3(IncomingRays[PreviousStageDataArrayIndex].Cos, CosRayGlob);
                        IncomingRays[PreviousStageDataArrayIndex].Num = RayNumber;

                        // Check if ray is last in stage
                        if (RayNumber == LastRayNumberInPreviousStage)
                        {
                            StageHasRays = false;
                            break;
                        }

                        PreviousStageDataArrayIndex++;
                        PreviousStageHasRays = true;

                        if (MultipleHitCount == 0)
                        {
                            FlagMiss = true;
                        }

                        // Go to next ray
                        continue;
                    }
                    // Ray missed stage entirely and is not traced
                    else
                    {
                        FlagMiss = true;
                    }

                    // Handle FlagMiss condition (
                    if (FlagMiss == true)
                    {
                        // LastElementNumber = 0;
                        LastRayNumber = RayNumber;
                        // CopyVec3(LastPosRaySurfStage, PosRayStage);
                        // CopyVec3(LastCosRaySurfStage, CosRayStage);

                        System->RayData.Append(0,
                                               PosRayGlob,
                                               CosRayGlob,
                                               ELEMENT_NULL,
                                               i + 1,
                                               LastRayNumber,
                                               RayEvent::EXIT);

                        n_rays_active--;

                        if (RayNumber == LastRayNumberInPreviousStage)
                        {
                            if (!Stage->TraceThrough)
                            {
                                PreviousStageHasRays = false;
                                if (PreviousStageDataArrayIndex > 0)
                                {
                                    PreviousStageHasRays = true;
                                    PreviousStageDataArrayIndex--; // last ray was previous one
                                }
                            }

                            // Exit stage
                            StageHasRays = false;
                            break;
                        }
                        else
                        {
                            if (i == 0)
                                RayNumber++; // generate new sun ray

                            // Start new ray
                            continue;
                        }
                    }
                }
            }

            // EndStage section...

            // skipping save_st_data logic

            if (!PreviousStageHasRays)
            {
                LastRayNumberInPreviousStage = 0;
                continue; // No rays to carry forward
            }

            if (PreviousStageDataArrayIndex < IncomingRays.size())
            {
                LastRayNumberInPreviousStage = IncomingRays[PreviousStageDataArrayIndex].Num;
                if (LastRayNumberInPreviousStage == 0)
                {
                    // size_t pp = IncomingRays[PreviousStageDataArrayIndex - 1].Num;
                    // System->errlog("LastRayNumberInPreviousStage=0, stage %d, PrevIdx=%d, CurIdx=%d, pp=%d", i + 1,
                    //                PreviousStageDataArrayIndex, StageDataArrayIndex, pp);
                    return RunnerStatus::ERROR;
                }
            }
            else
            {
                // System->errlog("Invalid PreviousStageDataArrayIndex: %u, @ stage %d",
                //                PreviousStageDataArrayIndex, i + 1);
                return RunnerStatus::ERROR;
            }
        }

        // std::cout << "Closing out rays..." << std::endl;

        // Close out any remaining rays as misses
        unsigned idx = System->StageList.size() - 1;
        tstage_ptr Stage = System->StageList[idx];
        for (uint_fast64_t k = 0; k < n_rays_active; ++k)
        {
            GlobalRay_refactored ray = IncomingRays[k];
            System->RayData.Append(0,
                                   ray.Pos,
                                   ray.Cos,
                                   ELEMENT_NULL,
                                   idx + 1,
                                   ray.Num,
                                   RayEvent::EXIT);
        }

        // // Clean embree
        // rtcReleaseScene(embree_scene);
        // embree_scene = nullptr;
        // rtcReleaseDevice(embree_device);
        // embree_device = nullptr;

        // std::cout << "Exiting..." << std::endl;

        return RunnerStatus::SUCCESS;
    }

} // namespace SolTrace::EmbreeRunner
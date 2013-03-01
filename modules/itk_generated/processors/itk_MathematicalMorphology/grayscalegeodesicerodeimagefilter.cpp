/***********************************************************************************
 *                                                                                 *
 * Voreen - The Volume Rendering Engine                                            *
 *                                                                                 *
 * Copyright (C) 2005-2013 University of Muenster, Germany.                        *
 * Visualization and Computer Graphics Group <http://viscg.uni-muenster.de>        *
 * For a list of authors please refer to the file "CREDITS.txt".                   *
 *                                                                                 *
 * This file is part of the Voreen software package. Voreen is free software:      *
 * you can redistribute it and/or modify it under the terms of the GNU General     *
 * Public License version 2 as published by the Free Software Foundation.          *
 *                                                                                 *
 * Voreen is distributed in the hope that it will be useful, but WITHOUT ANY       *
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR   *
 * A PARTICULAR PURPOSE. See the GNU General Public License for more details.      *
 *                                                                                 *
 * You should have received a copy of the GNU General Public License in the file   *
 * "LICENSE.txt" along with this file. If not, see <http://www.gnu.org/licenses/>. *
 *                                                                                 *
 * For non-commercial academic use see the license exception specified in the file *
 * "LICENSE-academic.txt". To get information about commercial licensing please    *
 * contact the authors.                                                            *
 *                                                                                 *
 ***********************************************************************************/

#include "grayscalegeodesicerodeimagefilter.h"
#include "voreen/core/datastructures/volume/volumeram.h"
#include "voreen/core/datastructures/volume/volume.h"
#include "voreen/core/datastructures/volume/volumeatomic.h"
#include "voreen/core/ports/conditions/portconditionvolumetype.h"
#include "modules/itk/utils/itkwrapper.h"
#include "voreen/core/datastructures/volume/operators/volumeoperatorconvert.h"
#include "itkImage.h"

#include "itkGrayscaleGeodesicErodeImageFilter.h"

#include <iostream>

namespace voreen {

const std::string GrayscaleGeodesicErodeImageFilterITK::loggerCat_("voreen.GrayscaleGeodesicErodeImageFilterITK");

GrayscaleGeodesicErodeImageFilterITK::GrayscaleGeodesicErodeImageFilterITK()
    : ITKProcessor(),
    inport1_(Port::INPORT, "MarkerImage"),
    inport2_(Port::INPORT, "MaskImage"),
    outport1_(Port::OUTPORT, "OutputImage"),
    runOneIteration_("runOneIteration", "RunOneIteration", false),
    fullyConnected_("fullyConnected", "FullyConnected", false)
{
    addPort(inport1_);
    PortConditionLogicalOr* orCondition1 = new PortConditionLogicalOr();
    orCondition1->addLinkedCondition(new PortConditionVolumeTypeUInt8());
    orCondition1->addLinkedCondition(new PortConditionVolumeTypeInt8());
    orCondition1->addLinkedCondition(new PortConditionVolumeTypeUInt16());
    orCondition1->addLinkedCondition(new PortConditionVolumeTypeInt16());
    orCondition1->addLinkedCondition(new PortConditionVolumeTypeUInt32());
    orCondition1->addLinkedCondition(new PortConditionVolumeTypeInt32());
    orCondition1->addLinkedCondition(new PortConditionVolumeTypeFloat());
    orCondition1->addLinkedCondition(new PortConditionVolumeTypeDouble());
    inport1_.addCondition(orCondition1);
    addPort(inport2_);
    PortConditionLogicalOr* orCondition2 = new PortConditionLogicalOr();
    orCondition2->addLinkedCondition(new PortConditionVolumeTypeUInt8());
    orCondition2->addLinkedCondition(new PortConditionVolumeTypeInt8());
    orCondition2->addLinkedCondition(new PortConditionVolumeTypeUInt16());
    orCondition2->addLinkedCondition(new PortConditionVolumeTypeInt16());
    orCondition2->addLinkedCondition(new PortConditionVolumeTypeUInt32());
    orCondition2->addLinkedCondition(new PortConditionVolumeTypeInt32());
    orCondition2->addLinkedCondition(new PortConditionVolumeTypeFloat());
    orCondition2->addLinkedCondition(new PortConditionVolumeTypeDouble());
    inport2_.addCondition(orCondition2);
    addPort(outport1_);

    addProperty(runOneIteration_);
    addProperty(fullyConnected_);

}

Processor* GrayscaleGeodesicErodeImageFilterITK::create() const {
    return new GrayscaleGeodesicErodeImageFilterITK();
}

template<class T, class S>
void GrayscaleGeodesicErodeImageFilterITK::grayscaleGeodesicErodeImageFilterITK() {


    typedef itk::Image<T, 3> InputImageType1;
    typedef itk::Image<T, 3> OutputImageType1;

    typename InputImageType1::Pointer p1 = voreenToITK<T>(inport1_.getData());
    typename InputImageType1::Pointer p2 = voreenToITK<T>(inport2_.getData());


    //Filter define
    typedef itk::GrayscaleGeodesicErodeImageFilter<InputImageType1, OutputImageType1> FilterType;
    typename FilterType::Pointer filter = FilterType::New();

    filter->SetMarkerImage(p1);
    filter->SetMaskImage(p2);

    filter->SetRunOneIteration(runOneIteration_.get());
    filter->SetFullyConnected(fullyConnected_.get());


    observe(filter.GetPointer());

    try
    {
        filter->Update();

    }
    catch (itk::ExceptionObject &e)
    {
        LERROR(e);
    }


    Volume* outputVolume1 = 0;
    outputVolume1 = ITKToVoreenCopy<T>(filter->GetOutput());

    if (outputVolume1) {
        transferRWM(inport1_.getData(), outputVolume1);
        transferTransformation(inport1_.getData(), outputVolume1);
        outport1_.setData(outputVolume1);
    } else
        outport1_.setData(0);



}




void GrayscaleGeodesicErodeImageFilterITK::process() {
    const VolumeBase* inputHandle1 = inport1_.getData();
    const VolumeRAM* inputVolume1 = inputHandle1->getRepresentation<VolumeRAM>();

    if (dynamic_cast<const VolumeRAM_UInt8*>(inputVolume1))  {
        volumeTypeSwitch1<uint8_t>();
    }
    else if (dynamic_cast<const VolumeRAM_Int8*>(inputVolume1))  {
        volumeTypeSwitch1<int8_t>();
    }
    else if (dynamic_cast<const VolumeRAM_UInt16*>(inputVolume1))  {
        volumeTypeSwitch1<uint16_t>();
    }
    else if (dynamic_cast<const VolumeRAM_Int16*>(inputVolume1))  {
        volumeTypeSwitch1<int16_t>();
    }
    else if (dynamic_cast<const VolumeRAM_UInt32*>(inputVolume1))  {
        volumeTypeSwitch1<uint32_t>();
    }
    else if (dynamic_cast<const VolumeRAM_Int32*>(inputVolume1))  {
        volumeTypeSwitch1<int32_t>();
    }
    else if (dynamic_cast<const VolumeRAM_Float*>(inputVolume1))  {
        volumeTypeSwitch1<float>();
    }
    else if (dynamic_cast<const VolumeRAM_Double*>(inputVolume1))  {
        volumeTypeSwitch1<double>();
    }
    else {
        LERROR("Inputformat of Volume 1 is not supported!");
    }

}

template <class T>
void GrayscaleGeodesicErodeImageFilterITK::volumeTypeSwitch1() {
    const VolumeBase* inputHandle2 = inport2_.getData();
    const VolumeRAM* inputVolume2 = inputHandle2->getRepresentation<VolumeRAM>();

    if (!dynamic_cast<const VolumeAtomic<T>*>(inputVolume2))  {
        LERROR("Volume 2 must be from the same type as Volume 1!");
    }
else if (dynamic_cast<const VolumeRAM_UInt8*>(inputVolume2))  {
        grayscaleGeodesicErodeImageFilterITK<T, uint8_t>();
    }
else if (dynamic_cast<const VolumeRAM_Int8*>(inputVolume2))  {
        grayscaleGeodesicErodeImageFilterITK<T, int8_t>();
    }
else if (dynamic_cast<const VolumeRAM_UInt16*>(inputVolume2))  {
        grayscaleGeodesicErodeImageFilterITK<T, uint16_t>();
    }
else if (dynamic_cast<const VolumeRAM_Int16*>(inputVolume2))  {
        grayscaleGeodesicErodeImageFilterITK<T, int16_t>();
    }
else if (dynamic_cast<const VolumeRAM_UInt32*>(inputVolume2))  {
        grayscaleGeodesicErodeImageFilterITK<T, uint32_t>();
    }
else if (dynamic_cast<const VolumeRAM_Int32*>(inputVolume2))  {
        grayscaleGeodesicErodeImageFilterITK<T, int32_t>();
    }
else if (dynamic_cast<const VolumeRAM_Float*>(inputVolume2))  {
        grayscaleGeodesicErodeImageFilterITK<T, float>();
    }
else if (dynamic_cast<const VolumeRAM_Double*>(inputVolume2))  {
        grayscaleGeodesicErodeImageFilterITK<T, double>();
    }


}


}   // namespace

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

#ifndef VRN_ERODEOBJECTMORPHOLOGYIMAGEFILTER_H
#define VRN_ERODEOBJECTMORPHOLOGYIMAGEFILTER_H

#include "modules/itk/processors/itkprocessor.h"
#include "voreen/core/processors/volumeprocessor.h"
#include "voreen/core/ports/allports.h"
#include <string>

#include "voreen/core/properties/boolproperty.h"
#include "voreen/core/properties/voxeltypeproperty.h"
#include "voreen/core/properties/optionproperty.h"
#include "voreen/core/properties/vectorproperty.h"


namespace voreen {

class VolumeBase;

class ErodeObjectMorphologyImageFilterITK : public ITKProcessor {
public:
    ErodeObjectMorphologyImageFilterITK();

    virtual Processor* create() const;

    virtual std::string getCategory() const   { return "Volume Processing/Filtering/BinaryMathematicalMorphology"; }
    virtual std::string getClassName() const  { return "ErodeObjectMorphologyImageFilterITK";  }
    virtual CodeState getCodeState() const    { return CODE_STATE_STABLE; }

protected:
    virtual void setDescriptions() {
        setDescription("<a href=\"http://www.itk.org/Doxygen/html/classitk_1_1ErodeObjectMorphologyImageFilter.html\">Go to ITK-Doxygen-Description</a>");
    }
    template<class T>
    void erodeObjectMorphologyImageFilterITK();


    virtual void process();


private:
    VolumePort inport1_;
    VolumePort outport1_;


    BoolProperty enableProcessing_;
    VoxelTypeProperty backgroundValue_;
    VoxelTypeProperty erodeValue_;
    StringOptionProperty structuringElement_;
    StringOptionProperty shape_;
    IntVec3Property radius_;


    static const std::string loggerCat_;
};
}
#endif

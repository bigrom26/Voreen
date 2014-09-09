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

#include "modules/mod_sampler2d.frag"

uniform sampler2D colorTex0_;
uniform sampler2D depthTex0_;
uniform TextureParameters texParams0_;
uniform sampler2D colorTex1_;
uniform sampler2D depthTex1_;
uniform TextureParameters texParams1_;
uniform sampler2D colorTex2_;
uniform sampler2D depthTex2_;
uniform TextureParameters texParams2_;
uniform sampler2D colorTex3_;
uniform sampler2D depthTex3_;
uniform TextureParameters texParams3_;


void main() {
    vec2 p = gl_FragCoord.xy * screenDimRCP_;
    vec4 color0 = textureLookup2Dnormalized(colorTex0_, texParams0_, p);
    float depth0 = textureLookup2Dnormalized(depthTex0_, texParams0_, p).z;
    vec4 color1 = textureLookup2Dnormalized(colorTex1_, texParams1_, p);
    float depth1 = textureLookup2Dnormalized(depthTex1_, texParams1_, p).z;
    vec4 color2 = textureLookup2Dnormalized(colorTex2_, texParams2_, p);
    float depth2 = textureLookup2Dnormalized(depthTex2_, texParams2_, p).z;
    vec4 color3 = textureLookup2Dnormalized(colorTex3_, texParams3_, p);
    float depth3 = textureLookup2Dnormalized(depthTex3_, texParams3_, p).z;
    vec4 resColor0 = mix(color0, color1, 0.5);
    vec4 resColor1 = mix(color2, color3, 0.5);
    FragData0 = mix(resColor0, resColor1, 0.5);
    gl_FragDepth = min(min(min(depth0, depth1),depth2),depth3);
}
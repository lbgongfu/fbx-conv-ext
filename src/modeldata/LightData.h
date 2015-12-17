/*******************************************************************************
* Copyright 2011 See AUTHORS file.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*   http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
******************************************************************************/
/** @author gf */
#ifdef _MSC_VER 
#pragma once
#endif
#ifndef MODELDATA_LIGHT_DATA_H
#define MODELDATA_LIGHT_DATA_H

#include "../json/BaseJSONWriter.h"

namespace fbxconv
{
	namespace modeldata
	{
		struct LightData : public json::ConstSerializable
		{
			enum Type
			{
				Directional, Point, Spot
			};
			Type mType;
			float mColor[4];
			float mDirection[3];
			float mPosition[3];
			float mIntensity;
			float mCutoffAngle;
			float mExponent;

			LightData() : mType(Directional), mIntensity(100), mCutoffAngle(180), mExponent(1) 
			{
				mColor[0] = 1;
				mColor[1] = 1;
				mColor[2] = 1;
				mColor[3] = 1;
			}

			LightData(LightData &copyFrom) : mType(copyFrom.mType), mIntensity(copyFrom.mIntensity),
				mCutoffAngle(copyFrom.mCutoffAngle), mExponent(copyFrom.mExponent)
			{
				mColor[0] = copyFrom.mColor[0];
				mColor[1] = copyFrom.mColor[1];
				mColor[2] = copyFrom.mColor[2];
				mDirection[0] = copyFrom.mDirection[0];
				mDirection[1] = copyFrom.mDirection[1];
				mDirection[2] = copyFrom.mDirection[2];
				mPosition[0] = copyFrom.mPosition[0];
				mPosition[1] = copyFrom.mPosition[1];
				mPosition[2] = copyFrom.mPosition[2];
			}

			~LightData(){ }

			virtual void serialize(json::BaseJSONWriter &writer) const;
		};
	}
}

#endif
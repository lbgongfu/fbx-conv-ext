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
#ifndef MODELDATA_CAMERA_DATA_H
#define MODELDATA_CAMERA_DATA_H

#include "../json/BaseJSONWriter.h"

namespace fbxconv
{
	namespace modeldata
	{
		struct CameraData : public json::ConstSerializable
		{
			float position[3];
			float up[3];
			float lookAt[3];
			float fieldOfView;
			float aspectRatio;
			float near;
			float far;

			CameraData() : fieldOfView(67), aspectRatio(1.33333), near(1), far(300)
			{
				up[0] = 0;
				up[1] = 1;
				up[2] = 0;
				lookAt[0] = 0;
				lookAt[1] = 0;
				lookAt[2] = -1;
			}

			CameraData(float posX, float posY, float posZ, float lookAtX, float lookAtY, float lookAtZ,
				float upX, float upY, float upZ, float fieldOfView, float aspectRatio, float near, float far)
				: fieldOfView(fieldOfView), aspectRatio(aspectRatio), near(near), far(far)
			{
				position[0] = posX;
				position[1] = posY;
				position[2] = posZ;
				lookAt[0] = lookAtX;
				lookAt[1] = lookAtY;
				lookAt[2] = lookAtZ;
				up[0] = upX;
				up[1] = upY;
				up[2] = upZ;
			}

			CameraData(const CameraData &copyFrom) : fieldOfView(copyFrom.fieldOfView), aspectRatio(copyFrom.aspectRatio),
				near(copyFrom.near), far(copyFrom.far)
			{
				position[0] = copyFrom.position[0];
				position[1] = copyFrom.position[1];
				position[2] = copyFrom.position[2];
				lookAt[0] = copyFrom.lookAt[0];
				lookAt[1] = copyFrom.lookAt[1];
				lookAt[2] = copyFrom.lookAt[2];
				up[0] = copyFrom.up[0];
				up[1] = copyFrom.up[1];
				up[2] = copyFrom.up[2];
			}

			~CameraData(){  }

			virtual void serialize(json::BaseJSONWriter &writer) const;
		};
	}
}

#endif
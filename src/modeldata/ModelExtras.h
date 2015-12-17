/*******************************************************************************
* Copyright 2011 See AUTHORS file.
*
* Licensed under the Apache License, mVersion 2.0 (the "License");
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
#ifndef MODELDATA_MODEL_EXTRAS_H
#define MODELDATA_MODEL_EXTRAS_H

#include <vector>
#include <string>
#include "CameraData.h"
#include "LightData.h"
#include "../json/BaseJSONWriter.h"

namespace fbxconv
{
	namespace modeldata
	{
		struct ModelExtras : public json::ConstSerializable
		{
			short mVersion[2];
			float mAmbientColor[4];
			std::vector<CameraData *> mCameraDatas;
			std::vector<LightData *> mLightDatas;

			ModelExtras() 
			{ 
				mVersion[0] = VERSION_HI;
				mVersion[1] = VERSION_LO;
				mAmbientColor[0] = 1;
				mAmbientColor[1] = 1;
				mAmbientColor[2] = 1;
				mAmbientColor[3] = 1;
			}

			ModelExtras(const ModelExtras &copyFrom)
			{
				mVersion[0] = copyFrom.mVersion[0];
				mVersion[1] = copyFrom.mVersion[1];
				for (std::vector<CameraData *>::const_iterator itr = copyFrom.mCameraDatas.begin(); itr != copyFrom.mCameraDatas.end(); ++itr)
					mCameraDatas.push_back(new CameraData(**itr));
				for (std::vector<LightData *>::const_iterator itr = copyFrom.mLightDatas.begin(); itr != copyFrom.mLightDatas.end(); ++itr)
					mLightDatas.push_back(new LightData(**itr));
			}

			~ModelExtras() {
				clear();
			}

			void clear() {
				for (std::vector<CameraData *>::iterator itr = mCameraDatas.begin(); itr != mCameraDatas.end(); ++itr)
					delete *itr;
				mCameraDatas.clear();
				for (std::vector<LightData *>::iterator itr = mLightDatas.begin(); itr != mLightDatas.end(); ++itr)
					delete *itr;
				mLightDatas.clear();
			}

			virtual void serialize(json::BaseJSONWriter &writer) const;
		};
	}
}

#endif

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
/** @author lbgongfu */
#ifdef _MSC_VER
#pragma once
#endif //_MSC_VER
#ifndef FBXCONV_READERS_MODELEXTRASHELPER_H
#define FBXCONV_READERS_MODELEXTRASHELPER_H

#include "fbxsdk.h"

#define HFOV2VFOV(h, ar) (2.0 * atan((ar) * tan( (h * FBXSDK_PI_DIV_180) * 0.5)) * FBXSDK_180_DIV_PI) //ar : aspectY / aspectX
#define VFOV2HFOV(v, ar) (2.0 * atan((ar) * tan( (v * FBXSDK_PI_DIV_180) * 0.5)) * FBXSDK_180_DIV_PI) //ar : aspectX / aspectY

namespace fbxconv
{
	namespace readers
	{
		class ModelExtrasHelper
		{
		public:
			ModelExtrasHelper(FbxScene *pScene) : mScene(pScene) { }
			~ModelExtrasHelper(){ Clear(); }
			void FetchModelExtras()
			{
				mCameraArray.Clear();
				mLightArray.Clear();
				FillCameraAndLightArrayRecursive(mScene->GetRootNode());
			}
			void AddModelExtras(ModelExtras *modelExtras)
			{
				for (size_t i = 0; i < mCameraArray.GetCount(); i++)
				{
					CameraData *cameraData = CreateCameraData(mCameraArray[i]);
					if (cameraData != NULL)
						modelExtras->mCameraDatas.push_back(cameraData);
				}
				for (size_t i = 0; i < mLightArray.GetCount(); i++)
				{
					LightData *lightData = CreateLightData(mLightArray[i]);
					if (lightData != NULL)
						modelExtras->mLightDatas.push_back(lightData);
				}
			}

		private:
			FbxScene *mScene;
			//store camera nodes
			FbxArray<FbxNode*> mCameraArray;
			FbxArray<FbxNode *> mLightArray;
			void FillCameraAndLightArrayRecursive(FbxNode* pNode)
			{
				if (pNode)
				{
					if (pNode->GetNodeAttribute())
					{
						FbxNodeAttribute::EType type = pNode->GetNodeAttribute()->GetAttributeType();
						if (type == FbxNodeAttribute::eCamera)
						{
							mCameraArray.Add(pNode);
							mScene->RemoveNode(pNode);
						}
						else if (type == FbxNodeAttribute::eLight)
						{
							mLightArray.Add(pNode);
							mScene->RemoveNode(pNode);
						}
					}

					const int lCount = pNode->GetChildCount();
					for (int i = 0; i < lCount; i++)
					{
						FillCameraAndLightArrayRecursive(pNode->GetChild(i));
					}
				}
			}

			void Clear()
			{
				mCameraArray.Clear();
			}

			CameraData * CreateCameraData(FbxNode * fbxNode)
			{
				// Compute the camera position and direction.
				FbxVector4 lEye(0, 0, 1);
				FbxVector4 lCenter(0, 0, 0);
				FbxVector4 lUp(0, 1, 0);
				FbxVector4 lForward, lRight;

				FbxCamera * lFbxCamera = fbxNode->GetCamera();
				if (lFbxCamera && lFbxCamera->ProjectionType.Get() != FbxCamera::ePerspective)
					return NULL;

				//lEye = lFbxCamera->Position.Get();
				//lEye = fbxNode->EvaluateGlobalTransform().GetT();
				lEye = lFbxCamera->EvaluatePosition();
				lUp = lFbxCamera->UpVector.Get();

				if (fbxNode && fbxNode->GetTarget())
				{
					lCenter = fbxNode->GetTarget()->EvaluateGlobalTransform().GetT();
				}
				else
				{
					// Get the direction
					FbxAMatrix lGlobalRotation;
					FbxVector4 lRotationVector(fbxNode->EvaluateGlobalTransform().GetR());
					lGlobalRotation.SetR(lRotationVector);

					// Get the length
					FbxVector4 lInterestPosition(lFbxCamera->InterestPosition.Get());
					FbxVector4 lCameraGlobalPosition(fbxNode->EvaluateGlobalTransform().GetR());
					double      lLength = (FbxVector4(lInterestPosition - lCameraGlobalPosition).Length());

					// Set the center.
					// A camera with rotation = {0,0,0} points to the X direction. So create a
					// vector in the X direction, rotate that vector by the global rotation amount
					// and then position the center by scaling and translating the resulting vector
					lRotationVector = FbxVector4(1.0, 0, 0);
					lCenter = lGlobalRotation.MultT(lRotationVector);
					lCenter *= lLength;
					lCenter += lEye;

					// Update the default up vector with the camera rotation.
					lRotationVector = FbxVector4(0, 1.0, 0);
					lUp = lGlobalRotation.MultT(lRotationVector);
				}

				// Align the up vector.
				lForward = lCenter - lEye;
				lForward.Normalize();
				lRight = lForward.CrossProduct(lUp);
				lRight.Normalize();
				lUp = lRight.CrossProduct(lForward);
				lUp.Normalize();

				// Rotate the up vector with the roll value.
				double lRadians = 0;

				if (lFbxCamera)
					lRadians = lFbxCamera->Roll.Get() * FBXSDK_PI_DIV_180;
				lUp = lUp * cos(lRadians) + lRight * sin(lRadians);

				double lNearPlane = 0.01;
				if (lFbxCamera)
					lNearPlane = lFbxCamera->GetNearPlane();
				double lFarPlane = 4000.0;
				if (lFbxCamera)
					lFarPlane = lFbxCamera->GetFarPlane();

				////Get global scaling.
				//FbxVector4 lCameraScaling = fbxNode->EvaluateGlobalTransform().GetS();
				//static const int  FORWARD_SCALE = 2;

				////scaling near plane and far plane
				//lNearPlane *= lCameraScaling[FORWARD_SCALE];
				//lFarPlane *= lCameraScaling[FORWARD_SCALE];

				//get the aspect ratio
				FbxCamera::EAspectRatioMode lCamAspectRatioMode = lFbxCamera->GetAspectRatioMode();
				double lAspectX = lFbxCamera->AspectWidth.Get();
				double lAspectY = lFbxCamera->AspectHeight.Get();
				double lAspectRatio = 1.333333;
				switch (lCamAspectRatioMode)
				{
				case FbxCamera::eWindowSize:
					lAspectRatio = lAspectX / lAspectY;
					break;
				case FbxCamera::eFixedRatio:
					lAspectRatio = lAspectX;

					break;
				case FbxCamera::eFixedResolution:
					lAspectRatio = lAspectX / lAspectY * lFbxCamera->GetPixelRatio();
					break;
				case FbxCamera::eFixedWidth:
					lAspectRatio = lFbxCamera->GetPixelRatio() / lAspectY;
					break;
				case FbxCamera::eFixedHeight:
					lAspectRatio = lFbxCamera->GetPixelRatio() * lAspectX;
					break;
				default:
					break;

				}

				//get the aperture ratio
				double lFilmHeight = lFbxCamera->GetApertureHeight();
				double lFilmWidth = lFbxCamera->GetApertureWidth() * lFbxCamera->GetSqueezeRatio();
				//here we use Height : Width
				double lApertureRatio = lFilmHeight / lFilmWidth;


				//change the aspect ratio to Height : Width
				lAspectRatio = 1 / lAspectRatio;
				//revise the aspect ratio and aperture ratio
				FbxCamera::EGateFit lCameraGateFit = lFbxCamera->GateFit.Get();
				switch (lCameraGateFit)
				{

				case FbxCamera::eFitFill:
					if (lApertureRatio > lAspectRatio)  // the same as eHORIZONTAL_FIT
					{
						lFilmHeight = lFilmWidth * lAspectRatio;
						lFbxCamera->SetApertureHeight(lFilmHeight);
						lApertureRatio = lFilmHeight / lFilmWidth;
					}
					else if (lApertureRatio < lAspectRatio) //the same as eVERTICAL_FIT
					{
						lFilmWidth = lFilmHeight / lAspectRatio;
						lFbxCamera->SetApertureWidth(lFilmWidth);
						lApertureRatio = lFilmHeight / lFilmWidth;
					}
					break;
				case FbxCamera::eFitVertical:
					lFilmWidth = lFilmHeight / lAspectRatio;
					lFbxCamera->SetApertureWidth(lFilmWidth);
					lApertureRatio = lFilmHeight / lFilmWidth;
					break;
				case FbxCamera::eFitHorizontal:
					lFilmHeight = lFilmWidth * lAspectRatio;
					lFbxCamera->SetApertureHeight(lFilmHeight);
					lApertureRatio = lFilmHeight / lFilmWidth;
					break;
				case FbxCamera::eFitStretch:
					lAspectRatio = lApertureRatio;
					break;
				case FbxCamera::eFitOverscan:
					if (lFilmWidth > lFilmHeight)
					{
						lFilmHeight = lFilmWidth * lAspectRatio;
					}
					else
					{
						lFilmWidth = lFilmHeight / lAspectRatio;
					}
					lApertureRatio = lFilmHeight / lFilmWidth;
					break;
				case FbxCamera::eFitNone:
				default:
					break;
				}
				//change the aspect ratio to Width : Height
				lAspectRatio = 1 / lAspectRatio;

				double lFieldOfViewX = 0.0;
				double lFieldOfViewY = 0.0;
				if (lFbxCamera->GetApertureMode() == FbxCamera::eVertical)
				{
					lFieldOfViewY = lFbxCamera->FieldOfView.Get();
					lFieldOfViewX = VFOV2HFOV(lFieldOfViewY, 1 / lApertureRatio);
				}
				else if (lFbxCamera->GetApertureMode() == FbxCamera::eHorizontal)
				{
					lFieldOfViewX = lFbxCamera->FieldOfView.Get(); //get HFOV
					lFieldOfViewY = HFOV2VFOV(lFieldOfViewX, lApertureRatio);
				}
				else if (lFbxCamera->GetApertureMode() == FbxCamera::eFocalLength)
				{
					lFieldOfViewX = lFbxCamera->ComputeFieldOfView(lFbxCamera->FocalLength.Get());    //get HFOV
					lFieldOfViewY = HFOV2VFOV(lFieldOfViewX, lApertureRatio);
				}
				else if (lFbxCamera->GetApertureMode() == FbxCamera::eHorizAndVert) {
					lFieldOfViewX = lFbxCamera->FieldOfViewX.Get();
					lFieldOfViewY = lFbxCamera->FieldOfViewY.Get();
				}

				return new CameraData(lEye[0], lEye[1], lEye[2], lCenter[0], lCenter[1], lCenter[2],
					lUp[0], lUp[1], lUp[2], lFieldOfViewY, lAspectRatio, lNearPlane, lFarPlane);
			}

			LightData * CreateLightData(FbxNode * fbxNode)
			{
				FbxLight *lLight = fbxNode->GetLight();
				if (lLight)
				{
					FbxVector4 lTranslation = fbxNode->EvaluateGlobalTransform().GetT();
					FbxVector4 lRoration = fbxNode->EvaluateGlobalTransform().GetR();
					FbxDouble3 lColor = lLight->Color.Get();

					LightData * lLightData = new LightData();
					lLightData->mColor[0] = lColor[0];
					lLightData->mColor[1] = lColor[1];
					lLightData->mColor[2] = lColor[2];
					lLightData->mPosition[0] = lTranslation[0];
					lLightData->mPosition[1] = lTranslation[1];
					lLightData->mPosition[2] = lTranslation[2];
					lLightData->mIntensity = lLight->Intensity.Get();
					lLightData->mCutoffAngle = lLight->InnerAngle.Get();
					
					FbxLight::EType lLightType = lLight->LightType.Get();
					switch (lLightType)
					{
					case FbxLight::eDirectional:
						lLightData->mType = LightData::Directional;
						break;
					case FbxLight::ePoint:
						lLightData->mType = LightData::Point;
						break;
					case FbxLight::eSpot:
						lLightData->mType = LightData::Spot;
						break;
					default:
						break;
					}
					return lLightData;
				}
				return NULL;
			}
		};
	}
}

#endif
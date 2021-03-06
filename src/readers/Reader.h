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
/** @author Xoppa */
#ifdef _MSC_VER
#pragma once
#endif //_MSC_VER
#ifndef FBXCONV_READERS_READER_H
#define FBXCONV_READERS_READER_H

#include "../modeldata/Model.h"

using namespace fbxconv::modeldata;

namespace fbxconv {
namespace readers {

class Reader {
public:
	virtual ~Reader() {}
	virtual bool load(Settings *settings) = 0;
	//add parameter "modelExtras"(update by lbgongfu)
	virtual bool convert(Model * const &model, ModelExtras * const &modelExtras) = 0;
};

} // readers
} // fbxconv

#endif //FBXCONV_READERS_READER_H
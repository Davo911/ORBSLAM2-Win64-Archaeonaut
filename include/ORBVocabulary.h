/**
* This file is part of ORB-SLAM2.
*
* Copyright (C) 2014-2016 Raúl Mur-Artal <raulmur at unizar dot es> (University of Zaragoza)
* For more information see <https://github.com/raulmur/ORB_SLAM2>
*
* ORB-SLAM2 is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* ORB-SLAM2 is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with ORB-SLAM2. If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef ORBVOCABULARY_H
#define ORBVOCABULARY_H
#ifdef __APPLE__
#include "Thirdparty/macOS/DBoW2/DBoW2/BowVector.h"
#include "Thirdparty/macOS/DBoW2/DBoW2/FeatureVector.h"
#include "Thirdparty/macOS/DBoW2/DBoW2/FORB.h"
#include "Thirdparty/macOS/DBoW2/DBoW2/TemplatedVocabulary.h"
#elif _WIN32
#include "Thirdparty\win\DBoW2\DBoW2\BowVector.h"
#include "Thirdparty\win\DBoW2\DBoW2\FeatureVector.h"
#include "Thirdparty\win\DBoW2\DBoW2\FORB.h"
#include "Thirdparty\win\DBoW2\DBoW2\TemplatedVocabulary.h"
#elif __linux__ 
#include "Thirdparty/linux/DBoW2/DBoW2/BowVector.h"
#include "Thirdparty/linux/DBoW2/DBoW2/FeatureVector.h"
#include "Thirdparty/linux/DBoW2/DBoW2/FORB.h"
#include "Thirdparty/linux/DBoW2/DBoW2/TemplatedVocabulary.h"

#endif
namespace ORB_SLAM2
{
typedef DBoW2::TemplatedVocabulary<DBoW2::FORB::TDescriptor, DBoW2::FORB>
  ORBVocabulary;

} //namespace ORB_SLAM

#endif // ORBVOCABULARY_H

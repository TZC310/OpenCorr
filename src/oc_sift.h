/*
 * This file is part of OpenCorr, an open source C++ library for
 * study and development of 2D, 3D/stereo and volumetric
 * digital image correlation.
 *
 * Copyright (C) 2021, Zhenyu Jiang <zhenyujiang@scut.edu.cn>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one from http://mozilla.org/MPL/2.0/.
 *
 * More information about OpenCorr can be found at https://www.opencorr.org/
 */

#pragma once

#ifndef _SIFT_H_
#define _SIFT_H_

#include "oc_feature.h"

namespace opencorr
{
	//SIFT--------------------------------------------------------------------------------------------------
	struct Sift2DConfig {
		int n_features; //number of best features to retain
		int n_octave_layers; //number of layers in each octave for feature detection
		float contrast_threshold; //contrast threshold used to filter out weak features in semi-uniform (low-contrast) regions
		float edge_threshold; //threshold used to filter out edge-like features
		float sigma; //sigma of the Gaussian applied to the input image at the octave #0
	};

	class SIFT2D : public Feature2D
	{
	protected:
		cv::Mat* ref_mat = nullptr;
		cv::Mat* tar_mat = nullptr;

		Sift2DConfig sift_config;
		float matching_ratio; //ratio of the shortest distance to the second shortest distance

	public:
		std::vector<Point2D> ref_matched_kp; //matched keypoints in ref image
		std::vector<Point2D> tar_matched_kp; //matched keypoints in tar image

		SIFT2D();
		~SIFT2D();

		Sift2DConfig getSiftConfig() const;
		float getMatchingRatio() const;
		void setSiftConfig(Sift2DConfig sift_config);
		void setMatching(float matching_ratio);

		void prepare();
		void compute();
	};

}//namespace opencorr

#endif //_SIFT_H_


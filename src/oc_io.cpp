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

#include <Eigen>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "oc_io.h"

using std::ofstream;
using std::ifstream;

namespace opencorr
{
	IO2D::IO2D() {
	}

	IO2D::~IO2D() {
	}

	string IO2D::getPath() const {
		return file_path;
	}

	string IO2D::getDelimiter() const {
		return delimiter;
	}

	int IO2D::getWidth() const {
		return width;
	}

	int IO2D::getHeight()const {
		return height;
	}

	void IO2D::setPath(string file_path) {
		this->file_path = file_path;
	}

	void IO2D::setDelimiter(string delimiter) {
		this->delimiter = delimiter;
	}

	void IO2D::setWidth(int width) {
		this->width = width;
	}

	void IO2D::setHeight(int height) {
		this->height = height;
	}

	vector<POI2D> IO2D::loadTable2D() {
		ifstream file_in(file_path);
		if (!file_in) {
			std::cerr << "failed to read file " << file_path << std::endl;
		}

		string data_line;
		getline(file_in, data_line);
		vector<POI2D> poi_queue;
		size_t position1, position2;
		string variable;
		vector<float> key_buffer;

		while (getline(file_in, data_line)) {
			position1 = 0;
			position2 = 0;
			vector<float> empty_buffer;
			key_buffer.swap(empty_buffer);
			do {
				position2 = data_line.find(delimiter, position1);
				if (position2 == string::npos)
					position2 = data_line.length();

				variable = data_line.substr(position1, position2 - position1);
				if (!variable.empty())
					key_buffer.push_back(std::stof(variable));

				position1 = position2 + delimiter.length();
			} while (position2 < data_line.length() && position1 < data_line.length());

			float x = key_buffer[0];
			float y = key_buffer[1];
			POI2D current_POI(x, y);

			current_POI.deformation.u = key_buffer[2];
			current_POI.deformation.v = key_buffer[3];

			int current_index = 4;
			int array_size = (int)(sizeof(current_POI.result.r) / sizeof(current_POI.result.r[0]));
			for (int i = 0; i < array_size; i++) {
				current_POI.result.r[i] = key_buffer[current_index + i];
			}

			current_index += array_size;
			array_size = (int)(sizeof(current_POI.strain.e) / sizeof(current_POI.strain.e[0]));
			for (int i = 0; i < array_size; i++) {
				current_POI.strain.e[i] = key_buffer[current_index + i];
			}

			poi_queue.push_back(current_POI);
		}
		return poi_queue;
	}

	vector<Point2D> IO2D::loadPOI2D() {
		ifstream file_in(this->file_path);
		if (!file_in) {
			std::cerr << "failed to read file " << file_path << std::endl;
		}

		string data_line;
		getline(file_in, data_line);
		vector<Point2D> poi_queue;
		size_t position1, position2;
		string variable;
		vector<float> key_buffer;
		int poi_number = 0;

		while (getline(file_in, data_line)) {
			poi_number++;
			position1 = 0;
			position2 = 0;
			vector<float> empty_buffer;
			key_buffer.swap(empty_buffer);

			position2 = data_line.find(delimiter, position1);
			if (position2 == string::npos) {
				std::cerr << "failed to read POI at line: " << poi_number << std::endl;
			}
			variable = data_line.substr(position1, position2 - position1);
			if (!variable.empty())
				key_buffer.push_back(std::stof(variable));
			position1 = position2 + delimiter.length();
			position2 = data_line.length();
			variable = data_line.substr(position1, position2 - position1);
			if (!variable.empty())
				key_buffer.push_back(std::stof(variable));

			float x = key_buffer[0];
			float y = key_buffer[1];
			Point2D current_poi(x, y);

			poi_queue.push_back(current_poi);
		}
		return poi_queue;
	}

	void IO2D::saveTable2D(vector<POI2D>& poi_queue) {
		std::ofstream file_out(file_path);
		file_out.setf(std::ios::fixed);
		file_out << std::setprecision(8);
		if (file_out.is_open()) {
			file_out << "x" << delimiter;
			file_out << "y" << delimiter;

			file_out << "u" << delimiter;
			file_out << "v" << delimiter;

			file_out << "u0" << delimiter;
			file_out << "v0" << delimiter;
			file_out << "ZNCC" << delimiter;
			file_out << "iteration" << delimiter;
			file_out << "convergence" << delimiter;
			file_out << "feature" << delimiter;

			file_out << "exx" << delimiter;
			file_out << "eyy" << delimiter;
			file_out << "exy" << delimiter;
			file_out << std::endl;

			for (vector<POI2D>::iterator iter = poi_queue.begin(); iter != poi_queue.end(); ++iter) {
				file_out << iter->x << delimiter;
				file_out << iter->y << delimiter;

				file_out << iter->deformation.u << delimiter;
				file_out << iter->deformation.v << delimiter;

				int array_size = (int)(sizeof(iter->result.r) / sizeof(iter->result.r[0]));
				for (int i = 0; i < array_size; i++) {
					file_out << iter->result.r[i] << delimiter;
				}

				array_size = (int)(sizeof(iter->strain.e) / sizeof(iter->strain.e[0]));
				for (int i = 0; i < array_size; i++) {
					file_out << iter->strain.e[i] << delimiter;
				}
				file_out << std::endl;
			}
		}
		file_out.close();
	}

	void IO2D::saveDeformationTable2D(vector<POI2D>& poi_queue) {
		std::ofstream file_out(file_path);
		file_out.setf(std::ios::fixed);
		file_out << std::setprecision(8);
		if (file_out.is_open()) {
			file_out << "x" << delimiter;
			file_out << "y" << delimiter;

			file_out << "u" << delimiter;
			file_out << "ux" << delimiter;
			file_out << "uy" << delimiter;
			file_out << "uxx" << delimiter;
			file_out << "uxy" << delimiter;
			file_out << "uyy" << delimiter;

			file_out << "v" << delimiter;
			file_out << "vx" << delimiter;
			file_out << "vy" << delimiter;
			file_out << "vxx" << delimiter;
			file_out << "vxy" << delimiter;
			file_out << "vyy" << delimiter;
			file_out << std::endl;

			for (vector<POI2D>::iterator iter = poi_queue.begin(); iter != poi_queue.end(); ++iter) {
				file_out << iter->x << delimiter;
				file_out << iter->y << delimiter;

				int array_size = (int)(sizeof(iter->deformation.p) / sizeof(iter->deformation.p[0]));
				for (int i = 0; i < array_size; i++) {
					file_out << iter->deformation.p[i] << delimiter;
				}
				file_out << std::endl;
			}
		}
		file_out.close();
	}

	void IO2D::saveMap2D(vector<POI2D>& poi_queue, char variable) {
		int height = getHeight();
		int width = getWidth();
		Eigen::MatrixXf output_map = Eigen::MatrixXf::Zero(height, width);

		switch (variable) {
		case 'u':
			for (int i = 0; i < poi_queue.size(); ++i) {
				output_map((int)poi_queue[i].y, (int)poi_queue[i].x) = poi_queue[i].deformation.u;
			}
			break;
		case 'v':
			for (int i = 0; i < poi_queue.size(); ++i) {
				output_map((int)poi_queue[i].y, (int)poi_queue[i].x) = poi_queue[i].deformation.v;
			}
			break;
		case 'c':
			for (int i = 0; i < poi_queue.size(); ++i) {
				output_map((int)poi_queue[i].y, (int)poi_queue[i].x) = poi_queue[i].result.zncc;
			}
			break;
		case 'd':
			for (int i = 0; i < poi_queue.size(); ++i) {
				output_map((int)poi_queue[i].y, (int)poi_queue[i].x) = poi_queue[i].result.convergence;
			}
			break;
		case 'i':
			for (int i = 0; i < poi_queue.size(); ++i) {
				output_map((int)poi_queue[i].y, (int)poi_queue[i].x) = poi_queue[i].result.iteration;
			}
			break;
		case 'f':
			for (int i = 0; i < poi_queue.size(); ++i) {
				output_map((int)poi_queue[i].y, (int)poi_queue[i].x) = poi_queue[i].result.feature;
			}
			break;
		case 'x':
			for (int i = 0; i < poi_queue.size(); ++i) {
				output_map((int)poi_queue[i].y, (int)poi_queue[i].x) = poi_queue[i].strain.exx;
			}
			break;
		case 'y':
			for (int i = 0; i < poi_queue.size(); ++i) {
				output_map((int)poi_queue[i].y, (int)poi_queue[i].x) = poi_queue[i].strain.eyy;
			}
			break;
		case 'r':
			for (int i = 0; i < poi_queue.size(); ++i) {
				output_map((int)poi_queue[i].y, (int)poi_queue[i].x) = poi_queue[i].strain.exy;
			}
			break;
		default:
			return;
		}

		std::ofstream file_out(file_path);
		file_out.setf(std::ios::fixed);
		file_out << std::setprecision(8);
		if (file_out.is_open()) {
			for (int r = 0; r < height; r++) {
				for (int c = 0; c < width; c++) {
					file_out << output_map(r, c) << delimiter;
				}
				file_out << std::endl;
			}
		}
		file_out.close();
	}

	vector<POI2DS> IO2D::loadTable2DS() {
		ifstream file_in(file_path);
		if (!file_in) {
			std::cerr << "failed to read file " << file_path << std::endl;
		}

		string data_line;
		getline(file_in, data_line);
		vector<POI2DS> poi_queue;
		size_t position1, position2;
		string variable;
		vector<float> key_buffer;

		while (getline(file_in, data_line)) {
			position1 = 0;
			position2 = 0;
			vector<float> empty_buffer;
			key_buffer.swap(empty_buffer);
			do {
				position2 = data_line.find(delimiter, position1);
				if (position2 == string::npos)
					position2 = data_line.length();

				variable = data_line.substr(position1, position2 - position1);
				if (!variable.empty())
					key_buffer.push_back(std::stof(variable));

				position1 = position2 + delimiter.length();
			} while (position2 < data_line.length() && position1 < data_line.length());

			float x = key_buffer[0];
			float y = key_buffer[1];
			POI2DS current_POI(x, y);

			int current_index = 2;
			int array_size = (int)(sizeof(current_POI.deformation.p) / sizeof(current_POI.deformation.p[0]));
			for (int i = 0; i < array_size; i++) {
				current_POI.deformation.p[i] = key_buffer[current_index + i];
			}

			current_index += array_size;
			array_size = (int)(sizeof(current_POI.result.r) / sizeof(current_POI.result.r[0]));
			for (int i = 0; i < array_size; i++) {
				current_POI.result.r[i] = key_buffer[current_index + i];
			}

			current_index += array_size;
			current_POI.ref_coor.x = key_buffer[current_index];
			current_POI.ref_coor.y = key_buffer[current_index + 1];
			current_POI.ref_coor.z = key_buffer[current_index + 2];

			current_POI.tar_coor.x = key_buffer[current_index + 3];
			current_POI.tar_coor.y = key_buffer[current_index + 4];
			current_POI.tar_coor.z = key_buffer[current_index + 5];

			current_index += 6;
			array_size = (int)(sizeof(current_POI.strain.e) / sizeof(current_POI.strain.e[0]));
			for (int i = 0; i < array_size; i++) {
				current_POI.strain.e[i] = key_buffer[current_index + i];
			}
			poi_queue.push_back(current_POI);
		}
		return poi_queue;
	}

	void IO2D::saveTable2DS(vector<POI2DS>& poi_queue) {
		std::ofstream file_out(file_path);
		file_out.setf(std::ios::fixed);
		file_out << std::setprecision(8);
		if (file_out.is_open()) {
			file_out << "x" << delimiter;
			file_out << "y" << delimiter;

			file_out << "u" << delimiter;
			file_out << "v" << delimiter;
			file_out << "w" << delimiter;

			file_out << "r1r2 ZNCC" << delimiter;
			file_out << "r1t1 ZNCC" << delimiter;
			file_out << "r1t2 ZNCC" << delimiter;

			file_out << "r2_x" << delimiter;
			file_out << "r2_y" << delimiter;
			file_out << "t1_x" << delimiter;
			file_out << "t1_y" << delimiter;
			file_out << "t2_x" << delimiter;
			file_out << "t2_y" << delimiter;

			file_out << "ref_x" << delimiter;
			file_out << "ref_y" << delimiter;
			file_out << "ref_z" << delimiter;
			file_out << "tar_x" << delimiter;
			file_out << "tar_y" << delimiter;
			file_out << "tar_z" << delimiter;

			file_out << "exx" << delimiter;
			file_out << "eyy" << delimiter;
			file_out << "ezz" << delimiter;
			file_out << "exy" << delimiter;
			file_out << "eyz" << delimiter;
			file_out << "ezx" << delimiter;

			file_out << std::endl;

			for (vector<POI2DS>::iterator iter = poi_queue.begin(); iter != poi_queue.end(); ++iter) {
				file_out << iter->x << delimiter;
				file_out << iter->y << delimiter;

				int array_size = (int)(sizeof(iter->deformation.p) / sizeof(iter->deformation.p[0]));
				for (int i = 0; i < array_size; i++) {
					file_out << iter->deformation.p[i] << delimiter;
				}

				array_size = (int)(sizeof(iter->result.r) / sizeof(iter->result.r[0]));
				for (int i = 0; i < array_size; i++) {
					file_out << iter->result.r[i] << delimiter;
				}

				file_out << iter->ref_coor.x << delimiter;
				file_out << iter->ref_coor.y << delimiter;
				file_out << iter->ref_coor.z << delimiter;

				file_out << iter->tar_coor.x << delimiter;
				file_out << iter->tar_coor.y << delimiter;
				file_out << iter->tar_coor.z << delimiter;

				array_size = (int)(sizeof(iter->strain.e) / sizeof(iter->strain.e[0]));
				for (int i = 0; i < array_size; i++) {
					file_out << iter->strain.e[i] << delimiter;
				}
				file_out << std::endl;
			}
		}
		file_out.close();
	}

	void IO2D::saveMap2DS(vector<POI2DS>& poi_queue, char variable) {
		int height = getHeight();
		int width = getWidth();
		Eigen::MatrixXf output_map = Eigen::MatrixXf::Zero(height, width);

		switch (variable) {
		case 'u':
			for (int i = 0; i < poi_queue.size(); ++i) {
				output_map((int)poi_queue[i].y, (int)poi_queue[i].x) = poi_queue[i].deformation.u;
			}
			break;
		case 'v':
			for (int i = 0; i < poi_queue.size(); ++i) {
				output_map((int)poi_queue[i].y, (int)poi_queue[i].x) = poi_queue[i].deformation.v;
			}
			break;
		case 'w':
			for (int i = 0; i < poi_queue.size(); ++i) {
				output_map((int)poi_queue[i].y, (int)poi_queue[i].x) = poi_queue[i].deformation.w;
			}
			break;
		case 'c':
			for (int i = 0; i < poi_queue.size(); ++i) {
				output_map((int)poi_queue[i].y, (int)poi_queue[i].x) = poi_queue[i].result.r1r2_zncc;
			}
			break;
		case 'd':
			for (int i = 0; i < poi_queue.size(); ++i) {
				output_map((int)poi_queue[i].y, (int)poi_queue[i].x) = poi_queue[i].result.r1t1_zncc;
			}
			break;
		case 'e':
			for (int i = 0; i < poi_queue.size(); ++i) {
				output_map((int)poi_queue[i].y, (int)poi_queue[i].x) = poi_queue[i].result.r1t2_zncc;
			}
			break;
		case 'x':
			for (int i = 0; i < poi_queue.size(); ++i) {
				output_map((int)poi_queue[i].y, (int)poi_queue[i].x) = poi_queue[i].strain.exx;
			}
			break;
		case 'y':
			for (int i = 0; i < poi_queue.size(); ++i) {
				output_map((int)poi_queue[i].y, (int)poi_queue[i].x) = poi_queue[i].strain.eyy;
			}
			break;
		case 'z':
			for (int i = 0; i < poi_queue.size(); ++i) {
				output_map((int)poi_queue[i].y, (int)poi_queue[i].x) = poi_queue[i].strain.ezz;
			}
			break;
		case 'r':
			for (int i = 0; i < poi_queue.size(); ++i) {
				output_map((int)poi_queue[i].y, (int)poi_queue[i].x) = poi_queue[i].strain.exy;
			}
			break;
		case 's':
			for (int i = 0; i < poi_queue.size(); ++i) {
				output_map((int)poi_queue[i].y, (int)poi_queue[i].x) = poi_queue[i].strain.eyz;
			}
			break;
		case 't':
			for (int i = 0; i < poi_queue.size(); ++i) {
				output_map((int)poi_queue[i].y, (int)poi_queue[i].x) = poi_queue[i].strain.ezx;
			}
			break;
		default:
			return;
		}

		std::ofstream file_out(file_path);
		file_out.setf(std::ios::fixed);
		file_out << std::setprecision(8);
		if (file_out.is_open()) {
			for (int r = 0; r < height; r++) {
				for (int c = 0; c < width; c++) {
					file_out << output_map(r, c) << delimiter;
				}
				file_out << std::endl;
			}
		}
		file_out.close();
	}

}//namespace opencorr
// Copyright (c) 2014-2018 Bauhaus-Universitaet Weimar
// This Software is distributed under the Modified BSD License, see license.txt.
//
// Virtual Reality and Visualization Research Group 
// Faculty of Media, Bauhaus-Universitaet Weimar
// http://www.uni-weimar.de/medien/vr

#include <stdexcept>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <chrono>
#include <lamure/types.h>
#include <e57/E57Foundation.h>
#include <e57/E57Simple.h>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <math.h>
#include <float.h>
#include <limits.h>
#include <vector>
#include <algorithm>
#include <random>
#include <map>
#include <osg/Matrix>
#include <osg/Vec3>
#include <lamure/pre/io/format_e57.h>

#if defined(__GNUC__) && !defined(__clang__)
#include <parallel/algorithm>
namespace alg = __gnu_parallel;
#else
namespace alg = std;
#endif

using namespace std;

bool intensityOnly;
bool readScannerPositions = false;

namespace lamure
{
namespace pre
{

void format_e57::
getFilenames(const boost::filesystem::path &p, std::vector<std::string> &f) {
boost::filesystem::directory_iterator end_itr;
for (boost::filesystem::directory_iterator itr(p); itr != end_itr; ++itr)
{
    if (is_regular_file(itr->path())) {
        std::string current_file = itr->path().string();
        if (current_file.substr(current_file.size() - 4) == std::string(".e57")) {
            f.push_back(current_file);
            std::cout << current_file << std::endl;
        }
    }
}
}

void format_e57::
write(const std::string &filename, buffer_callback_function callback) {
	LOGGER_ERROR("Not implemented"); 
}


void format_e57::
read(const std::string &file, surfel_callback_funtion callback)
{
	osg::Matrix m;
	m.makeIdentity();
	try
	{
        const char *filename = file.c_str();
		e57::Reader	eReader(filename);
		e57::E57Root	rootHeader;
		eReader.GetE57Root(rootHeader);

		//Get the number of scan images available
		int data3DCount = eReader.GetData3DCount();
		e57::Data3D	scanHeader;
		for (int scanIndex = 0; scanIndex < data3DCount; scanIndex++)
		{
			eReader.ReadData3D(scanIndex, scanHeader);
			osg::Matrix trans;
			trans.makeTranslate(scanHeader.pose.translation.x, scanHeader.pose.translation.y, scanHeader.pose.translation.z);
			osg::Matrix rot;
			rot.makeRotate(osg::Quat(scanHeader.pose.rotation.x, scanHeader.pose.rotation.y, scanHeader.pose.rotation.z, scanHeader.pose.rotation.w));
			m = rot*trans;

			int64_t nColumn = 0;
			int64_t nRow = 0;
			int64_t nPointsSize = 0;	//Number of points
			int64_t nGroupsSize = 0;	//Number of groups
			int64_t nCountSize = 0;		//Number of points per group
			bool	bColumnIndex = false; //indicates that idElementName is "columnIndex"

			eReader.GetData3DSizes(scanIndex, nRow, nColumn, nPointsSize, nGroupsSize, nCountSize, bColumnIndex);

			int64_t nSize = nRow;
			if (nSize == 0) nSize = 1024;	// choose a chunk size

            int8_t * isInvalidData = NULL;
            isInvalidData = new int8_t[nSize];
            if (!scanHeader.pointFields.cartesianInvalidStateField)
            {
                for (int i = 0; i < nSize; i++)
                    isInvalidData[i] = 0;
            }

			double * xData = NULL;
			if (scanHeader.pointFields.cartesianXField)
				xData = new double[nSize];
			double * yData = NULL;
			if (scanHeader.pointFields.cartesianYField)
				yData = new double[nSize];
			double * zData = NULL;
			if (scanHeader.pointFields.cartesianZField)
				zData = new double[nSize];
			double * rangeData = NULL;
			if (scanHeader.pointFields.sphericalRangeField)
				rangeData = new double[nSize];
			double * azData = NULL;
			if (scanHeader.pointFields.sphericalAzimuthField)
				azData = new double[nSize];
			double * elData = NULL;
			if (scanHeader.pointFields.sphericalElevationField)
				elData = new double[nSize];

			double *	intData = NULL;
			bool		bIntensity = false;
			double		intRange = 0;
			double		intOffset = 0;


			if (scanHeader.pointFields.intensityField)
			{
				bIntensity = true;
				intData = new double[nSize];
				intRange = scanHeader.intensityLimits.intensityMaximum - scanHeader.intensityLimits.intensityMinimum;
				intOffset = scanHeader.intensityLimits.intensityMinimum;
			}


			uint16_t *	redData = NULL;
			uint16_t *	greenData = NULL;
			uint16_t *	blueData = NULL;
			bool		bColor = false;
			int32_t		colorRedRange = 1;
			int32_t		colorRedOffset = 0;
			int32_t		colorGreenRange = 1;
			int32_t		colorGreenOffset = 0;
			int32_t		colorBlueRange = 1;
			int32_t		colorBlueOffset = 0;


			if (scanHeader.pointFields.colorRedField)
			{
				bColor = true;
				redData = new uint16_t[nSize];
				greenData = new uint16_t[nSize];
				blueData = new uint16_t[nSize];
				colorRedRange = scanHeader.colorLimits.colorRedMaximum - scanHeader.colorLimits.colorRedMinimum;
				colorRedOffset = scanHeader.colorLimits.colorRedMinimum;
				colorGreenRange = scanHeader.colorLimits.colorGreenMaximum - scanHeader.colorLimits.colorGreenMinimum;
				colorGreenOffset = scanHeader.colorLimits.colorGreenMinimum;
				colorBlueRange = scanHeader.colorLimits.colorBlueMaximum - scanHeader.colorLimits.colorBlueMinimum;
				colorBlueOffset = scanHeader.colorLimits.colorBlueMinimum;
			}


			int64_t * idElementValue = NULL;
			int64_t * startPointIndex = NULL;
			int64_t * pointCount = NULL;
			if (nGroupsSize > 0)
			{
				idElementValue = new int64_t[nGroupsSize];
				startPointIndex = new int64_t[nGroupsSize];
				pointCount = new int64_t[nGroupsSize];

				if (!eReader.ReadData3DGroupsData(scanIndex, nGroupsSize, idElementValue,
					startPointIndex, pointCount))
					nGroupsSize = 0;
			}

			int8_t * rowIndex = NULL;
			int32_t * columnIndex = NULL;
			if (scanHeader.pointFields.rowIndexField)
				rowIndex = new int8_t[nSize];
			if (scanHeader.pointFields.columnIndexField)
				columnIndex = new int32_t[nRow];


			e57::CompressedVectorReader dataReader = eReader.SetUpData3DPointsData(
				scanIndex,			//!< data block index given by the NewData3D
				nSize,				//!< size of each of the buffers given
				xData,				//!< pointer to a buffer with the x data
				yData,				//!< pointer to a buffer with the y data
				zData,				//!< pointer to a buffer with the z data
				isInvalidData,		//!< pointer to a buffer with the valid indication
				intData,			//!< pointer to a buffer with the lidar return intesity
				NULL,
				redData,			//!< pointer to a buffer with the color red data
				greenData,			//!< pointer to a buffer with the color green data
				blueData,			//!< pointer to a buffer with the color blue data
				NULL, //sColorInvalid
				rangeData,
				azData,
				elData
				/*rowIndex,			//!< pointer to a buffer with the rowIndex
				columnIndex			//!< pointer to a buffer with the columnIndex*/
			);

			int64_t		count = 0;
			unsigned	size = 0;
			int			col = 0;
			int			row = 0;
			while ((size = dataReader.read()))
			{
				for (unsigned int i = 0; i < size; i++)
				{

					if (isInvalidData[i] == 0 && xData)
					{
						osg::Vec3 p(xData[i], yData[i], zData[i]);
						p = p * m;

                        vec3r pos;
						pos.x = p[0];
						pos.y = p[1];
						pos.z = p[2];

                        vec3b color;
						color.r = ((redData[i] - colorRedOffset) * 255.0) / colorRedRange;
						color.g = ((greenData[i] - colorGreenOffset) * 255.0) / colorGreenRange;
						color.b = ((blueData[i] - colorBlueOffset) * 255.0) / colorBlueRange;

						callback(surfel(vec3r(pos[0], pos[1], pos[2]), vec3b(color[0], color[1], color[2])));

					}
					else if (isInvalidData[i] == 0 && rangeData)
					{
						osg::Vec3 p(rangeData[i] * cos(elData[i]) * cos(azData[i]), rangeData[i] * cos(elData[i]) * sin(azData[i]), rangeData[i] * sin(elData[i]));
						p = p * m;

                        vec3r pos;
						pos.x = p[0];
						pos.y = p[1];
						pos.z = p[2];

						//Normalize color to 0 - 255
                        vec3b color;
						color.r = ((redData[i] - colorRedOffset) * 255.0) / colorRedRange;
						color.g = ((greenData[i] - colorGreenOffset) * 255.0) / colorGreenRange;
						color.b = ((blueData[i] - colorBlueOffset) * 255.0) / colorBlueRange;

						callback(surfel(vec3r(pos[0], pos[1], pos[2]), vec3b(color[0], color[1], color[2])));
						
					}

                 
				}
			}

			dataReader.close();

			delete[] isInvalidData;
			delete[] xData;
			delete[] yData;
			delete[] zData;
			delete[] rangeData;
			delete[] azData;
			delete[] elData;
			delete[] intData;
			delete[] redData;
			delete[] greenData;
			delete[] blueData;
			delete[] idElementValue;
			delete[] startPointIndex;
			delete[] pointCount;
			delete[] rowIndex;
			delete[] columnIndex;
		}

		eReader.Close();
		
		}
		catch (std::exception& ex) {
			cerr << "Got an std::exception, what=" << ex.what() << endl;
		}
		catch (...) {
			cerr << "Got an unknown exception" << endl;
		}


}
    
}

}



#include "stdafx.h"

#include <string>
#include <cstddef>
#include <cstdlib>
#include <cmath>
#include <vector>


			std::string encode(int number_to_encode)
			{
				std::string output;
				while (number_to_encode >= 0x20)
				{
					const int next_value = (0x20 | (number_to_encode & 0x1f)) + 63;
					output += static_cast<char>(next_value);
					number_to_encode >>= 5;
				}

				number_to_encode += 63;
				output += static_cast<char>(number_to_encode);
				return output;
			}

			std::string encode(std::vector<int> &numbers)
			{
				std::string output;
				for (uint32 i = 0; i < numbers.size(); ++i)
				{
					int& number = numbers[i];
					bool isNegative = number < 0;

					if (isNegative)
					{
						const unsigned binary = std::llabs(number);
						const unsigned twos = (~binary) + 1u;
						number = twos;
					}

					number <<= 1u;

					if (isNegative)
					{
						number = ~number;
					}
				}
				for (uint32 i = 0; i < numbers.size(); ++i)
				{
					int& number = numbers[i];
					output += encode(number);
				}
				return output;
			}
 
		std::string polylineEncode(CommonLib::GisXYPoint* pPoint, int32 nPointCnt, double dOffsetX, double dOffsetY, double dScale)
		{
			 

			std::vector<int32> delta_numbers;
			delta_numbers.reserve(nPointCnt* 2  + 1);
			//util::FixedPointCoordinate previous_coordinate = {0, 0};
			int32 nPrevCoordX = 0; 
			int32 nPrevCoordY = 0; 
		//	for (const auto &segment : polyline)
			for (uint32 i = 0; i < nPointCnt; ++i)
			{
				//if (segment.necessary)
				{
					int32 X = (int32)((pPoint[i].x + dOffsetX)/dScale);
					int32 Y = (int32)((pPoint[i].y + dOffsetY)/dScale);

					const int32 x_diff = X - nPrevCoordX;
					const int32 y_diff = Y - nPrevCoordY;
					delta_numbers.emplace_back(x_diff);
					delta_numbers.emplace_back(y_diff);
					nPrevCoordX = X;
					nPrevCoordY = Y;
				}
			}
			return encode(delta_numbers);
		}
		
	/*	std::vector<util::FixedPointCoordinate> polylineDecode(const std::string &geometry_string)
		{
			std::vector<util::FixedPointCoordinate> new_coordinates;
			int index = 0, len = geometry_string.size();
			int lat = 0, lng = 0;

			while (index < len)
			{
				int b, shift = 0, result = 0;
				do
				{
					b = geometry_string.at(index++) - 63;
					result |= (b & 0x1f) << shift;
					shift += 5;
				} while (b >= 0x20);
				int dlat = ((result & 1) != 0 ? ~(result >> 1) : (result >> 1));
				lat += dlat;

				shift = 0;
				result = 0;
				do
				{
					b = geometry_string.at(index++) - 63;
					result |= (b & 0x1f) << shift;
					shift += 5;
				} while (b >= 0x20);
				int dlng = ((result & 1) != 0 ? ~(result >> 1) : (result >> 1));
				lng += dlng;

				util::FixedPointCoordinate p;
				p.lat = COORDINATE_PRECISION * (((double)lat / 1E6));
				p.lon = COORDINATE_PRECISION * (((double)lng / 1E6));
				new_coordinates.push_back(p);
			}

			return new_coordinates;
		}*/

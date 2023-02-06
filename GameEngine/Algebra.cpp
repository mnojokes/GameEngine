// Copyright (C) 2023  Mantas Naujokas
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "Algebra.h"
#include <math.h>

namespace Math
{
	Vector3 Line::F(float x) const
	{
		Vector3 ret = {};

		if (m_bound0.x == m_bound1.x)
		{
			ret = { m_bound0.x, x, 0.0f };
		}
		else
		{
			float y = k * x + b;
			ret = { x, y, 0.0f };
		}

		return ret;		
	}

	Line LineFromTwoPoints(const Vector3& p0, const Vector3& p1)
	{
		Line ret;
		ret.m_bound0 = p0;
		ret.m_bound1 = p1;
		// y = kx + b

		ret.k = (p1.y - p0.y) / (p1.x - p0.x);
		ret.b = p0.y - ret.k * p0.x;

		return ret;
	}

	LineStandardForm LineToStandardForm(const Line& line)
	{
		// ax + by + c = 0
		LineStandardForm ret;
		if (line.m_bound0.x == line.m_bound1.x)
		{ // vertical line
			ret.a = 1.0f;
			ret.b = 0.0f;
			ret.c = 0.0f;
		}
		else
		{
			ret.a = line.k;
			ret.b = 1.0f;
			ret.c = line.b;
		}

		return ret;
	}

	float DistancePointToLine(const Line& line, const Vector3& point)
	{
		LineStandardForm stdLine = LineToStandardForm(line);
		// ret = |aXx + bXy + c| / sqrt(a ^ 2 + b ^ 2)

		float ret = 0.0f;
		if (line.m_bound0.x == line.m_bound1.x)
		{ // vertical line
			ret = static_cast<float>(fabs(point.x - line.m_bound0.x));
		}
		else
		{
			ret = static_cast<float>(
				fabs(stdLine.a * point.x + stdLine.b * point.y + stdLine.c) /
				sqrt(stdLine.a * stdLine.a + stdLine.b * stdLine.b));
		}

		return ret;
	}

	float LineLength(const Line& line)
	{
		return LineLength(line.m_bound0, line.m_bound1);
	}

	float LineLength(const Vector3& a, const Vector3& b)
	{
		Vector3 diff = a - b;

		return diff.Magnitude();
	}
}

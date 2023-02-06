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

#ifndef ALGEBRA_H
#define ALGEBRA_H

#include "Vector3.h"

#define POW2(x) ((x) * (x))

namespace Math
{
	struct Line
	{
		Vector3 F(float x) const;

		Vector3 m_bound0;
		Vector3 m_bound1;
		float k = 0.0f;
		float b = 0.0f;
	};

	struct LineStandardForm
	{
		float a;
		float b;
		float c;
	};

	Line LineFromTwoPoints(const Vector3& p0, const Vector3& p1);
	LineStandardForm LineToStandardForm(const Line& line);
	float DistancePointToLine(const Line& line, const Vector3& point);

	float LineLength(const Line& line);
	float LineLength(const Vector3& a, const Vector3& b);
}

#endif

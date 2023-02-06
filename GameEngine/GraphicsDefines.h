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

#ifndef GRAPHICSDEFINES_H
#define GRAPHICSDEFINES_H

enum class FillMode
{
	Wireframe,
	Solid
};

enum class CullMode
{
	None,
	Front,
	Back
};

enum class BlendType
{
	Zero,
	One,
	SrcAlpha,
	DestAlpha,
	InvSrcAlpha,
	InvDestAlpha
};

enum class ShaderType
{
	Vertex,
	Pixel
};

enum class ChannelMask
{
	Red,
	Green,
	Blue,
	Alpha,
	All
};

enum class SamplerType
{
	Point,
	Bilinear,
	Trilinear,
	Anisotropic1,
	Anisotropic4,
	Anisotropic16
};

enum class TextureAddressMode
{
	Wrap,
	Clamp,
	Mirror
};

#endif

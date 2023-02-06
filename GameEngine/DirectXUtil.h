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

#ifndef DIRECTXUTIL_H
#define DIRECTXUTIL_H

#include <d3d11.h>
#include <dxgi.h>
#include <DirectXTex.h>

/* Release DirectX resource */
#define SAFE_RELEASE(x) \
if (x != nullptr) \
{ \
	x->Release(); \
	x = nullptr; \
}

#endif

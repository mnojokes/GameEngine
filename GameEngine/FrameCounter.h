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

#ifndef FRAMECOUNTER_H
#define FRAMECOUNTER_H

#include <stdint.h>
#include <chrono>

class FrameCounter
{
public:
	void Start(void);	// start measurement
	void Finish(void);	// finish measurement, wait for FPSLock time if necessary, record m_lastDelta
	
	/* Get last calculated delta time in seconds */
	float GetDelta(void) const;

	/* SetFPSLock - lock FPS to a specified number of frames */
	void SetFPSLock(uint32_t fps);

private:
	/* Frame time in seconds, determined by value sent to SetFPSLock */
	std::chrono::nanoseconds m_lockedFrameTime = {};

	/* Time at the start of frame */
	std::chrono::steady_clock::time_point m_frameStart = {};

	/* Delta time of the last frame */
	std::chrono::nanoseconds m_lastDelta = {};
};

#endif

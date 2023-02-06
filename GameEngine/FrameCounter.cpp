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

#include "FrameCounter.h"
#include "MathConstants.h"

void FrameCounter::Start(void)
{
	m_frameStart = std::chrono::steady_clock::now();
}

void FrameCounter::Finish(void)
{
	m_lastDelta = std::chrono::steady_clock::now() - m_frameStart;
	while (m_lastDelta < m_lockedFrameTime)
	{
		m_lastDelta = std::chrono::steady_clock::now() - m_frameStart;
	}
}

float FrameCounter::GetDelta(void) const
{
	return static_cast<float>(m_lastDelta.count()) * rcpNano;
}

void FrameCounter::SetFPSLock(uint32_t fps)
{
	float timeNanoFL = fps > 0 ? NANO / static_cast<float>(fps) : 0.0f;
	uint64_t timeNanoUINT = static_cast<uint64_t>(timeNanoFL);
	m_lockedFrameTime = std::chrono::nanoseconds(timeNanoUINT);
}
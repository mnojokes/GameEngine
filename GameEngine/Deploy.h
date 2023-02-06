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

#ifndef DEPLOY_H
#define DEPLOY_H

#ifdef _DEBUG
#define MAIN_RET int
#define MAIN_NAME main
#define MAIN_PARAMS void
#else
#define MAIN_RET INT
#define MAIN_NAME WINAPI WinMain
#define MAIN_PARAMS _In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int
#endif

// Create app with GamePlaySystemType
#define DEPLOY(GameplaySystemType)\
MAIN_RET MAIN_NAME(MAIN_PARAMS) \
{ \
	App app; \
\
	bool initialized = app.Initialize<GameplaySystemType>(); \
	if (!initialized) \
	{ \
		app.Exit(); \
		return 1; \
	} \
	app.Update(); \
	app.Exit(); \
\
	return 0; \
}

//#ifdef _DEBUG
//
//#define DEPLOY(GameplaySystemType)\
//int main (void) \
//{ \
//	return Main_Internal<GameplaySystemType>(); \
//}
//
//#else
//
//#define DEPLOY(GameplaySystemType)\
//INT WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) \
//{ \
//	return Main_Internal<GameplaySystemType>(); \
//}
//
//#endif
//
//template <typename GameplaySystemType>
//int Main_Internal(void)
//{
//	App app;
//
//	bool initialized = app.Initialize<GameplaySystemType>();
//	if (!initialized)
//	{
//		app.Exit();
//		return 1;
//	}
//	app.Update();
//	app.Exit();
//
//	return 0;
//}

#endif

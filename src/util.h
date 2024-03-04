#pragma once

#include <iostream>
namespace NWin {
	#define display(msg) std::cout << (msg) << "\n";
	//TODO::Add line and file of the error
	#define WIN_CHECK2(expr, errorValue) if (((expr)) == errorValue) { display("Error at evaluation expression: " #expr)}

	#define WIN_CHECK(expr) WIN_CHECK2(expr, 0)

	#define DEFAULT_WINDOW_POSITION_SIZE ((int)0x80000000)
	typedef unsigned long Word;

	struct Vec2 {
		int x = 0;
		int y = 0;
	};

	struct Rect {
		Vec2 pos = { DEFAULT_WINDOW_POSITION_SIZE, DEFAULT_WINDOW_POSITION_SIZE };
		Vec2 size = { DEFAULT_WINDOW_POSITION_SIZE, DEFAULT_WINDOW_POSITION_SIZE };
	};

	extern Word bitOr(int number, Word args...);
}
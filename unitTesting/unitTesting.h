#pragma once

#include "util.h"

namespace UnitTesting {
#define ASSERT(expr, value) if ((expr) != value) {display("Test failed during" expr  "evaluation")}

	extern void simpleWindow();
	extern void keyboardTest();
	extern void testOpenGL3Window();
};
#include <iostream>
#include "gl_context.h"
#include "window.h"   
#include <windows.h>

#include "../unitTesting/unitTesting.h"   
int main() {
	UnitTesting::keyboardTest();
	UnitTesting::testOpenGL3Window();
}


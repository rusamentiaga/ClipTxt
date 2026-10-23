// TestMain.cpp : Runs the unit tests in UnitTest.h without starting the tray app.
//

#include <windows.h>
#include <tchar.h>
#include "UnitTest.h"

int main()
{
	// Same locale as the app, so accented characters behave the same way
	std::locale::global(std::locale(""));

	UnitTest Test;
	return Test.Run() ? EXIT_SUCCESS : EXIT_FAILURE;
}

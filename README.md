# ClipTxt

[![Build](https://github.com/rusamentiaga/ClipTxt/actions/workflows/build.yml/badge.svg)](https://github.com/rusamentiaga/ClipTxt/actions/workflows/build.yml)
[![Tests](https://github.com/rusamentiaga/ClipTxt/actions/workflows/tests.yml/badge.svg)](https://github.com/rusamentiaga/ClipTxt/actions/workflows/tests.yml)
[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](LICENSE)

ClipTxt is a small Windows tray app that turns everything you copy into plain
text. Copy from a web page, Word, Outlook or a PDF, and you paste just the
text: no fonts, colors, links, tables or hidden formatting.

It works on its own as soon as it starts. When you copy text, ClipTxt
replaces the clipboard contents with only the plain text. Copied images and
files are left untouched.

Optionally, ClipTxt can also transform the text as you copy it, for example
to change its case or swap decimal separators.

## Transformations

All transformations are optional and also output plain text.

| Menu entry                 | Example input            | Result                   |
|----------------------------|--------------------------|--------------------------|
| Paragraph Case             | `THIS IS AN EXAMPLE. AND MORE` | `This is an example. And more` |
| Title Camel Case           | `a study of the method`  | `A Study of the Method`  |
| Upper Case                 | `This is an example`     | `THIS IS AN EXAMPLE`     |
| Lower Case                 | `This Is An Example`     | `this is an example`     |
| Spaces for tabs            | `a b c`                  | `a⇥b⇥c`                  |
| Tabs for spaces            | `a⇥b⇥c`                  | `a   b   c`              |
| Dot per comma              | `12,5`                   | `12.5`                   |
| Comma per dot              | `12.5`                   | `12,5`                   |
| Remove dots, comma per dot | `7.584,93`               | `7584.93`                |

Title Camel Case is meant for paper titles. It leaves articles, prepositions
and conjunctions in lower case, in both English and Spanish.

Tabs for spaces replaces each tab with 3 spaces. Both tab conversions also
trim leading and trailing whitespace.

The number conversions also trim leading and trailing whitespace, which helps
when moving values between spreadsheets that use different decimal separators.

## Usage

1. Run `ClipTxt.exe`. It has no window, only an icon in the notification area.
2. Copy some text. The clipboard now holds it as plain text, ready to paste.
3. Optionally, right-click the icon and pick a transformation. Pick it again
   to turn it off and go back to plain text only.

Other menu options:

- **Disable** pauses ClipTxt, so copied text keeps its formatting.
- **Verbose** shows a notification each time text is transformed.
- **Exit** closes the app.

To start ClipTxt with Windows, put a shortcut to `ClipTxt.exe` in your Startup
folder (press `Win+R` and run `shell:startup`).

## Building

Requirements: Visual Studio 2026 with the **Desktop development with C++**
workload (toolset v145).

1. Open `ClipTxt.sln`.
2. Pick a configuration (`Release` / `x64` for normal use) and build.

From a Developer Command Prompt:

```
msbuild ClipTxt.sln /p:Configuration=Release /p:Platform=x64
```

To build with Visual Studio 2022, add `/p:PlatformToolset=v143`.

Debug builds open a console with trace output.

## Tests

The unit tests are in [UnitTest.h](UnitTest.h) and use no test framework.
The solution has a second project, `ClipTxtTests` in [Tests/](Tests/). It is a
small console program that runs the tests without starting the tray app.

Building the solution compiles the tests but doesn't run them. To build and
run them from any command prompt:

```
RunTests                  :: Debug x64
RunTests Release Win32
```

[RunTests.bat](RunTests.bat) finds MSBuild by itself, so it doesn't need a
Developer Command Prompt. It ends with `TESTS PASSED` or `TESTS FAILED`, and
its exit code is 0 or 1. If a check fails, the output shows where:

```
C:\...\UnitTest.h(119): FAILED: TabSpace.ModifyString(L"a\tb\tc") == L"a   b   c"
54/55 checks passed
```

After a build you can also run `x64\Debug\ClipTxtTests.exe` directly.

To add a test, write a `Run...` method in `UnitTest.h` that uses
`CHECK(expression)`, and call it from `Run()`.

## Adding a transformation

Each transformation is a class that derives from `ClipboardModifyStrategy`
(see [ClipboardModifyStrategy.h](ClipboardModifyStrategy.h)) and implements
`ModifyString`. To add one:

1. Write the class in a header next to related ones, for example
   [ChangeCase.h](ChangeCase.h) (case), [SpaceTab.h](SpaceTab.h) (spaces and
   tabs) or [DecimalSeparator.h](DecimalSeparator.h) (decimal separators).
2. Add a menu command ID in [resource.h](resource.h) and a menu entry under
   `IDR_TRAYICON_MENU` in [ClipTxt.rc](ClipTxt.rc).
3. Register it in the `MainWindow` constructor in [MainWindow.h](MainWindow.h):

   ```cpp
   m_AvailableStrategies[ID_MENU_MYTRANSFORM] = new MyTransform;
   ```

4. Add tests for it in [UnitTest.h](UnitTest.h).

## Contributing

Issues and pull requests are welcome. Before you submit a change, make sure
the solution builds; a successful build means the unit tests pass.

## License

Copyright (C) 2010-2026 Rubén Usamentiaga

ClipTxt is free software, released under the
[GNU General Public License v3.0](LICENSE).

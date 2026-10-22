# ClipTxt

[![Build](https://github.com/rusamentiaga/ClipTxt/actions/workflows/build.yml/badge.svg)](https://github.com/rusamentiaga/ClipTxt/actions/workflows/build.yml)
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
| Dot per comma              | `12,5`                   | `12.5`                   |
| Comma per dot              | `12.5`                   | `12,5`                   |
| Remove dots, comma per dot | `7.584,93`               | `7584.93`                |

Title Camel Case is meant for paper titles. It leaves articles, prepositions
and conjunctions in lower case, in both English and Spanish.

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

Debug builds open a console with trace output and run the unit tests in
`UnitTest.h` on startup.

## Adding a transformation

Each transformation is a class that derives from `ClipboardModifyStrategy`
(see [ClipboardModifyStrategy.h](ClipboardModifyStrategy.h)) and implements
`ModifyString`. To add one:

1. Write the class, for example in [ChangeCase.h](ChangeCase.h).
2. Add a menu command ID in [resource.h](resource.h) and a menu entry under
   `IDR_TRAYICON_MENU` in [ClipTxt.rc](ClipTxt.rc).
3. Register it in the `MainWindow` constructor in [MainWindow.h](MainWindow.h):

   ```cpp
   m_AvailableStrategies[ID_MENU_MYTRANSFORM] = new MyTransform;
   ```

## Contributing

Issues and pull requests are welcome. Please make sure the Debug build runs
the unit tests without assertion failures before you submit a change.

## License

Copyright (C) 2010-2026 Rubén Usamentiaga

ClipTxt is free software, released under the
[GNU General Public License v3.0](LICENSE).

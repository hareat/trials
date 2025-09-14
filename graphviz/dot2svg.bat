@echo off
REM Regenerate all SVG files from existing DOT files.

REM If Graphviz is not in the search path - PATH
SET DOT="C:\Program Files\Graphviz\bin\dot"
REM If Graphviz is included in the search path - PATH
REM SET DOT=dot

REM %f includes the extension and ~n extracts the basename
for /f %%f in ('dir /b *.dot') do (
    echo "%%f => %%~nf.svg"
    %DOT% -Tsvg -o%%~nf.svg %%f
)

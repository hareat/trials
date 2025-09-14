#!/bin/bash
# Regenerate all SVG files from existing DOT files.

# If Graphviz is not in the search path - PATH
#DOT=/usr/local/bin/dot
# If Graphviz is included in the search path - PATH
DOT=dot

# Convert all DOT files in the same directory.
for f in $(dirname "$0")/*.dot
do
    NAME="${f%.*}"  # the name without the extension
    echo "$f => $NAME.svg"
    $DOT -Tsvg -o"$NAME.svg" "$f"
done

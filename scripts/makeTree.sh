#!/opt/local/bin/bash
sed -n '/^----$/ { :a; n; p; ba; }' "$1" > "$1.tree"
python makeTrees.py "$1.tree" > "$1.dot"
dot -Tpng -o "$1.png" "$1.dot"
#dot -Tpdf -o "$1.pdf" "$1.dot"

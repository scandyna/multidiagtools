
original_dir="$PWD"

cp doc/man/src/img/* doc/man/html/img/

cd doc/man/src/
pandoc -f markdown -t html userman.txt -o ../html/userman.html
pandoc -f markdown -t html userman-testtool.txt -o ../html/userman-testtool.html

cd ../html/
pandoc -f html userman.html -o ../pdf/userman.pdf
pandoc -f html userman-testtool.html -o ../pdf/userman-testtool.pdf

cd "$original_dir"

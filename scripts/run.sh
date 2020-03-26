cd .. 
make
./tool | tee scripts/log.txt 
cd scripts
. makeTree.sh log.txt 

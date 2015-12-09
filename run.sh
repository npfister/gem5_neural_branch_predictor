
#./build/ALPHA_MESI_CMP_directory/gem5.debug --debug-flags="Perceptron,InOrderBPred" -d perceptronOut configs/spec2k6/run.py -b bzip2 --maxinsts=250000 --cpu-type=inorder --caches --pred-type="local" --global-hist-size=34
./build/ALPHA_MESI_CMP_directory/gem5.debug --debug-flags="Fetch,InOrderBPred" -d temp configs/spec2k6/run.py -b bzip2 --maxinsts=10000000 --cpu-type=inorder --caches --pred-type="hybridpg" --global-hist-size=4 --global-pred-size=4096

#./build/ALPHA_MESI_CMP_directory/gem5.debug -d gshareOut12 configs/spec2k6/run.py -b bzip2 --maxinsts=10000000 --cpu-type=inorder --caches --pred-type="gshare" --global-hist-size=12 --global-pred-size=4096

#./build/ALPHA_MESI_CMP_directory/gem5.debug -d localOut2 configs/spec2k6/run.py -b bzip2 --maxinsts=10000000 --cpu-type=inorder --caches --pred-type="local" --local-pred-size=4096

#./build/ALPHA_MESI_CMP_directory/gem5.debug -d hybridOut configs/spec2k6/run.py -b bzip2 --maxinsts=10000000 --cpu-type=inorder --caches --pred-type="hybridpg" --global-hist-size=12 --local-pred-size=4096


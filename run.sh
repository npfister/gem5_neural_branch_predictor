
#./build/ALPHA_MESI_CMP_directory/gem5.debug --debug-flags="Perceptron,InOrderBPred" -d perceptronOut configs/spec2k6/run.py -b bzip2 --maxinsts=250000 --cpu-type=inorder --caches --pred-type="local" --global-hist-size=34
./build/ALPHA_MESI_CMP_directory/gem5.debug -d perceptronOut configs/spec2k6/run.py -b gcc --maxinsts=2500000 --cpu-type=inorder --caches --pred-type="perceptron" --global-hist-size=34 --global-pred-size=8192

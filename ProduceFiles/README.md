Please check DataFormat.xlsx for the output format

The names of the 40 input modules are written in ModuleList.txt

1. Make 8BX ntuple:
root -l runReadTree.C++

2. Produce data sourcing files:
root -l runProduceFile.C++

3. Produce simple bank:
root -l runMakeBank.C++

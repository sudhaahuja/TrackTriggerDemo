Please check DataFormat.xlsx for the output format

The names of the 40 input modules are written in ../inputfiles/ModuleListBoard00.txt

* Make 8BX ntuple:
```
root -l runReadTree.C++
```
* Produce data sourcing file and PREF file:
```
root -l runProduceFile.C++
```
* Produce simple bank:
```
root -l runMakeBank.C++
```

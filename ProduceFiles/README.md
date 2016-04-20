Please check DataFormat.xlsx for DS and PRBF.2 output format

The input module lists for each board are written in ../inputfiles/ModuleListBoard??.txt
The input moudle list for TT27 is in ../inputfiles/ModuleList.txt

* Please tune some bools at the beginning of utils.C

* (Optional) Make 8BX ntuple:
```
root -l runReadTree.C++
```

* Produce data sourcing file and PREF.2 file:
```
root -l runProduceFile.C++
```
The output txt files will be produced under ProduceFiles/outputfiles

* Produce simple bank:
```
root -l runMakeBank.C++
```

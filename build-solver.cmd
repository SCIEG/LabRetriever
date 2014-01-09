@ECHO OFF

cd ../LikelihoodSolver
build.cmd

cd ../LabRetriever
rmdir /S /Q src/labr
xcopy /E /I ../LikelihoodSolver/target src/labr


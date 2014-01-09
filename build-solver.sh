#!/bin/bash

cd ../LikelihoodSolver
sh build.sh

cd ../LabRetriever
rm -rf src/labr
cp -R ../LikelihoodSolver/target src/labr


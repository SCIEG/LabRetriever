#-------------------------------------------------
#
# Project created by QtCreator 2012-09-29T00:13:04
#
#-------------------------------------------------

QT       += core gui

TARGET = LabRetrieverGUI
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    Configuration.cpp \
    lrmain.cpp \
    utils/ProbabilityUtil.cpp \
    utils/LikelihoodUtil.cpp \
    utils/FileReaderUtil.cpp \
    utils/DebugUtil.cpp \
    utils/AlleleUtil.cpp \
    LikelihoodSolver/OneSuspectTwoUnknownsLikelihoodSolver.cpp \
    LikelihoodSolver/OneSuspectOneUnknownLikelihoodSolver.cpp \
    LikelihoodSolver/OneSuspectNoUnknownsLikelihoodSolver.cpp \
    LikelihoodSolver/NoSuspectTwoUnknownsLikelihoodSolver.cpp \
    LikelihoodSolver/NoSuspectThreeUnknownsLikelihoodSolver.cpp \
    LikelihoodSolver/NoSuspectOneUnknownLikelihoodSolver.cpp \
    LikelihoodSolver/LikelihoodSolver.cpp \
    LikelihoodSolver/CachingSolver.cpp

HEADERS  += mainwindow.h \
    Configuration.h \
    lrmain.h \
    utils/ProbabilityUtil.h \
    utils/LikelihoodUtil.h \
    utils/FileReaderUtil.h \
    utils/DebugUtil.h \
    utils/AlleleUtil.h \
    LikelihoodSolver/LikelihoodSolver.h \
    LikelihoodSolver/CachingSolver.h

FORMS    += mainwindow.ui

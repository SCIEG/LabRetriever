
# Downloads

Currently 1.0.9 ALPHA release

[Mac](http://lab-retriever.googlecode.com/files/LabRetriever-1.0.9.dmg)

[Windows](http://lab-retriever.googlecode.com/files/LabRetriever-1.0.9.msi)

Linux (coming maybe? soon)

# Building it yourself

This is for developers who want to make additions or fixes to the project.

## Prerequisites

[make](http://www.gnu.org/software/make/)

[gcc](http://gcc.gnu.org/)

On windows use MinGW to install make/gcc (put MinGW on your path and run mingw32-make instead of just 'make' below).

[TideSDK](http://www.tidesdk.org/) - Both the latest SDK version AND the TideSDK Developer.

[Instructions on installing / setting up TideSDK](http://tidesdk.multipart.net/docs/user-dev/generated/#!/guide/getting_started) - which is especially important to read for Windows.


## Building / Running

Compile the labr program:

    cd src
    make

Import the Tide project (src/Tide) in TideSDK Developer.

Click "Test & Package" tab

Click "Launch App"


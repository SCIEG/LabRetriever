
# Downloads

Currently 1.0.5 ALPHA release

[Mac](http://lab-retriever.googlecode.com/files/LabRetriever-1.0.5.ALPHA.dmg)

[Windows](http://lab-retriever.googlecode.com/files/LabRetriever-1.0.5.msi)

Linux (coming maybe? soon)

# Building it yourself

This is for developers who want to make additions or fixes to the project.

## Prerequisites

[make](http://www.gnu.org/software/make/)

[gcc](http://gcc.gnu.org/)

On windows use cygwin to install make/gcc but use a regular 'cmd' shell to compile.

[TideSDK](http://www.tidesdk.org/) - Both the latest SDK version AND the TideSDK Developer.

[Instructions on installing / setting up TideSDK](http://tidesdk.multipart.net/docs/user-dev/generated/#!/guide/getting_started)


## Building / Running

Compile the labr program:

    cd src
    make

Import the Tide project (src/Tide) in TideSDK Developer.

Click "Test & Package" tab

Click "Launch App"


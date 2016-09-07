# TarotClub, the free French Tarot card game!

## Introduction

TarotClub is a Tarot card game (not the divination one) playable on any computer 
with Mac OS X, Windows or Linux.
The Tarot card game is mostly played in french-spoken countries. It uses a specific 
deck of cards including standard card figures plus the Knight and 22 trumps. The complete 
rules of the Tarot card game is available in Wikipedia.

This project is a tentative to create a nice open source version of this game, playable in 
various operating system and with a modular design.

Project: https://bitbucket.org/tarotclub/tarotclub
Website: http://www.tarotclub.fr
Contact: support@tarotclub.fr

Thanks for your help and your support.
  
# Installation guide

## Installation on Windows

Please use the provided setup executable. It should guide you seamlessly for a proper installation of the game.

## Installation on Debian / Arch / Fedora

Please use the provided package. It has been tested on the last version of each system and is dependant of the Qt libraries (Qt5.x).

## Installation on other systems

For the moment, no any specific package is available for other Linux or BSD distribution. Consider building from the source code.
Since I have no Macintosh computer, I cannot provide the game on this system (try to build from the source).

# Building the game

See the official documentation on http://www.tarotclub.fr

# ICL dependency

## Add Subtree
git subtree add --prefix src/icl https://github.com/belegar/icl master --squash

## Update Subtree
git subtree pull --prefix src/icl https://github.com/belegar/icl master --squash


Now we can use the subtree push command like the following:
git subtree push --prefix=.vim/bundle/tpope-vim-surround/ durdn-vim-surround master



# Credits / contributions

  * Hizoka, for all of his remarks, game testing and contributions
  * cledesol, for all his remarks and game testing
  * Duktape author, for his Javascript engine and helps

# TCDS  - TarotClub Dedicated Server #

This repository contains project files to build the TarotClub dedicated server executable.

# Building TarotClub Dedicated Server (TCDS)

There are three projects available:

  * A Visual Studio 2013 project file (Windows only)
  * A Qt project file
  * A hand-made Makefile

## Building using Visual Studio

Just open the solution file and build the target.

## Building using Qt

Either use QtCreator IDE or run on the command line:

```
qmake tcds.pro
make
```

## Building using the hand-made Makefile
Another way is to use the hand-made "Makefile" available at the directory root. Some batch/sh command-line
scripts are available.

Example under Windows:
```
cd tcds
build_tcds.bat
```

# TODO

  * Add a maximum play delay for network players, if timeout a random card is played
  * Log all the messages (per table and of the hall, one file per day)
  * Add memory stats and players stats in the JSON status command result

# HISTORY OF CHANGES

## Version 1.2.0

  * Added a default Log limit to 50MB per log file
  * Corrected a critical bug in the TCP server (bad socket management)
  * Added WebSocket protocol support

## Version 1.1.0

  * Integration of the CouchDb driver and deal JSON file upload at each end of deal
  * New tournament configuration (each deal can be configured individually)
  * Updated SQLite to 3.8.10.1
  * Removed dedicated server web interface
  * Added new console TCP/IP interface
  * Integration of AI tournament

## Version 1.0.0

  * Moved bot configuration into client parameters
  * Use of dedicated version for TCDS



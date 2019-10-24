# (Semi) Automatic Infinite Entertainment System

The goal of this project is to create a system capable of providing continuous video content based on the environment in which a user is inserted. The application has 2 available modes: one for the operator and another for the user. The operator mode allows an operator to explore a collection of videos, check their correspondent metadata and control the content provided to the user by creating playlists of videos to be displayed based on the data retrieved from the user’s environment. This data is captured on the user’s mode of the application through a camera and is then processed based on the amount of people's faces in front of the camera and displays videos that will change based on the user’s current environment. The user can skip videos by making a noise.

### Folder Content
```
README.md -> this file

(Semi) Automatic Infinite Entertainment System.pdf -> the project report

openFrameworks/ -> openFrameworks folder

openFrameworks/apps/myApps/SemiAutomaticInfiniteEntertainmentSystem/ -> application folder

../../../SemiAutomaticInfiniteEntertainmentSystem/src/ -> the application's code

../../../SemiAutomaticInfiniteEntertainmentSystem/bin/SemiAutomaticInfiniteEntertainmentSystem.app -> the application

../../../SemiAutomaticInfiniteEntertainmentSystem/bin/data/ -> the application's multimedia content
```

## Built With

* [openFrameworks](https://openframeworks.cc) - an open source C++ toolkit designed to assist the creative process by providing a simple and intuitive framework for experimentation.
### Addons
* [ofxOpenCv](https://openframeworks.cc/documentation/ofxOpenCv/) - Computer vision utils based on the OpenCv library.
* [ofxXmlSettings](https://openframeworks.cc/documentation/ofxXmlSettings/) - Simple XML loader and saver.
* [ofxCV](https://github.com/kylemcdonald/ofxCv) - Alternative approach to interfacing with OpenCv from openFrameworks.
* [ofxDatGui](https://github.com/braitsch/ofxDatGui) - Simple to use, fully customizable, high-resolution graphical user interface for openFrameworks.

### Note:

* The application was made to run on macOS Mojave version 10.14.6 (18G95) and the audio detection feature will not work on other operating systems.
* The application must be compiled before being able run on other operating systems.

### Copyrights
I do not own any of the videos used in the project. All Rights Reserved to BlockBerryCreative.

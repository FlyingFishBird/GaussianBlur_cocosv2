A Guassian blur class for cocos2d-x v2 use GLSH
===============================================
## file list
	├── Classes
	│   ├── FilterNode.cpp
	│   ├── FilterNode.h
	│   ├── HelloWorldScene.cpp
	│   └── HelloWorldScene.h
	├── LICENSE
	├── README.md
	└── Resources
	    ├── a.png			
	    ├── bg.png
	    ├── CloseNormal.png
	    ├── CloseSelected.png
	    └── shaders
	        ├── hblur.fsh
	        └── vblur.fsh
## how to use it
	1)copy Resources/shaders to your resources path
	2)put FileterNode.h and FilterNode.cpp in your source code path
	3)at the init time do call GausianBlur::do_ready() to load shaders
	  at the end time do call GaussianBlur::do_free() to free shader strings
	  do_ready() at AppDelegate and do_free() in the exit is suggested.
	4)the sample code in the HelloWorld.cpp


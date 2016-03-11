# Patch Matrix

## JACK Matrix Patchbay

### Build status

[![Build Status](https://travis-ci.org/OpenMusicKontrollers/patchmatrix.svg?branch=master)](https://travis-ci.org/OpenMusicKontrollers/patchmatrix)

### Screenshot
![Screenshot](https://raw.githubusercontent.com/OpenMusicKontrollers/patchmatrix/master/patchmatrix_screeny.png "PatchMatrix Screenshot")

### Dependencies

* [JACK](http://jackaudio.org/) (JACK audio connection kit)
* [SQLite3](https://www.sqlite.org/) (SQL database engine)
* [Elementary](http://docs.enlightenment.org/auto/elementary/) (EFL UI toolkit)

### Build / install

	git clone https://github.com/OpenMusicKontrollers/patchmatrix.git
	cd patchmatrix 
	mkdir build
	cd build
	cmake ..
	make
	sudo make install

### License

Copyright (c) 2016 Hanspeter Portner (dev@open-music-kontrollers.ch)

This is free software: you can redistribute it and/or modify
it under the terms of the Artistic License 2.0 as published by
The Perl Foundation.

This source is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
Artistic License 2.0 for more details.

You should have received a copy of the Artistic License 2.0
along the source as a COPYING file. If not, obtain it from
<http://www.perlfoundation.org/artistic_license_2_0>.

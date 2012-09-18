sudo apt-get install cmake libpthread-stubs0-dev libgl1-mesa-dev libx11-dev libxrandr-dev libfreetype6-dev libglew1.5-dev libjpeg8-dev libsndfile1-dev libopenal-dev
cmake -G "Unix Makefiles" -D CMAKE_BUILD_TYPE=Release -D BUILD_SHARED_LIBS=TRUE .
make
sudo make install
cmake -G "Unix Makefiles" -D CMAKE_BUILD_TYPE=Debug -D BUILD_SHARED_LIBS=TRUE .
make
sudo make install


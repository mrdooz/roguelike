cmake -G "Unix Makefiles" -D CMAKE_BUILD_TYPE=Release -D BUILD_SHARED_LIBS=TRUE .
make
sudo make install
cmake -G "Unix Makefiles" -D CMAKE_BUILD_TYPE=Debug -D BUILD_SHARED_LIBS=TRUE .
make
sudo make install


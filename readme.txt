Various installation tips

OSX

protobuf, compiling with clang, libc++
--------------------------------------

./configure CC=clang CXX=clang++ CXXFLAGS='-stdlib=libc++ -O3 -g' LDFLAGS='-stdlib=libc++' --prefix=/opt/local

https://gist.github.com/BennettSmith/7150245
I commented out the sed stuff that changed the namespace, because I'm not using both iOS and OSX.

boost, compiling with clang, libc++
-----------------------------------
I don't know the correct location to install Boost, but I like the idea of dumping stuff
in /opt/local so that they can be removed without trouble.
For this reason I had to run b2 with sudo

./bootstrap.sh --prefix=/opt/local/boost
./b2 clean
sudo ./b2 toolset=clang cxxflags="-stdlib=libc++" linkflags="-stdlib=libc++" install


sfml
----
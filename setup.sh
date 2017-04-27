sudo apt-get update
sudo apt-get -y upgrade
sudo apt-get -y install python-dev python-xlib libx11-dev

#wget http://www.airspayce.com/mikem/bcm2835/bcm2835-1.52.tar.gz
tar xvfz bcm2835-1.52.tar.gz
cd bcm2835-1.52
./configure
make
sudo make install

cd ..

gcc -fPIC -std=c11 -shared -o libkedei.so libkedei.c -L. -lbcm2835 
gcc -fPIC -std=c11 -shared -I/usr/include/python2.7/ -o lcdmodule.so lcdmodule.c -L. -lkedei
gcc -shared -O3 -lX11 -fPIC -Wl,-soname,prtscn -o prtscn.so prtscn.c

sudo cp lcdmodule.so /usr/lib/lcdmodule.so
sudo cp libkedei.so /usr/lib/libkedei.so
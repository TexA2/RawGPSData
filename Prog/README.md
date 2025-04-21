#Сырая прослушка GPS через USB
##Для работы требуется библиотека (https://github.com/kosma/minmea)

##Установка библиотеки
```bash
mkdir build
cd build
cmake ../
make
sudo make install

 
##Сборка
```bash
g++ UsbGps.cpp -lminme

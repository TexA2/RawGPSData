#Сырая прослушка GPS через USB
##Для работы требуется библиотека (https://github.com/kosma/minmea)

##Установка библиотеки
```bash
git clone git@github.com:kosma/minmea.git
cd minmea
mkdir build
cd build
cmake ../
make
sudo make install 
```
  
##Сборка
```bash
g++ UsbGps.cpp -lminme

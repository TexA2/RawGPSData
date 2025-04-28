#include <cstddef>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <stdio.h>
#include <iomanip>
#include <minmea.h>

const char* port  = "/dev/ttyACM0"; // вывести в лаунч файл


struct GPSMeasurement {
    double latitude;
    double longitude;
    double altitude;
};

class UsbGpsListener {
public:
    UsbGpsListener()
    {
        PortReadOpen();
    }

    ~UsbGpsListener()
    {
        fclose(file); 
        close(fd);
    }

    void run();

    double getLatitude();
    double getLongitude();
    double getAltitude();
    virtual void GpsDataPrint();

protected:
    void ReadGpsData();
    char buffer[256];

private:
    void InstallSettingPort();
    double NmeaToDecimal(double nmea_coord);
    void PortReadOpen();

    int fd;
    FILE* file;
    termios tty;

    GPSMeasurement GPS;

};

double
UsbGpsListener::getLatitude()  { return GPS.latitude; }

double
UsbGpsListener::getLongitude() { return GPS.longitude;}

double
UsbGpsListener::getAltitude()  { return GPS.altitude; }


void
UsbGpsListener::run()
{
    while(true)
        ReadGpsData();
}

double 
UsbGpsListener:: NmeaToDecimal(double nmea_coord) {

    double first_part = floor(nmea_coord / 1e7);
    
    double remainder = nmea_coord - first_part * 1e7; 
    double fractional_part = (remainder / 60 )  / 1e5;    
    
    return first_part + fractional_part;
}
    

void
UsbGpsListener::PortReadOpen()
{
    fd = open(port, O_RDWR | O_NOCTTY | O_NDELAY);

    if (fd == -1)
    {
        std::cout << "Порт не найдет/не подключен!" << std::endl;
        std::exit(-1);
    }

    std::cout << "Подключен к --> " << port << std::endl;
    InstallSettingPort();

    file = fdopen(fd, "r");
}

void 
UsbGpsListener::InstallSettingPort()
{
    tcgetattr(fd, &tty);
    cfsetospeed(&tty, B9600);  // Скорость 9600 baud 
    cfsetispeed(&tty, B9600);

    tty.c_cflag &= ~PARENB;     // Без бита чётности
    tty.c_cflag &= ~CSTOPB;     // 1 стоп-бит
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;         // 8 бит данных
    tty.c_cflag &= ~CRTSCTS;    // Без аппаратного управления потоком
    tty.c_cflag |= CREAD | CLOCAL;

    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);  // Неканонический режим
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);          // Без программного управления потоком
    tty.c_oflag &= ~OPOST;                           // Без постобработки

    tcsetattr(fd, TCSANOW, &tty);
}

void
UsbGpsListener::GpsDataPrint()
{
    std::cout << std::setprecision(9)  << GPS.latitude  << " "
    << std::setprecision(9)  << GPS.longitude << " "
    << GPS.altitude << std::endl;
}

void
UsbGpsListener::ReadGpsData()
{

    while (fgets(buffer, sizeof(buffer), file))
    {
        //minmea_sentence_rmc frame;
        //minmea_sentence_gst frame;
        //minmea_sentence_gsv frame;
        //minmea_sentence_gll frame;
        minmea_sentence_gga frame_gga;

		if(buffer[3] == 'G' && buffer[4] == 'G' && buffer[5] == 'A')
		{
			minmea_parse_gga(&frame_gga,buffer);

            GPS.latitude  = NmeaToDecimal(frame_gga.latitude.value);
            GPS.longitude = NmeaToDecimal(frame_gga.longitude.value);
            GPS.altitude  = frame_gga.altitude.value / 10.0;
            GpsDataPrint();
		}
    }
}

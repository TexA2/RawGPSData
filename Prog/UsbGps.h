#include <cstddef>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <stdio.h>
#include <iomanip>
#include <minmea.h>

const char* port  = "/dev/ttyACM0";

class UsbGpsListener
{
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

    private:
    void InstallSettingPort();
    double nmea_to_decimal(double nmea_coord);
    void PortReadOpen();
    void ReadGpsData();

    int fd;
    FILE* file;
    termios tty;
    char buffer[256];

};

void
UsbGpsListener::run()
{
    while(true)
        ReadGpsData();
}

double 
UsbGpsListener::nmea_to_decimal(double nmea_coord) {

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
    cfsetospeed(&tty, B9600);  // Скорость 38400 baud (подставьте свою)
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
UsbGpsListener::ReadGpsData()
{


    while (fgets(buffer, sizeof(buffer), file))
    {
        //minmea_sentence_rmc frame;
        //minmea_sentence_gst frame;
        //minmea_sentence_gsv frame;
        //minmea_sentence_gll frame_gll;
        minmea_sentence_gga frame_gga;

		if(buffer[3] == 'G' && buffer[4] == 'G' && buffer[5] == 'A')
		{
			minmea_parse_gga(&frame_gga,buffer);

            double latitude_decimal = nmea_to_decimal(frame_gga.latitude.value);
            double longitude_decimal = nmea_to_decimal(frame_gga.longitude.value);

            std::cout << std::setprecision(9)  << latitude_decimal  << " "
                      << std::setprecision(9)  << longitude_decimal << " "
                      << frame_gga.altitude.value / 10.0  << std::endl;
		}
    }
}

#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <minmea.h>
#include <stdio.h>
#include <iomanip>

#define INDENT_SPACES "  "


// Функция для преобразования NMEA-координат в десятичные градусы
double nmea_to_decimal(double nmea_coord, char direction) {
    // Извлекаем градусы и минуты
    int degrees = (int)(nmea_coord / 100);
    double minutes = nmea_coord - degrees * 100;
    
    // Переводим в десятичные градусы
    double decimal = degrees + minutes / 60.0;
    
    // Учитываем направление (N/S, E/W)
    if (direction == 'S' || direction == 'W') {
        decimal = -decimal;
    }
    
    return decimal;
}


int main(){

    const char* file_name  = "GPS.txt";
    int fd = open(file_name, O_RDWR );

	FILE* file = fdopen(fd, "r");

    if (fd == -1)
    {
        std::cout << "Файл не найден" << std::endl;
        std::exit(-1);
    }


// Чтение данных
    char buffer[256];
	int n = 1;
	int count_line;
	int count_gga;


    
    while (fgets(buffer, sizeof(buffer), file)) {

        // struct minmea_sentence_rmc frame;
        minmea_sentence_gga frame_gga;
        // struct minmea_sentence_gst frame;
        // struct minmea_sentence_gsv frame;
        //minmea_sentence_gll frame_gll;

				if(buffer[3] == 'G' && buffer[4] == 'G' && buffer[5] == 'A')
				{
					//std::cout << buffer;
					// minmea_parse_gll(&frame_gll,buffer);
					// std::cout << "GGL: " <<frame_gll.latitude.value << " " << frame_gll.longitude.value << std::endl;

					minmea_parse_gga(&frame_gga,buffer);

					std::cout << "GGA: "  << frame_gga.latitude.value / double(frame_gga.latitude.scale) 
                                                        << " " << frame_gga.longitude.value / double(frame_gga.longitude.scale) 
                                                        << " " << frame_gga.altitude.value / 10.0  << std::endl;
					++count_gga;
                    double latitude_decimal = nmea_to_decimal(frame_gga.latitude.value / double(frame_gga.latitude.scale), 'N');
                    double longitude_decimal = nmea_to_decimal(frame_gga.longitude.value / double(frame_gga.longitude.scale) , 'E');
                    std::cout << std::setprecision(9) <<  "Преобразованные: " << latitude_decimal/ 1e5 << " " << longitude_decimal / 1e5 << std::endl;
                    std::cout << std::setprecision(9) << "GGA: "<< frame_gga.latitude.scale;

				}
                ++count_line;

    }
	// std::cout << "строк: " << count_line << std::endl;
	// std::cout << "GGA найдено:" << count_gga << std::endl;
    close(fd);
    return 0;
}
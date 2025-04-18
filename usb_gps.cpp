#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>



int main(){

    const char* port  = "/dev/ttyACM0";
    int fd = open(port, O_RDWR | O_NOCTTY | O_NDELAY);

    if (fd == -1)
    {
        std::cout << "Порт не найдет/не подключен!" << std::endl;
        std::exit(-1);
    }

//Настройка порта
    termios tty;
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

// Чтение данных
    char buffer[256];
    while (true) {
        int n = read(fd, buffer, sizeof(buffer) - 1);
        if (n > 0) {
            buffer[n] = '\0';
            std::cout << buffer;
        }
    }

    close(fd);
    return 0;
}
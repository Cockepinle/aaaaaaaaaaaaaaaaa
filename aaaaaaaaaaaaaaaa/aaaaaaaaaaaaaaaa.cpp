#define WIN32_LEAN_AND_MEAN // Оптимизация Windows.h, исключение ненужных структур и определений
#include <iostream>  // Стандартная библиотека для ввода/вывода
#include <Windows.h> // Библиотека Windows API
#include <WinSock2.h> // Библиотека Windows Sockets
#include <WS2tcpip.h> // Библиотека для работы с адресами и сокетами

using namespace std; // Использование имен из пространства имен std

int main() {
    SetConsoleCP(1251); // Установка кодовой страницы консоли в 1251 (кириллица)
    setlocale(0, ""); // Установка локали для корректного отображения символов

    WSADATA wsaData; // Структура для хранения информации о библиотеке Winsock
    ADDRINFO hints; // Структура для хранения информации о типе сокета
    ADDRINFO* addrResult; // Указатель на структуру с информацией о полученных адресах
    SOCKET ClientSocket = INVALID_SOCKET; // Сокет для связи с клиентом
    SOCKET ListenSocket = INVALID_SOCKET; // Сокет для прослушивания входящих соединений

    const char* sendBuffer = "Hello from Server"; // Сообщение для отправки клиенту
    char recvBuffer[512]; // Буфер для хранения полученного сообщения

    int result = WSAStartup(MAKEWORD(2, 2), &wsaData); // Инициализация Winsock, версия 2.2
    if (result != 0) { // Проверка успешной инициализации
        cout << "WSAStartup falied" << endl;
        return 1; // Выход с ошибкой
    }

    ZeroMemory(&hints, sizeof(hints)); // Обнуление структуры hints
    hints.ai_family = AF_INET; // Семейство адресов: IPv4
    hints.ai_socktype = SOCK_STREAM; // Тип сокета: TCP
    hints.ai_protocol = IPPROTO_TCP; // Протокол: TCP
    hints.ai_flags = AI_PASSIVE; // Флаг: использовать адрес, назначенный операционной системой

    result = getaddrinfo(NULL, "788", &hints, &addrResult); // Получение информации о доступных адресах, порт 788
    if (result != 0) { // Проверка успешного получения адресов
        cout << "getaddrinfo faled" << endl;
        return 1; // Выход с ошибкой
    }

    ListenSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol); // Создание сокета
    if (ListenSocket == INVALID_SOCKET) { // Проверка успешного создания сокета
        cout << "socket creation with error" << endl;
        freeaddrinfo(addrResult); // Освобождение ресурсов, связанных с адресами
        WSACleanup(); // Очистка ресурсов Winsock
        return 1; // Выход с ошибкой
    }

    result = bind(ListenSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen); // Связывание сокета с адресом
    if (ListenSocket == SOCKET_ERROR) { // Проверка успешного связывания
        cout << "bigging connect falied" << endl;
        freeaddrinfo(addrResult); // Освобождение ресурсов
        WSACleanup(); // Очистка ресурсов
        return 1; // Выход с ошибкой
    }

    result = listen(ListenSocket, SOMAXCONN); // Запуск прослушивания входящих соединений
    if (result == SOCKET_ERROR) { // Проверка успешного запуска прослушивания
        cout << "listening falied" << endl;
        freeaddrinfo(addrResult); // Освобождение ресурсов
        WSACleanup(); // Очистка ресурсов
        return 1; // Выход с ошибкой
    }

    ClientSocket = accept(ListenSocket, NULL, NULL); // Принятие входящего соединения
    if (result == SOCKET_ERROR) { // Проверка успешного принятия соединения
        cout << "accepting falied" << endl;
        freeaddrinfo(addrResult); // Освобождение ресурсов
        WSACleanup(); // Очистка ресурсов
        return 1; // Выход с ошибкой
    }
    closesocket(ListenSocket); // Закрытие сокета для прослушивания

    do { // Цикл обработки сообщений
        ZeroMemory(recvBuffer, 512); // Обнуление буфера для приема сообщения
        result = recv(ClientSocket, recvBuffer, 512, 0); // Получение сообщения от клиента
        if (result > 0) { // Если сообщение получено
            cout << "Получено" << result << "байт" << endl;
            cout << "Получено" << recvBuffer << endl;

            result = send(ClientSocket, sendBuffer, (int)strlen(sendBuffer), MSG_DONTROUTE); // Отправка сообщения клиенту
            if (ClientSocket == SOCKET_ERROR) { // Проверка успешной отправки
                cout << "send FAILED" << endl;
                freeaddrinfo(addrResult); // Освобождение ресурсов
                WSACleanup(); // Очистка ресурсов
                return 1; // Выход с ошибкой
            }
        }
        else if (result == 0) { // Если result == 0, клиент закрыл соединение
            cout << "Connection closed" << endl;
        }
        else { // Если result меньше 0, ошибка при приеме сообщения
            cout << "Не было получено сообщение" << endl;
        }
    } while (result > 0); // Повторять цикл, пока result > 0

    result = shutdown(ClientSocket, SD_SEND); // Закрытие сокета для отправки данных
    if (ClientSocket == SOCKET_ERROR) { // Проверка успешного закрытия
        cout << "shutdown error" << endl;
        freeaddrinfo(addrResult); // Освобождение ресурсов
        WSACleanup(); // Очистка ресурсов
        return 1; // Выход с ошибкой
    }
    closesocket(ClientSocket); // Закрытие сокета для связи с клиентом
    freeaddrinfo(addrResult); // Освобождение ресурсов, связанных с адресами
    WSACleanup(); // Очистка ресурсов Winsock
    return 0; // Возврат 0 - успешное завершение
}
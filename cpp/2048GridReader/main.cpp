
#include <QApplication>
#include <QProcess>
#include <QString>
#include <QStringList>
#include <QThread>
#include <QPixmap>
#include <QDesktopWidget>
#include <QRgb>
#include <QColor>
#include <QEvent>
#include <QKeyEvent>

#include <iostream>
#include <list>
#include <tuple>

#include <windows.h>

#include "../GameMatrix.h"

void treatScreen(const QImage &desk);
bool goodPixel(const QImage &img, int x, int y);
void apply(move m);
bool closeRGB(const QRgb &r1, const QRgb &r2);

const int ColorThreshold = 5;
const QString WebBrowser = "C:/Program Files (x86)/Mozilla Firefox/firefox.exe";
const QString GameUrl = "https://gabrielecirulli.github.io/2048/";
std::list<std::tuple<QRgb, int>> colorMapping = {
    std::make_tuple(0xFFCDC0B4, 0),
    std::make_tuple(0xFFEEE4DA, 2),
    std::make_tuple(0xFFEDE0C8, 4),
    std::make_tuple(0xFFF2B179, 8),
    std::make_tuple(0xFFF59563, 16),
    std::make_tuple(0xFFF67C5F, 32),
    std::make_tuple(0xFFF65E3B, 64),
    std::make_tuple(0xFFEDCF72, 128),
    std::make_tuple(0xFFEDCC61, 256),
    std::make_tuple(0xFFEDC850, 512),
    std::make_tuple(0xFFEDC53F, 1024)};

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Start game process in the browser
    QStringList params;
    params << GameUrl;
    QProcess gameProcess(&a);
    qDebug("Starting web browser");
    gameProcess.start(WebBrowser, params);

    // Read and play
    QThread::sleep(4);
    while (true)
    {
        QPixmap screen = QPixmap::grabWindow(QApplication::desktop()->winId());
        treatScreen(screen.toImage());
        QThread::msleep(100);
    }

    return a.exec();
}

void treatScreen(const QImage &desk)
{
    /*QImage copy(desk);*/

    // get grid size
    int x = desk.width() / 2 - 100; // cheat to avoid score
    int top = 0;
    int bottom = desk.height()-1;
    int left = 0;
    int right = desk.width()-1;
    while (!goodPixel(desk, x, top)) { ++top; }
    while (!goodPixel(desk, x, bottom)) { --bottom; }
    int y = (top + bottom) / 2;
    while (!goodPixel(desk, left, y)) { ++left; }
    while (!goodPixel(desk, right, y)) { --right; }

    std::cout << top << " " << bottom << " " << left << " " << right << std::endl;

    // get cell size
    int spacing = 0;
    y = (bottom - top) / 8 + top;
    while (goodPixel(desk, left+spacing, y)) { ++spacing; }
    int cellDim = (right - left - 5*spacing) / 4;

    // get cell images
    uint values[4][4] = {static_cast<uint>(-1)};
    for (int i=0; i<4; ++i)
    {
        for (int j=0; j<4; ++j)
        {
            QRgb currPix = desk.pixel(left+spacing+(spacing+cellDim)*j+5,
                                      top+spacing+(spacing+cellDim)*i+5);
            /*copy.setPixel(left+spacing+(spacing+cellDim)*j+5,
                          top+spacing+(spacing+cellDim)*i+5,
                          0xFFFF00FF);*/
            for (auto t : colorMapping)
                if (closeRGB(std::get<0>(t), currPix))
                    values[i][j] = std::get<1>(t);

            if (values[i][j] == static_cast<uint>(-1))
            {
                QApplication::beep();
                /*std::cout << std::dec << "Please add 0x" << std::hex << currPix << " in " << i << " " << j << " for future mapping." << std::endl;
                int value;
                std::cin >> value;*/
                return;
            }
            std::cout << std::dec << values[i][j] << "\t";
        }
        std::cout << std::endl;
    }
    /*copy.save("C:/tmp/test.png", "png");*/
    std::cout << std::endl;

    GameMatrix gm(values);
    apply(gm.get_best_move());
}

bool goodPixel(const QImage &img, int x, int y)
{
    //return closeRGB(img.pixel(x, y), 0xFFBEAAA0) == 0;
    QRgb currPix = img.pixel(x, y);
    return ((qRed(currPix) <= 200 && qRed(currPix) >= 180) &&
        (qGreen(currPix) <= 185 && qGreen(currPix) >= 165) &&
        (qBlue(currPix) <= 170 && qBlue(currPix) >= 150));
}

void apply(move m)
{
    int key = static_cast<Qt::Key>(-1);
    switch (m)
    {
    case UP:
        key = VK_UP;
        break;
    case DOWN:
        key = VK_DOWN;
        break;
    case LEFT:
        key = VK_LEFT;
        break;
    case RIGHT:
        key = VK_RIGHT;
        break;
    }

    INPUT ip;
    ip.type = INPUT_KEYBOARD;
    ip.ki.wScan = 0;
    ip.ki.time = 0;
    ip.ki.dwExtraInfo = 0;

    ip.ki.wVk = key;
    ip.ki.dwFlags = 0; // 0 for key press
    SendInput(1, &ip, sizeof(INPUT));

    ip.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
    SendInput(1, &ip, sizeof(INPUT));
}

bool closeRGB(const QRgb &r1, const QRgb &r2)
{
    return std::abs(qRed(r1) - qRed(r2)) +
           std::abs(qGreen(r1) - qGreen(r2)) +
           std::abs(qBlue(r1) - qBlue(r2)) < ColorThreshold;
}

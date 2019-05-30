#include "Truck.hpp"

#include <cmath>
#include <stdlib.h>

Truck::Truck(int N, int C, int X, int Y, int s, float sp, Warehouse* w, char dC, int tO, TrafficLights* tL)
{
    isFull = false;
    nr = N;
    capacity = C;
    status = s;
    x = X;
    y = Y;
    speed = sp;
    warH = w;
    dirC = dC; 
    timeOperation = tO;
    threadT = std::thread(&Truck::Move, this);
    tLights = tL;
}

void Truck::Load()
{
    int div = 10;
    for(int i; i<div; i++)
    {
        //std::this_thread::sleep_for(std::chrono::milliseconds(timeOperation/div));
        status += 100/div;
    }
    status = 0;
}

void Truck::Unload()
{
    int div = 10;
    for(int i; i<div; i++)
    {
        //std::this_thread::sleep_for(std::chrono::milliseconds(timeOperation/div));
        status += 100/div;
    }
    status = 0;
}

void Truck::Move()
{
    double dx;
    bool moved;
    double dy;
    short direction = 1; //zmienna odpowiedzialna za kierunek jazdy ciężarówki, 1 to prawo, -1 to lewo
    
    while (true)
    {
        if((x >= warH->roadY+warH->roadLength-1 && dirC == 'E') || (x <= 2 && dirC == 'W')) // ciężarówka zawraca
        {
            direction *= -1;
            if(dirC == 'E')
            {
                dirC = 'W';
                EntryWarehouse();
            }
            else
            {
                dirC = 'E';
                EntryHarbor();
            }
        }

        dx = x + 1 * direction;
        moved = false;
        dy = y;

        if(x == warH->roadX-3)//wyjazd z portu, ciężarówka czeka na pozwolenie wyjazdu
        {
            tLights->mRoad.lock();
            EntryRoad();
            tLights->mRoad.unlock();
        }

        while(!moved)
        {
            warH->mutexWarh.lock();

            if(warH->arr_trucks[ceil(dx)][ceil(dy)] == 0)
            {
                moved = true;

                warH->arr_trucks[ceil(dx)][ceil(dy)] = nr;
                warH->arr_trucks[x][y] = 0;
                
                y = ceil(dy);
                x = ceil(dx);
            }
            else
            {
                //std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }

            warH->mutexWarh.unlock();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(150 + (rand() % 100)));
    }
}

void Truck::EntryWarehouse() //rozpoczyna rozładunek, po wyjeździe ciężarówka jest pas wyżej
{
    Load();
    warH->mutexWarh.lock();
    warH->arr_trucks[x][y] = 0;
    y = y + 2;
    warH->arr_trucks[x][y] = nr;
    warH->mutexWarh.unlock();
}

void Truck::EntryHarbor() //rozpoczyna załadunek, po wyjeździe ciężarówka jest pas niżej
{
    Unload();
    warH->mutexWarh.lock();
    warH->arr_trucks[x][y] = 0;
    y = y - 2;
    warH->arr_trucks[x][y] = nr;
    warH->mutexWarh.unlock(); 
}

void Truck::EntryRoad()
{
    /*double dy;
    double dx;
    int dir = 1;
    bool moved;

    if(y > warH->roadY)
        dir = -1;

    warH->mutexWarh.lock();
    warH->arr_trucks[x][y] = 0;
    dx = x + 1;
    warH->arr_trucks[dx][y] = nr;
    warH->mutexWarh.unlock();
    std::this_thread::sleep_for(std::chrono::milliseconds(150 + (rand() % 100)));

    while (y != warH->roadY)
    {
        moved = false;
        dy = y+(1*dir); 
        
        while(!moved)
        {
            warH->mutexWarh.lock();

            if(warH->arr_trucks[ceil(dx)][ceil(dy)] == 0)
            {
                moved = true;

                warH->arr_trucks[x][y] = 0;
                warH->arr_trucks[x][ceil(dy)] = nr;

                y = ceil(dy);
            }
            else
            {
                //std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }

            warH->mutexWarh.unlock();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(150 + (rand() % 100)));
    }/*/

    int dir = 1;
    if(y > warH->roadY)
        dir = -1;

    //ciężarówka przejeżdża na tor do wjazdu na drogę
    warH->mutexWarh.lock();
    warH->arr_trucks[x][y] = 0;
    x = x + 1;
    warH->arr_trucks[x][y] = nr;
    warH->mutexWarh.unlock();
    std::this_thread::sleep_for(std::chrono::milliseconds(150 + (rand() % 100)));

    while (y != warH->roadY)
    {
        warH->mutexWarh.lock();
        warH->arr_trucks[x][y] = 0;
        //y = y + 1 * dir;
        y.exchange(y+1, std::memory_order::memory_order_acq_rel);
        warH->arr_trucks[x][y] = nr;
        warH->mutexWarh.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(150 + (rand() % 100)));
    }//*/
}
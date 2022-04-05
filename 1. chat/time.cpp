#include <iostream>
#include <ctime>
 
int getime ()
{
  time_t now;
  tm *nowtime;
  int hour;
  time(&now);
  nowtime=localtime(&now);
  hour=nowtime->tm_hour;
  std::cout<<"good ";
  if (hour<6)
    std::cout<<"night";
  else if(hour>=6&&hour<12)
    std::cout<<"morning";
  else if (hour>=12&&hour<18)
    std::cout<<"afternoon";
  else
    std::cout<<"evening";
return 0;
}
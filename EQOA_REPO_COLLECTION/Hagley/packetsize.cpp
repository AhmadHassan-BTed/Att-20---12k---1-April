#include "packetsize.h"

packetsize::packetsize(QObject *parent) : QObject(parent)
{

}


QByteArray packetsize::switchsize(QByteArray mainsize) //take in bytearray and return a bytearray 2 byte size for that packet
{
         int numb1 = mainsize.size() / 2 + 4; //div by 2 add 4 for crc

         //qDebug() << "Packetsize from size class" <<numb1;
         int numb2 = 0;
         int counter = 0;

         if (numb1 < 146)
             {
                 numb2 = numb1;
             }
             else

         do
         {

         numb2 =  numb1 - 128;
         counter++;
         numb1 = numb2;

         } while(numb2 > 145);

         QString result1 = result1.number(numb2,10); //remainder
         QString strcounter = strcounter.number(counter,16); //how many times
         int blah = size_map.key(numb2); //get value from key
         QString output = output.number(blah,16); //change to hex
         QByteArray sizeout1 = output.toUtf8();
         sizeout1.append("C"); //append table name
         sizeout1.append(strcounter.toUtf8()); //append counter
         //qDebug() << "sizeout from size class" << sizeout1;

         return sizeout1; //return to caller 2 byte packet size

}

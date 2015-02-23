#include <iostream>
#include <conio.h>
#include <stdio.h>
#include <QCoreApplication>
#include "dynamixel.h"
#include <QDebug>

int giID_List[2] = {1, 2};

#define MAX_IN_CHAR 128


#define P_TORQUE_ENABLE 562
#define P_GOAL_POSITION 596
#define P_PRESENT_POS	611

#define POSITION_LENGTH 4

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
    
    int port_num = 0, baud_rate = 0;

	while(true)
	{
		port_num = 0;
		printf("Input COM port number : ");
		scanf("%d", &port_num);
		fflush(stdin);

		baud_rate = 0;
		printf("Input baudrate : ");
		scanf("%d", &baud_rate);
		fflush(stdin);

		printf("\nYour input info. is\n");
		printf("COM port number : %d\n", port_num);
		printf("       Baudrate : %d\n", baud_rate);
		char _temp;
		while(true)
		{
			printf("Are you sure? (Y/N)  ");

			scanf("%c", &_temp);
			fflush(stdin);
			if(_temp == 'y' || _temp == 'Y')
				break;
			else if(_temp == 'n' || _temp == 'N')
				break;
			else
				printf("Incorrect Answer!\n");

		}
		if( (_temp == 'y') || (_temp == 'Y'))
			break;
		else
			printf("\n");
	}
    QString name("COM");
    name += QString::number(port_num);
    qDebug() << name;
    
    dxl din;
	if(din.initialize(name, baud_rate) == 0)
	{
		printf( "Failed to open USB2Dynamixel\n" );
		printf( "Press any key to terminate...\n" );
		_getch();
		return 0;
	}
	else
		printf( "Succeed to open USB2Dynamixel!\n\n" );
    
    
}

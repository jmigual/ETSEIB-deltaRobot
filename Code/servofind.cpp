#include "servofind.h"

ServoFind::ServoFind() 
{

}

ServoFind::~ServoFind()
{
    
}

void ServoFind::run()
{
    QVector<int> data(_servo.size());
    
    for (int i = 0; i < data.size(); ++i) 
        data[i] = _servo[i]->currentData().toInt();
    
    for (QCB *s : _servo) {
        s->clear();
        s->addItem("None", -1);
    }
    
    int index = 1;
    QVector<int> pos(_servo.size(), 0);
    
    dynamixel dxl(_port, _baud);
    
    for (int i = _min; i < _max; ++i) {
        dxl.ping(i);
        emit completion(((i - _min)/double(_max - _min))*100.0);
        if (dxl.get_comm_result() == COMM_RXSUCCESS) {
            
            for (int j = 0; j < _servo.size(); ++j) {
                if (data[j] == i) pos[j] = index;
                _servo[j]->addItem(QString::number(i), i);
            }
            
            ++index;
        }
    }
    
    for (int i = 0; i < _servo.size(); ++i) _servo[i]->setCurrentIndex(pos[i]);
}

void ServoFind::setData(QVector<QComboBox *> servo, QString port, 
                        int baud, int min, int max)
{
    if (this->isRunning()) return;
    _servo = servo;
    _port = port;
    _baud = baud;
    
    if (min > max) {
        int aux = min;
        min = max;
        max = aux;
    }
    
    if (min < 0) min = 0;
    if (max > MAX_ID) max = MAX_ID;
    
    
    _min = min;
    _max = max;
}



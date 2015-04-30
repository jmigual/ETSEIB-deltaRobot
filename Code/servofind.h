#ifndef SERVOFIND_H
#define SERVOFIND_H

#include "stable.h"
#include "dxl/ax12.h"

class ServoFind : public QThread
{
    Q_OBJECT
    
    typedef QComboBox QCB;
public:
    
    /// Default constructor
    ServoFind();
    
    /// Default destructor
    ~ServoFind();
    
    /// Main function
    void run();
    
    /// To set all data
    void setData(QVector<QComboBox *> servo, QString port, 
                 int baud, int min = 0, int max = MAX_ID);
    
    
signals:
    
    /// Shows the completion of the process
    void completion(int);
    
private:
    
    /// Contains the baud rate
    int _baud;
    
    /// Minimum value to find
    int _min = 0;
    
    /// Maximum value to find
    int _max = MAX_ID;
    
    /// Contains the current port
    QString _port;
    
    /// Contains the pointer to the servos QComboBoxes
    QVector<QComboBox *> _servo;    
};

#endif // SERVOFIND_H

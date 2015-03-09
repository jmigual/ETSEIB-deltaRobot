#ifndef OPTIONSWINDOW_H
#define OPTIONSWINDOW_H

#include <QDialog>
#include <QListWidgetItem>
#include <QDebug>
#include <QStackedWidget>
#include "optionsservos.h"

namespace Ui {
class OptionsWindow;
}

class OptionsWindow : public QDialog
{
    Q_OBJECT
    
public:
    explicit OptionsWindow(QWidget *parent = 0);
    ~OptionsWindow();
    
private slots:
    
    /// When the current selected item changes

    
    void on_option_currentItemChanged(QListWidgetItem *item, 
                                      QListWidgetItem *);
    
private:
    /// Containsh the GUI
    Ui::OptionsWindow *ui;
};

#endif // OPTIONSWINDOW_H

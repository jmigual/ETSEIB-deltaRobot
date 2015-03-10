#ifndef OPTIONSWINDOW_H
#define OPTIONSWINDOW_H

#include <QDialog>
#include <QListWidgetItem>
#include <QDebug>
#include <QStackedWidget>

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

private:
    /// Containsh the GUI
    Ui::OptionsWindow *ui;
};

#endif // OPTIONSWINDOW_H

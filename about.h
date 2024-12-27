#ifndef ABOUT_H
#define ABOUT_H

#include <QMainWindow>
#include <QSqlDatabase>

namespace Ui {
class about;
}

class about : public QMainWindow
{
    Q_OBJECT

public:
    explicit about(QWidget *parent = nullptr);
    ~about();

private slots:
    void resetDatabase();  // Slot to reset the database

private:
    Ui::about *ui;

    bool connectToDatabase(); // Helper function to connect to the database
};

#endif // ABOUT_H

#ifndef VIEWDATABASE_H
#define VIEWDATABASE_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QTableWidgetItem>

namespace Ui {
class viewdatabase;
}

class viewdatabase : public QMainWindow
{
    Q_OBJECT

public:
    explicit viewdatabase(QWidget *parent = nullptr);
    ~viewdatabase();

private slots:
    void applySearch();            // Slot to handle searching across all columns
    void applyFilter();            // Slot to handle filtering based on selected column
    void updateDatabase(QTableWidgetItem *item); // Slot to update the database when a table item is edited
    void deleteSelectedEntry();    // Slot to delete the selected entry
    void refreshTable();


private:
    Ui::viewdatabase *ui;

    // Helper functions
    bool connectToDatabase();
    void populateTable(const QString &filterColumn = "", const QString &searchTerm = ""); // Filter parameters
    void populateColumnSelector(); // Populate combo box with column names

    QSqlDatabase db; // Database connection object
};

#endif // VIEWDATABASE_H

#include "viewdatabase.h"
#include "ui_viewdatabase.h"

#include <QSqlQuery>
#include <QSqlRecord>
#include <QTableWidgetItem>
#include <QSqlError>
#include <QMessageBox>

viewdatabase::viewdatabase(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::viewdatabase)
{
    ui->setupUi(this);

    // Connect to the database
    if (connectToDatabase()) {
        populateColumnSelector(); // Populate the combo box with column names
        populateTable(); // Populate the table initially with all data
    } else {
        qDebug() << "Failed to connect to the database.";
    }

    // Connect the filter button to the applyFilter slot
    connect(ui->filterButton, &QPushButton::clicked, this, &viewdatabase::applyFilter);

    ui->tableWidget->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked);

    connect(ui->tableWidget, &QTableWidget::itemChanged, this, &viewdatabase::updateDatabase);

    connect(ui->deleteButton, &QPushButton::clicked, this, &viewdatabase::deleteSelectedEntry);

    connect(ui->filterComboBox, &QComboBox::currentIndexChanged, this, &viewdatabase::applySearch);

}

viewdatabase::~viewdatabase()
{
    delete ui;
}

bool viewdatabase::connectToDatabase()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("media_tracking.db");

    if (!db.open()) {
        qDebug() << "Error: Unable to connect to database:" << db.lastError().text();
        return false;
    }
    return true;
}

void viewdatabase::updateDatabase(QTableWidgetItem *item)
{
    if (!item) return;

    // Get the column name from the table header
    QString columnName = ui->tableWidget->horizontalHeaderItem(item->column())->text();

    // Get the new value and the ID of the row being edited
    QString newValue = item->text();
    int id = ui->tableWidget->item(item->row(), 0)->text().toInt(); // Assuming 'ID #' is the first column

    // Construct the SQL update query
    QString queryStr = QString("UPDATE Media SET %1 = :value WHERE id = :id").arg(columnName);

    QSqlQuery query;
    query.prepare(queryStr);
    query.bindValue(":value", newValue);
    query.bindValue(":id", id);

    if (!query.exec()) {
        qDebug() << "Failed to update database:" << query.lastError().text();
    } else {
        qDebug() << "Database updated successfully!";
    }
}

void viewdatabase::deleteSelectedEntry()
{
    // Get the currently selected row
    int currentRow = ui->tableWidget->currentRow();
    if (currentRow == -1) {
        qDebug() << "No row selected for deletion.";
        return;
    }

    // Get the ID of the selected entry (assuming 'ID #' is the first column)
    QTableWidgetItem *idItem = ui->tableWidget->item(currentRow, 0);
    if (!idItem) {
        qDebug() << "Failed to get the ID of the selected row.";
        return;
    }
    int id = idItem->text().toInt();

    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Delete Entry", "Are you sure you want to delete the selected entry?",
        QMessageBox::Yes | QMessageBox::No);

    if (reply != QMessageBox::Yes) {
        return;
    }


    // Confirm deletion from the database
    QSqlQuery query;
    query.prepare("DELETE FROM Media WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        qDebug() << "Failed to delete entry from database:" << query.lastError().text();
        return;
    }

    // Remove the row from the table
    ui->tableWidget->removeRow(currentRow);

    qDebug() << "Entry successfully deleted.";
}



void viewdatabase::populateTable(const QString &filterColumn, const QString &searchTerm)
{

    // Close the previous database connection if open
    QSqlDatabase db = QSqlDatabase::database();
    if (db.isOpen()) {
        db.close();
    }

    // Reconnect to the database
    if (!connectToDatabase()) {
        qDebug() << "Failed to reconnect to the database.";
        return;
    }


    QString queryStr = "SELECT m.id as 'ID #', m.type as 'Media Type', m.title as 'Title', m.release_date as 'Release Date', m.description as 'Description', m.genre as 'Genre', m.date_added as 'Date Added', m.parental_rating as 'Rating', m.status as 'Completed', m.location as 'Location Watched'";

    // Check the selected type and include additional columns from Movies or Episodes tables
    if (ui->filterComboBox->currentText() == "Film") {
        queryStr += ", mo.director as 'Director', mo.cast as 'Main Leads', mo.studio as 'Studio', mo.runtime as 'Runtime'";  // Additional columns for Movies
        queryStr += " FROM Media m LEFT JOIN Movies mo ON m.id = mo.media_id"; // Join with Movies table
    }
    else if (ui->filterComboBox->currentText() == "TV Show") {
        queryStr += ", e.episode_title as 'Ep Title', e.season_number as 'Season #', e.episode_number as 'Episode #', e.duration as 'Duration', e.rating as 'IMDB Rating', e.episode_description as 'Ep Description'"; // Additional columns for Episodes
        queryStr += " FROM Media m LEFT JOIN Episodes e ON m.id = e.media_id"; // Join with Episodes table
    } else {
        queryStr += " FROM Media m"; // Only Media table if neither Movies nor Episodes
    }

    // If a filter is applied, add a WHERE clause
    if (!filterColumn.isEmpty() && !searchTerm.isEmpty()) {
        queryStr += QString(" WHERE %1 LIKE '%%2%'").arg(filterColumn, searchTerm);
    }

    QSqlQuery query(queryStr);
    if (!query.exec()) {
        qDebug() << "Failed to execute query:" << query.lastError().text();
        return;
    }

    // Reset the table
    ui->tableWidget->setRowCount(0);

    // Set column count and headers dynamically
    int columnCount = query.record().count();
    ui->tableWidget->setColumnCount(columnCount);

    QStringList headers;
    for (int i = 0; i < columnCount; ++i) {
        headers << query.record().fieldName(i); // Get column names
    }
    ui->tableWidget->setHorizontalHeaderLabels(headers);

    // Populate rows with query results
    int row = 0;
    while (query.next()) {
        ui->tableWidget->insertRow(row); // Add a new row
        for (int col = 0; col < columnCount; ++col) {
            QTableWidgetItem *item = new QTableWidgetItem(query.value(col).toString());
            ui->tableWidget->setItem(row, col, item);
        }
        row++;
    }
}

void viewdatabase::applySearch()
{
    // Get the selected column from the combo box
    QString selectedColumn = ui->filterComboBox->currentText();

    // Get the search term from the search box
    //QString searchTerm = ui->searchBox->text();

    // Populate the table with the filtered data
    populateTable("type", selectedColumn);
}

void viewdatabase::applyFilter()
{
    // Get the selected column from the combo box
    QString selectedColumn = ui->columnSelector->currentText();

    // Get the search term from the search box
    QString searchTerm = ui->searchBox->text();

    // Populate the table with the filtered data
    populateTable(selectedColumn, searchTerm);
}

void viewdatabase::populateColumnSelector()
{
    QSqlQuery query("PRAGMA table_info(Media)"); // Get column names from the database
    while (query.next()) {
        QString columnName = query.value(1).toString(); // Column name is in the second field
        ui->columnSelector->addItem(columnName);
    }
}

void viewdatabase::refreshTable()
{
    // Call populateTable with empty filter to reload all data
    populateTable("type", "All");
}

#include "mainwindow.h"
#include "QtWidgets/qstyleditemdelegate.h"
#include "ui_mainwindow.h"
#include <QHBoxLayout>
#include <QHeaderView>
#include <QIntValidator>
#include <QLineEdit>
#include <QMessageBox>
#include <QFileDialog>
#include <QCloseEvent>
// Определение класса для делегата со встроенным валидатором
class YearDelegate : public QStyledItemDelegate
{
public:
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        Q_UNUSED(option);
        Q_UNUSED(index);

        QLineEdit *editor = new QLineEdit(parent);
        editor->setValidator(new QIntValidator(0, 2024, editor)); // Ограничение на цифры от 0 до 2024
        return editor;
    }
};





MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->action, &QAction::triggered, this, &MainWindow::on_action_triggered);
    connect(ui->action_2, &QAction::triggered, this, &MainWindow::on_action_2_triggered);
    connect(ui->action_3, &QAction::triggered, this, &MainWindow::on_action_3_triggered);
    model = new QStandardItemModel(0, 4, this);
    setupTableView();
    setupButtons();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupTableView()
{
    tableView = new QTableView;
    tableView->setModel(model);

    model->setHorizontalHeaderItem(0, new QStandardItem(QString("Автор")));
    model->setHorizontalHeaderItem(1, new QStandardItem(QString("Название")));
    model->setHorizontalHeaderItem(2, new QStandardItem(QString("Год издания")));
    model->setHorizontalHeaderItem(3, new QStandardItem(QString("Жанр")));

    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // Устанавливаем делегат для столбца "Year"
    tableView->setItemDelegateForColumn(2, new YearDelegate);

    QWidget *buttonsWidget = new QWidget;
    QHBoxLayout *buttonsLayout = new QHBoxLayout;

    // Используем кнопки из ui
    buttonsLayout->addWidget(ui->add);
    buttonsLayout->addWidget(ui->delet);

    QLineEdit *searchLineEdit = new QLineEdit;
    QPushButton *editButton = new QPushButton("Искать");
    buttonsLayout->addWidget(searchLineEdit);
    buttonsLayout->addWidget(editButton);

    connect(editButton, &QPushButton::clicked, [=](){
        QString searchText = searchLineEdit->text();
        for(int row = 0; row < model->rowCount(); row++){
            for(int col = 0; col < model->columnCount(); col++){
                QModelIndex index = model->index(row, col);
                QString cellText = index.data().toString();
                if(cellText.contains(searchText, Qt::CaseInsensitive)){
                    tableView->selectRow(row);
                    return;
                }
            }
        }
    });

    buttonsWidget->setLayout(buttonsLayout);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(tableView);
    layout->addWidget(buttonsWidget);

    QWidget *centralWidget = new QWidget;
    centralWidget->setLayout(layout);

    setCentralWidget(centralWidget);
}

void MainWindow::setupButtons()
{
    connect(ui->add, &QPushButton::clicked, this, &MainWindow::on_add_clicked);
    connect(ui->delet, &QPushButton::clicked, this, &MainWindow::on_delet_clicked);
}

void MainWindow::on_add_clicked()
{
    if(!rowAdded) {
        QString author = "Введите автора";
        QString title = "Введите название";
        QString year = "Введите год издания";
        QString genre = "Введите жанр";

        QList<QStandardItem *> newRow;
        newRow << new QStandardItem(author) << new QStandardItem(title) << new QStandardItem(year) << new QStandardItem(genre);

        model->appendRow(newRow);

        // Устанавливаем флаг, чтобы добавление следующей строки было заблокировано
        rowAdded = true;
    } else {
        // Сбрасываем флаг, чтобы можно было добавить следующую строку
        rowAdded = false;
    }
}




void MainWindow::on_edit_clicked()
{
    // Реализация редактирования записи
}

void MainWindow::on_delet_clicked()
{
    // Получаем список выбранных индексов
    QModelIndexList selectedIndexes = tableView->selectionModel()->selectedRows();

    // Если список индексов не пустой
    if (!selectedIndexes.isEmpty()) {
        // Спрашиваем у пользователя подтверждение удаления
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Подтверждение", "Вы уверены, что хотите удалить выбранные записи?", QMessageBox::Yes|QMessageBox::No);

        if (reply == QMessageBox::Yes) {
            // Итерация по списку индексов в обратном порядке и удаление строк
            for (int i = selectedIndexes.count() - 1; i >= 0; i--) {
                model->removeRow(selectedIndexes.at(i).row());
            }
        }
    }
}




void MainWindow::exportData()
{
    QFileDialog dialog(this, "Выберите файл для экспорта", "", "CSV (*.csv)");
    dialog.setAcceptMode(QFileDialog::AcceptSave);

    if (!dialog.isModal() && dialog.exec() == QDialog::Accepted) {
        QString fileName = dialog.selectedFiles().at(0);

        if (!fileName.isEmpty()) {
            QFile file(fileName);
            if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QTextStream out(&file);

                // Добавляем заголовки столбцов
                for (int column = 0; column < model->columnCount(); column++) {
                    out << model->headerData(column, Qt::Horizontal).toString();
                    if (column < model->columnCount() - 1) { // Добавляем запятую только если не последний столбец
                        out << ",";
                    }
                }
                out << "\n";

                // Добавляем данные
                for (int row = 0; row < model->rowCount(); row++) {
                    for (int column = 0; column < model->columnCount(); column++) {
                        out << model->item(row, column)->text();
                        if (column < model->columnCount() - 1) { // Добавляем запятую только если не последний столбец
                            out << ",";
                        }
                    }
                    out << "\n";
                }

                file.close();
            }
        }
    }
}





void MainWindow::importData()
{
    QFileDialog dialog(this, "Выберите файл для импорта", "", "CSV (*.csv)");
    dialog.setAcceptMode(QFileDialog::AcceptOpen);

    if (!dialog.isModal() && dialog.exec() == QDialog::Accepted) {
        QString fileName = dialog.selectedFiles().at(0);

        if (!fileName.isEmpty()) {
            QFile file(fileName);
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QTextStream in(&file);
                model->removeRows(0, model->rowCount());

                while (!in.atEnd()) {
                    QString line = in.readLine();
                    QStringList fields = line.split(",", Qt::SkipEmptyParts); // Обновленное использование Qt::SkipEmptyParts

                    QList<QStandardItem*> items;
                    for (const QString& field : fields) {
                        QString trimmedField = field.trimmed(); // Обрезаем пробелы по краям
                        items.append(new QStandardItem(trimmedField));
                    }

                    model->appendRow(items);
                }

                file.close();
            }
        }
    }
}






void MainWindow::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Подтверждение", "Вы уверены, что хотите закрыть приложение?", QMessageBox::Yes|QMessageBox::No);

    if (reply == QMessageBox::No) {
        event->ignore();  // Отклоняем закрытие основного окна
    } else {
        event->accept();  // Принимаем закрытие основного окна
    }
}

void MainWindow::on_action_triggered()
{
    exportData();
}

void MainWindow::on_action_2_triggered()
{
    importData();
}

void MainWindow::on_action_3_triggered()
{
    close();
}

#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QTableView>
#include <QStandardItemModel>
#include <QPushButton>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void exportData();
    void importData();
protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void on_add_clicked();
    void on_delet_clicked();
    void on_edit_clicked();


    void on_action_triggered();

    void on_action_2_triggered();

    void on_action_3_triggered();

private:
    Ui::MainWindow *ui;
    QTableView *tableView;
    QStandardItemModel *model;
    void setupTableView();
    void setupButtons();
    bool rowAdded = false;
    QString m_lastSelectedFile;
    bool exportedOnce = false;
    QString m_lastExportPath;
};

#endif // MAINWINDOW_H

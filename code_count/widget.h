#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include<QStandardItemModel>
#include<QDebug>
#include<QFileDialog>

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private:

    void _initTableModel();

    void _readFileToTable(QString filePath);

    void _foreachFile(QString path,QStringList& fileterate);

private:

    QStandardItemModel* model;


private slots:

    void on_PB_OpenFile_clicked();

    void on_PB_OpenDir_clicked();

    void on_PB_ClearResult_clicked();

private:
    Ui::Widget *ui;
};
#endif // WIDGET_H

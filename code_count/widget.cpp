#include "widget.h"
#include "./ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    _initTableModel();

}

Widget::~Widget()
{
    delete ui;
}

void Widget::_initTableModel()
{
    this->model = new QStandardItemModel(ui->tableView);
    QStringList table_headers;
    table_headers<<"文件名"<<"类型"<<"大小"<<"总行数"<<"代码行数"<<"注释行数"<<"空白行数"<<"路径";
    model->setHorizontalHeaderLabels(table_headers);

    model->setColumnCount(8);

    ui->tableView->setModel(model);
}

void Widget::_readFileToTable(QString filePath)
{
    QFile file(filePath);
    /* Only Read */
    if(file.open(QIODevice::ReadOnly))
    {
        /* table elements */
        QList<QStandardItem*> elements;
        QFileInfo fileInfo(filePath);
        /* file name */
        elements.append(new QStandardItem(fileInfo.fileName()));
        /* file suffix */
        elements.append(new QStandardItem(fileInfo.suffix()));

        /* count file */
        QString line = file.readLine();
        QTextStream in(&file);
        /* file length */
        int count_length = 0;
        /* file rows */
        int count_rows = 0;
        /* file code rows */
        int count_code = 0;
        /* file note rows */
        int count_note = 0;
        /* file null rows */
        int count_null = 0;

        bool isNote = false;
        while(!line.isNull())
        {
            count_rows++;
            count_length+=line.length();
            /* note (*) */
            if(!isNote)
            {
                if(line.indexOf("/*") != -1)
                {
                    line.replace(" ","");
                    if(line.indexOf("/*")==0)
                    {
                        count_note++;
                        if(line.indexOf("*/") == -1)
                        {
                            isNote = true;
                        }
                    }

                }
                else if(line.indexOf("//") != -1)
                {
                    //count_note++;
                    line.replace(" ","");
                    QStringList list = line.split("//");
                    if(list[0] == "")
                    {
                        count_note++;
                    }
                    else
                    {
                        count_code++;
                    }
                }
                else
                {
                    if(line == "")
                    {
                        count_null++;
                    }
                    else
                    {
                        count_code++;
                    }
                }
            }
            else
            {
                count_note++;
                if(line.indexOf("*/")!=-1)
                {
                    isNote = false;
                }
            }

            line = in.readLine();
        }
        /* add result */
        elements.append(new QStandardItem(QString::number(count_length)));
        elements.append(new QStandardItem(QString::number(count_rows)));
        elements.append(new QStandardItem(QString::number(count_code)));
        elements.append(new QStandardItem(QString::number(count_note)));
        elements.append(new QStandardItem(QString::number(count_null)));
        elements.append(new QStandardItem(fileInfo.absoluteFilePath()));
        model->appendRow(elements);

        /* set LineEdit value */

        QString _LE_filePath = ui->LE_FilePath->text() + ";" + fileInfo.absoluteFilePath();
        ui->LE_FilePath->setText(_LE_filePath);

        int _LE_FileNum = ui->LE_FileNum->text().toInt() + 1;
        ui->LE_FileNum->setText(QString::number(_LE_FileNum));

        int _LE_RowNum = ui->LE_RowNum->text().toInt() + count_rows;
        ui->LE_RowNum->setText(QString::number(_LE_RowNum));

        int _LE_ByteNum = ui->LE_ByteNum->text().toInt() + count_length;
        ui->LE_ByteNum->setText(QString::number(_LE_ByteNum));

        int _LE_CodeRows = ui->LE_CodeRows->text().toInt() + count_code;
        ui->LE_CodeRows->setText(QString::number(_LE_CodeRows));

        int _LE_NoteRows = ui->LE_NoteRows->text().toInt() + count_note;
        ui->LE_NoteRows->setText(QString::number(_LE_NoteRows));

        int _LE_NullRows = ui->LE_NullRows->text().toInt() + count_null;
        ui->LE_NullRows->setText(QString::number(_LE_NullRows));

        ui->PGB_CodeRows->setValue((_LE_CodeRows*100)/_LE_RowNum);
        ui->PGB_NoteRows->setValue((_LE_NoteRows*100)/_LE_RowNum);
        ui->PGB_NullRows->setValue((_LE_NullRows*100)/_LE_RowNum);

    }
    file.close();
}

void Widget::_foreachFile(QString path, QStringList& fileterate)
{
    QDir dir(path);
    if(!dir.exists())
    {
        return;
    }
    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::Dirs);
    QFileInfoList list = dir.entryInfoList();
    for(int i = 0;i < list.count();i++)
    {
        if(list.at(i).isDir())
        {
            _foreachFile(list.at(i).absoluteFilePath(),fileterate);
        }
        else
        {
            for(int j = 0;j < fileterate.count();j++)
            {
                if(list.at(i).suffix() == fileterate.at(j))
                {
                    _readFileToTable(list.at(i).absoluteFilePath());
                    break;
                }
            }
        }
    }
}



void Widget::on_PB_OpenFile_clicked()
{
    QString fileterate = ui->LE_Filterate->text();
    QString fiter = "Code files(" + fileterate + ");;All files(*.*)";
    QString filePath = QFileDialog::getOpenFileName(
        this,
        tr("open a file."),
        QCoreApplication::applicationDirPath(),
        tr(fiter.toStdString().c_str()));
    if(filePath.isEmpty())
    {
        return;
    }
    else
    {
        _readFileToTable(filePath);
    }

}


void Widget::on_PB_OpenDir_clicked()
{

    QString path = QFileDialog::getExistingDirectory(
        this,
        tr("open a file."),
        QCoreApplication::applicationDirPath()
        );

    ui->LE_FileDir->setText(path);
    if(path == "")
    {
        return;
    }
    QStringList fileterate = ui->LE_Filterate->text().replace("*.","").split(" ");
    _foreachFile(path,fileterate);

}


void Widget::on_PB_ClearResult_clicked()
{
    model->clear();
    ui->LE_ByteNum->setText("0");
    ui->LE_CodeRows->setText("0");
    ui->LE_NoteRows->setText("0");
    ui->LE_NullRows->setText("0");
    ui->LE_FileDir->setText("");
    ui->LE_FileNum->setText("0");
    ui->LE_FilePath->setText("");
    ui->LE_RowNum->setText("0");
    ui->PGB_CodeRows->setValue(0);
    ui->PGB_NoteRows->setValue(0);
    ui->PGB_NullRows->setValue(0);
}


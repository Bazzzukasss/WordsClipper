#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QClipboard>
#include <QFileDialog>
#include <QDesktopWidget>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <QMessageBox>
#include <QScopedPointer>
#include <QDebug>
#include <QTextBlock>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    int width(200),border(6);
    ui->setupUi(this);
    setWindowFlags(Qt::WindowStaysOnTopHint|Qt::FramelessWindowHint);//|Qt::MSWindowsFixedSizeDialogHint);
    setAttribute(Qt::WA_TranslucentBackground);

    setGeometry(QApplication::desktop()->width()-width-border,border,width,QApplication::desktop()->availableGeometry(0).height()-border*2);
    init();
    clipper =new TClipper(wordsFileName);


    timer=new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(slotTimer()));
    QApplication::clipboard()->clear();
    timer->start(200);

    tray=new QSystemTrayIcon(this);
    tray->setIcon(QPixmap(":/images/images/file.png"));
    tray->show();
    tray->setToolTip("Words Clipper:\n"+wordsFileName);
    ui->label_Title->setToolTip(wordsFileName);

    connect(tray,SIGNAL( activated(QSystemTrayIcon::ActivationReason)),this,SLOT(slotTrayActivated(QSystemTrayIcon::ActivationReason)));

    trayMenu=new QMenu(this);

    QAction* clearWordsAction=new QAction("Clear",this);
    clearWordsAction->setToolTip("Clear current words list");
    connect(clearWordsAction,SIGNAL(triggered()),this,SLOT(slotClear()));

    QAction* updateFileAction=new QAction("Update file",this);
    updateFileAction->setToolTip("Update file with current words list");
    connect(updateFileAction,SIGNAL(triggered()),this,SLOT(slotUpdateFile()));
/*
    QAction* saveFileAction=new QAction("Save file",this);
    saveFileAction->setToolTip("Save current words list to file");
    connect(saveFileAction,SIGNAL(triggered()),this,SLOT(onSaveFile()));
*/
    QAction* loadFileAction=new QAction("Load file",this);
    loadFileAction->setToolTip("Load words list from file");
    connect(loadFileAction,SIGNAL(triggered()),this,SLOT(slotLoadFile()));

    QAction* selectFileAction=new QAction("Select file",this);
    selectFileAction->setToolTip("Select file");
    connect(selectFileAction,SIGNAL(triggered()),this,SLOT(slotSelectFile()));

    QAction* closeAction=new QAction("Close",this);
    connect(closeAction,SIGNAL(triggered()),this,SLOT(slotClose()));



    trayMenu->addAction(selectFileAction);
    trayMenu->addAction(loadFileAction);
    trayMenu->addAction(updateFileAction);
    //trayMenu->addAction(saveFileAction);
    trayMenu->addAction(clearWordsAction);

    trayMenu->addSeparator();
    trayMenu->addAction(closeAction);
    tray->setContextMenu(trayMenu);

    connect(ui->pushButton_Active,SIGNAL(clicked()),this,SLOT(slotActivity()));
    connect(ui->pushButton_Load,SIGNAL(clicked()),this,SLOT(slotLoadFile()));
    connect(ui->pushButton_Save,SIGNAL(clicked()),this,SLOT(slotSaveFile()));
    connect(ui->pushButton_Select,SIGNAL(clicked()),this,SLOT(slotSelectFile()));
    connect(ui->pushButton_Update,SIGNAL(clicked()),this,SLOT(slotUpdateFile()));
    connect(ui->pushButton_Clear,SIGNAL(clicked()),this,SLOT(slotClear()));
    connect(ui->pushButton_Close,SIGNAL(clicked()),this,SLOT(slotVisibility()));
    connect(ui->pushButton_Delete,SIGNAL(clicked()),this,SLOT(slotDeleteWord()));
    connect(ui->pushButton_Apply,SIGNAL(clicked()),this,SLOT(slotApply()));
    connect(ui->horizontalSlider_fontSize,SIGNAL(valueChanged(int)),this,SLOT(slotFontSize(int)));
    connect(ui->listWidget_Words,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(slotSelectWord()));
}

MainWindow::~MainWindow()
{
    tray->hide();
    delete ui;
    delete clipper;
}
void MainWindow::init()
{
    QFile f(QString(QDir::currentPath()+"\\config.ini"));
    if(f.exists())
    {
        loadCFG();
        int cfgN=cfg.size();
        if(cfgN>0)
            wordsFileName=QString().fromStdString(cfg[0]);
        if(cfgN>1)
        {
            fontSize=QString().fromStdString(cfg[1]).toInt();
            ui->horizontalSlider_fontSize->setValue(fontSize);
            slotFontSize(fontSize);
        }
    }
    else
        wordsFileName=QDir::currentPath()+"\\words.txt";
}

void MainWindow::close()
{
    cfg.clear();
    cfg.push_back(wordsFileName.toStdString());
    cfg.push_back(QString().number(fontSize).toStdString());
    saveCFG();
}
void MainWindow::loadCFG()
{
    ifstream is(QString(QDir::currentPath()+"\\config.ini").toStdString());
    copy(
                istream_iterator<string>(is),
                istream_iterator<string>(),
                back_inserter(cfg)
                );
}

void MainWindow::saveCFG()
{
    ofstream os(QString(QDir::currentPath()+"\\config.ini").toStdString());
    copy(
                cfg.begin(),
                cfg.end(),
                ostream_iterator<string>(os,"\n")
                );
}
void MainWindow::slotTrayActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
        case QSystemTrayIcon::Trigger:
            slotVisibility();
            break;
        case QSystemTrayIcon::Context:
            break;
        case QSystemTrayIcon::DoubleClick:
            break;
        case QSystemTrayIcon::MiddleClick:
            slotActivity();
            break;
        default:
            break;
    }
}

void MainWindow::slotTimer()
{
    QClipboard* pClip=QApplication::clipboard();
    QString str = pClip->text();
    if(!str.isNull()&&isActive)
    {
        ui->textEdit_Word->setText(getFormatedString(str));
        clipper->addWord(str);
        showWords();
        pClip->clear();
    }
}
void MainWindow::slotActivity()
{
    QApplication::clipboard()->clear();
    isActive=!isActive;
    if(isActive)
        tray->setIcon(QPixmap(":/images/images/file.png"));
    else
        tray->setIcon(QPixmap(":/images/images/file_.png"));
}

void MainWindow::slotSelectWord()
{
    currentWord=ui->listWidget_Words->currentItem()->toolTip();
    ui->textEdit_Word->setText(currentWord);
}

void MainWindow::slotApply()
{
    QString str;
    if(currentWord!="")
        clipper->delWord(currentWord);
    str=ui->textEdit_Word->toPlainText();
    clipper->addWord(str);
    currentWord="";
    ui->textEdit_Word->setText("");
    showWords();
}
void MainWindow::slotVisibility()
{
    isVisible=!isVisible;
    if(isVisible)
        show();
    else
        hide();
}

void MainWindow::setWordsFileName(const QString& filename)
{
    wordsFileName=filename;
    tray->setToolTip("Words Clipper:\n"+filename);
}
void MainWindow::slotDeleteWord()
{
    QListWidgetItem* it=ui->listWidget_Words->currentItem();
    if(it)
    {
        QString str(it->toolTip());
        clipper->delWord(str);
        showWords();
    }
}
void MainWindow::slotUpdateFile()
{
    clipper->updateFile();
    showWords();
}
void MainWindow::slotClear()
{
    if(confirm("Clear all words?"))
        clipper->clearWords();
    showWords();
}
void MainWindow::slotClose()
{
    clipper->updateFile();
    close();
    QApplication::quit();
}
void MainWindow::slotLoadFile()
{
    clipper->loadFile();
    showWords();
}
void MainWindow::slotSaveFile()
{
    if(confirm("Save words to file?"))
        clipper->saveFile();
}
void MainWindow::slotSelectFile()
{
    QString filename = QFileDialog::getSaveFileName( this, tr("Select words file"), clipper->getFileName(), tr("Words files (*.txt);;"), 0, QFileDialog::DontUseNativeDialog );
    if(!filename.isNull())
    {
        setWordsFileName(filename);
        clipper->setFileName(filename);
        tray->setToolTip("Words Clipper:\n"+QString(clipper->getFileName().data()));
        ui->label_Title->setToolTip(QString(clipper->getFileName().data()));
    }
}

void MainWindow::slotFontSize(int _size)
{
    QFont font(ui->listWidget_Words->font());
    font.setPointSize(_size);
    fontSize=_size;
    ui->listWidget_Words->setFont(font);
    showWords();
}

bool MainWindow::confirm(const QString &message)
{
    QMessageBox* dialog=new QMessageBox(QMessageBox::Question,qApp->applicationName(),message,QMessageBox::Yes|QMessageBox::No);
    QScopedPointer<QMessageBox>P(dialog);
    int res=dialog->exec();
    return (res==QMessageBox::Yes);
}

void MainWindow::showWords()
{
    if(clipper!=nullptr)
    {
        QFont font(ui->listWidget_Words->font());
        font.setPointSize(fontSize);
        ui->listWidget_Words->clear();
        //ui->listWidget_Words->addItems(clipper->getWordsList());
        for(auto w:clipper->getWordsList())
        {
            QListWidgetItem* pItem = new QListWidgetItem(ui->listWidget_Words);
            QLabel* pLabel= new QLabel(getFormatedString(w),this);
            pItem->setToolTip(w);
            pItem->setFont(font);
            pLabel->setFont(font);
            ui->listWidget_Words->addItem(pItem);
            ui->listWidget_Words->setItemWidget(pItem,pLabel);
        }
    }

}

const QString MainWindow::getFormatedString(const QString &str)
{
    QString res;
    res="<b style =\"color:rgb(0,85,255)\" >"+str.section(QRegExp("[^a-zA-Z]"),0,0)+"</b> - <span style =\"color:grey\" >"+str.section(QRegExp("[^a-zA-Z]"),2,-1);
    return res;
}


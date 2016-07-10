#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QSystemTrayIcon>
#include <QMenu>
#include <vector>
#include "tclipper.h"
#include <QListWidgetItem>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void slotTrayActivated(QSystemTrayIcon::ActivationReason reason);
    void slotTimer();
    void slotClose();
    void slotUpdateFile();
    void slotSaveFile();
    void slotLoadFile();
    void slotDeleteWord();
    void slotClear();
    void slotSelectFile();
    void slotFontSize(int _size);
    void slotVisibility();
    void slotActivity();
    void slotSelectWord();
    void slotApply();

private:
    QString currentWord;
    bool confirm(const QString& message);
    void init();
    void close();
    void loadCFG();
    void saveCFG();
    void setWordsFileName(const QString& filename);
    bool    isVisible{true};
    bool    isActive{true};
    Ui::MainWindow *ui;
    QTimer*         timer;
    TClipper*       clipper{nullptr};
    QSystemTrayIcon* tray;
    QMenu*           trayMenu;
    vector<string>   cfg;
    QString          wordsFileName;
    int              fontSize;
    void showWords();
    const QString getFormatedString(const QString& str);
};

#endif // MAINWINDOW_H

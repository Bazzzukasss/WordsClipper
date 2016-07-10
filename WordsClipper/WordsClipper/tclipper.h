#ifndef TCLIPPER_H
#define TCLIPPER_H
#include <set>
#include <string>
#include <QString>
#include <QStringList>

using namespace std;

class TClipper
{
    public:
        TClipper(const QString& fname);
        void delWord(QString& str);
        void addWord(QString& str);
        const QStringList& getWordsList();
        void setFileName(const QString& fname);
        const QString& getFileName(){return fileName;}
        void updateFile();
        void loadFile();
        void saveFile();
        void clearWords();
    private:
        QString fileName;
        set<string> wordsSet;
        QStringList wordsList;
        void createWordsList();
        bool load();
        bool save();
};

#endif // TCLIPPER_H

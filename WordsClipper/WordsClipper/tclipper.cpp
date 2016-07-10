#include "tclipper.h"
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <QString>
#include <vector>
using namespace std;
TClipper::TClipper(const QString& fname):fileName(fname)
{
    /*
    if(fname!=NULL)
        fileName=fname;
        */
}

void TClipper::delWord(QString &str)
{
    wordsSet.erase(str.toStdString());
    createWordsList();
}

void TClipper::addWord(QString &str)
{
    if(str.size()>1)
    {
        string word=str.toLower().toStdString();
        wordsSet.insert(word);
        createWordsList();
    }
}

const QStringList &TClipper::getWordsList()
{
    return wordsList;
}

void TClipper::setFileName(const QString &fname)
{
    if(!fname.isNull())
        fileName=fname;
}

void TClipper::createWordsList()
{
    wordsList.clear();
    for(auto word:wordsSet)
        wordsList.append(QString().fromStdString(word));
}
void TClipper::updateFile()
{
    load();
    save();
    createWordsList();
}
void TClipper::loadFile()
{
    wordsSet.clear();
    load();
    createWordsList();
}
void TClipper::saveFile()
{
    save();
}
void TClipper::clearWords()
{
    wordsSet.clear();
    createWordsList();
}
bool TClipper::load()
{
    if(!fileName.isNull())
    {
        string str;
        ifstream is(fileName.toStdString());
        if(is.is_open())
            while (!is.eof())
            {
              getline (is, str);
              if(str!="")
                wordsSet.insert(str);
            }
        /*
        copy(
                istream_iterator<string>(is),
                istream_iterator<string>(),
                inserter(wordsSet,wordsSet.end())
            );
            */
        return true;
    }
    else
        return false;
}

bool TClipper::save()
{
    if(!fileName.isNull())
    {
        ofstream os(fileName.toStdString());
        copy(
                wordsSet.begin(),
                wordsSet.end(),
                ostream_iterator<string>(os,"\n")
            );
        return true;
    }
    else
        return false;
}


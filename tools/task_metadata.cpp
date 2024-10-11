//
// Created by bklytics on 10/11/24.
//

#include "task_metadata.h"
#include <QFile>
#include <QTextStream>

void task_metadata::writeTo(QString path)
{
    QFile file(path);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&file);
        out << "Image Path: " << imagePath << "\n";
        out << "Case Number: " << caseNumber << "\n";
        out << "Description: " << description << "\n";
        out << "Examiner Name: " << examinerName << "\n";
        out << "Evidence Name: " << evidenceName << "\n";
        out << "Note: " << note << "\n";
        out << "File Format: " << fileFormat << "\n";
        out << "Average Speed: " << averageSpeed << "\n";
        out << "Time: " << time << "\n";
        file.close();
    }
}
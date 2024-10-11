//
// Created by bklytics on 10/10/24.
//

#ifndef TASK_H
#define TASK_H

#include <QString>

enum HashType {
    MD5,
    SHA1,
    SHA256
};

enum T_CLI {
    DC3DD,
    AFFIMAGER,
};

struct Task {
    QString imageName;
    QString source;
    QString destination;
    HashType hash;
    QString logPath;
    QString mountedPath;
    T_CLI command;
};


#endif //TASK_H

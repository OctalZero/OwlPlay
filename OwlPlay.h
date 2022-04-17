#pragma once

#include <QtWidgets/QWidget>
#include "ui_OwlPlay.h"

class OwlPlay : public QWidget
{
    Q_OBJECT

public:
    OwlPlay(QWidget *parent = Q_NULLPTR);

private:
    Ui::OwlPlayClass ui;
};

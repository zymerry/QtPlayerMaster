#ifndef QTPLAYERMASTER_H
#define QTPLAYERMASTER_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class QtPlayerMaster; }
QT_END_NAMESPACE

class QtPlayerMaster : public QWidget
{
    Q_OBJECT

public:
    QtPlayerMaster(QWidget *parent = nullptr);
    ~QtPlayerMaster();

private:
    Ui::QtPlayerMaster *ui;
};
#endif // QTPLAYERMASTER_H

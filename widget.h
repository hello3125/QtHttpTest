#ifndef WIDGET_H
#define WIDGET_H

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void onfinished();

    void onfinished_1();

    void on_getBut_clicked();

    void on_postBut_clicked();

private:
    Ui::Widget *ui;
    QNetworkAccessManager *manager;
};
#endif // WIDGET_H

#include "widget.h"
#include "ui_widget.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QSslSocket>
#include "qdebug.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    qDebug()<< QSslSocket::sslLibraryBuildVersionString();

    manager = new QNetworkAccessManager(this);
    //connect(manager,&QNetworkAccessManager::finished,this,&Widget::onfinished);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_getBut_clicked()
{
    QNetworkRequest req(QUrl(QString("https://zj.v.api.aa1.cn/api/xz/?code=%1").arg(ui->code->text())));
    //qDebug()<< QString("https://zj.v.api.aa1.cn/api/xz/?code=%1").arg(ui->code->text()) << endl;
    QSslConfiguration config = QSslConfiguration::defaultConfiguration();
    config.setProtocol(QSsl::AnyProtocol);//表示可以使用任何支持的SSL/TLS协议版本,用来解决
    config.setPeerVerifyMode(QSslSocket::VerifyNone);
    req.setSslConfiguration(config);

    QNetworkReply* reply = manager->get(req);
    connect(reply,&QNetworkReply::finished,this,&Widget::onfinished);
    qDebug() << "test1" << endl;
}

void Widget::onfinished()
{
    qDebug() << "test2" << endl;
    QNetworkReply *reply = (QNetworkReply*)(sender());

    if(reply->error() != QNetworkReply::NoError) {
            qDebug() << "Network Error: " << reply->errorString();
            return;
        }

    QByteArray data = reply->readAll();
    reply->deleteLater();//读取完删除
    if(data.isEmpty()){
        qDebug() << "test3" << endl;
        return;
    }

    qDebug() << data << endl;
    QJsonDocument jd = QJsonDocument::fromJson(data);
    if(jd.isObject()){
        QJsonObject jo = jd.object();
        QJsonObject joData = jo.value("data").toObject();
        ui->NameLabel->setText(joData.value("Name").toString());
        ui->ProvinceLabel->setText(joData.value("Province").toString());
        ui->CityLabel->setText(joData.value("City").toString());
        ui->DistrictLabel->setText(joData.value("District").toString());
        ui->TowLabel->setText(joData.value("Tow").toString());
        ui->VillagLabel->setText(joData.value("Villag").toString());
        ui->LevelTypeLabel->setText(joData.value("LevelType").toString());
    }

}

void Widget::onfinished_1()
{
    qDebug() << "test2" << endl;
    QNetworkReply *reply = (QNetworkReply*)(sender());

    if(reply->error() != QNetworkReply::NoError) {
            qDebug() << "Network Error: " << reply->errorString();
            return;
        }

    QByteArray data = reply->readAll();
    reply->deleteLater();//读取完删除
    if(data.isEmpty()){
        qDebug() << "test3" << endl;
        return;
    }

    qDebug() << data << endl;
    QJsonDocument jd = QJsonDocument::fromJson(data);
    if(jd.isObject()){
        QJsonObject jo = jd.object();
        QJsonObject joData = jo.value("data").toObject();

        // 检查"data"下是否存在"lists"数组
        if (joData.contains("lists") && joData["lists"].isArray()) {
            QJsonArray listsArray = joData["lists"].toArray();

            // 遍历列表中的每个元素
            foreach (const QJsonValue &value, listsArray) {
                if (value.isObject()) {
                    QJsonObject item = value.toObject();

                    QString food1 = item["food1"].toString();
                    QString food2 = item["food2"].toString();
                    QString consequence = item["consequence"].toString();

                    qDebug() << "Food1:" << food1 << ", Food2:" << food2 << ", Consequence:" << consequence;
                    ui->textBrowser->append(food1+"+"+food2+":"+consequence);
                }
            }
        }
    }
}

void Widget::on_postBut_clicked()
{
    QNetworkRequest req(QUrl(QString("https://api.istero.com/resource/food/combinations")));
    QSslConfiguration config = QSslConfiguration::defaultConfiguration();
    config.setProtocol(QSsl::AnyProtocol);//表示可以使用任何支持的SSL/TLS协议版本,用来解决
    config.setPeerVerifyMode(QSslSocket::VerifyNone);
    req.setSslConfiguration(config);
    req.setHeader(QNetworkRequest::ContentTypeHeader,QVariant("application/json;charset=UTF-8"));

    QJsonObject json;
    QJsonDocument jsonDoc;
    json["token"]=QString("1b840a121eb70dc43256c418222f6351");
    json["food"]=ui->foodLineEdit->text();
    jsonDoc.setObject(json);
    QByteArray dataArray = jsonDoc.toJson(QJsonDocument::Compact);
    qDebug() << "post data = " << dataArray << endl;
    QNetworkReply* reply = manager->post(req,dataArray);
    connect(reply,&QNetworkReply::finished,this,&Widget::onfinished_1);
    qDebug() << "test1" << endl;
}

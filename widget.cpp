#include "widget.h"
#include "ui_widget.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QSslSocket>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include "qdebug.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    setWindowTitle(QString("qt实现Http"));
    qDebug()<< QSslSocket::sslLibraryBuildVersionString();

    manager = new QNetworkAccessManager(this);
    //connect(manager,&QNetworkAccessManager::finished,this,&Widget::onfinished);

    //限制下行政区划，只允许数字
    QRegularExpression regex("\\d*");
    QRegularExpressionValidator *validator = new QRegularExpressionValidator(regex,ui->code);
    ui->code->setValidator(validator);

    //限制食物，只允许输入中文字符
    QRegularExpression regex_1("^[\u4e00-\u9fa5\u3000\uFF00-\uFFEF]+$");
    QRegularExpressionValidator *validator_1 = new QRegularExpressionValidator(regex_1, ui->foodLineEdit);
    ui->foodLineEdit->setValidator(validator_1);

}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_getBut_clicked()
{
    if(ui->code->text().isEmpty()){
        //这里可以整理下区划代码的规则，检测错误的区划代码，这里仅检测不能为空
        myError("区划代码不能为空！！");
        return;
    }
    QNetworkRequest req(QUrl(QString("https://zj.v.api.aa1.cn/api/xz/?code=%1").arg(ui->code->text())));
    //qDebug()<< QString("https://zj.v.api.aa1.cn/api/xz/?code=%1").arg(ui->code->text()) << endl;
    QSslConfiguration config = QSslConfiguration::defaultConfiguration();
    config.setProtocol(QSsl::AnyProtocol);//表示可以使用任何支持的SSL/TLS协议版本,用来解决
    config.setPeerVerifyMode(QSslSocket::VerifyNone);
    req.setSslConfiguration(config);

    QNetworkReply* reply = manager->get(req);
    connect(reply,&QNetworkReply::finished,this,&Widget::onfinished);
    //qDebug() << "test1" << endl;

}

void Widget::onfinished()
{
    qDebug() << "test2" << endl;
    QNetworkReply *reply = (QNetworkReply*)(sender());

    if(reply->error() != QNetworkReply::NoError) {
            qDebug() << "QNetworkReply Error: " << reply->errorString();
            myError("QNetworkReply Error"+reply->errorString());
            return;
        }

    QByteArray data = reply->readAll();
    reply->deleteLater();//读取完删除
    if(data.isEmpty()){
        //qDebug() << "test3" << endl;
        myError("getData is empty!");
        return;
    }

    qDebug() << data << endl;
    QJsonDocument jd = QJsonDocument::fromJson(data);
    if(jd.isObject()){
        QJsonObject jo = jd.object();
        if(jo.value("code")!= 200){
             myError("code:" + jo.value("code").toString()+",msg:"+jo.value("msg").toString());
             return;
        }
        QJsonObject joData = jo.value("data").toObject();
        ui->NameLabel->setText(joData.value("Name").toString());
        ui->ProvinceLabel->setText(joData.value("Province").toString());
        ui->CityLabel->setText(joData.value("City").toString());
        ui->DistrictLabel->setText(joData.value("District").toString());
        ui->TowLabel->setText(joData.value("Tow").toString());
        ui->VillagLabel->setText(joData.value("Villag").toString());
        ui->LevelTypeLabel->setText(joData.value("LevelType").toString());

        if(jo.value("ID").isNull()){
            myError("The query result is empty. Please check the code!!");
        }
    }

}

void Widget::onfinished_1()
{
    qDebug() << "test2" << endl;
    QNetworkReply *reply = (QNetworkReply*)(sender());

    if(reply->error() != QNetworkReply::NoError) {
            qDebug() << "QNetworkReply Error: " << reply->errorString();
            myError("QNetworkReply Error: " + reply->errorString());
            return;
        }

    QByteArray data = reply->readAll();
    reply->deleteLater();//读取完删除
    if(data.isEmpty()){
        myError("post data is empty!");
        return;
    }

    qDebug() << data << endl;
    QJsonDocument jd = QJsonDocument::fromJson(data);
    if(jd.isObject()){
        QJsonObject jo = jd.object();
        if(jo.value("code")!=200){
             myError("code:" + jo.value("code").toString()+",msg:"+jo.value("msg").toString());
             return;
        }

        QJsonObject joData = jo.value("data").toObject();

        // 检查"data"下是否存在"lists"数组
        if (joData.contains("lists") && joData["lists"].isArray()) {
            QJsonArray listsArray = joData["lists"].toArray();

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
        }else{
            myError("post lists is empty");
        }
    }
}

void Widget::on_postBut_clicked()
{
    if(ui->foodLineEdit->text().isEmpty()){
        //输入校验：这里仅检测食物不得为空
        myError("食物不能为空！！");
        return;
    }
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

void Widget::myError(const QString &str)
{
    QMessageBox::critical(this, "报错",str);
}

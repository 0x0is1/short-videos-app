#include "mainwindow.h"
#include "ui_mainwindow.h"

static urls urllist;
static QStringList container;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    dropdown = new QComboBox(this);
    player = new QMediaPlayer(this);
    layout = new QVBoxLayout;
    vw = new QVideoWidget(this);
    song_container = new QLabel(this);
    subtitles_container = new QLabel(this);
    subtitles_container->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
    subtitles_container->setWordWrap(true);
    layout->addWidget(vw);
    layout->addWidget(subtitles_container);
    QStringList drop_items = {
        "MOJ App",
        "Josh App",
        "Chingari App"
    };
    dropdown->addItems(drop_items);
    player->setVideoOutput(vw);
    QWidget *cw = new QWidget(this);
    cw->setLayout(layout);
    this->setCentralWidget(cw);
    ui->statusBar->addWidget(dropdown);
    ui->statusBar->addWidget(song_container);
    ui->actionPlay->setDisabled(true);
    ui->actionPause->setDisabled(true);
    ui->actionNext->setDisabled(true);
    ui->actionPrevious->setDisabled(true);
    ui->actionMute->setDisabled(false);
    ui->actionUnmute->setDisabled(true);
    connect(player, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)), this, SLOT(media_changed(QMediaPlayer::MediaStatus)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

// custom functions
void MainWindow::fetch_moj()
{
    mgr = new QNetworkAccessManager(this);
    connect(mgr,SIGNAL(finished(QNetworkReply*)),this,SLOT(onfinish_moj(QNetworkReply*)));
    connect(mgr,SIGNAL(finished(QNetworkReply*)),mgr,SLOT(deleteLater()));
    QNetworkRequest request(urllist.moj_api);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json;charset=utf-8");
    QByteArray data;
    data.append("{\"message\":{\"lang\":\"Hindi\"},\"userId\":\"73475326411\",\"passCode\":\"f3c65e1d0a911696188f\",\"client\":\"web\"}");
    mgr->post(request, data);
}

void MainWindow::fetch_josh()
{
    mgr = new QNetworkAccessManager(this);
    connect(mgr,SIGNAL(finished(QNetworkReply*)),this,SLOT(onfinish_josh(QNetworkReply*)));
    connect(mgr,SIGNAL(finished(QNetworkReply*)),mgr,SLOT(deleteLater()));
    QNetworkRequest request(urllist.josh_api_url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QByteArray data;
    data.append("{\"method\":\"POST\",\"url\":\"" + urllist.josh_next_url + "\",\"body\":{},\"platform\":\"PWA\"}");
    mgr->post(request, data);
}

void MainWindow::fetch_chingari()
{
    mgr = new QNetworkAccessManager(this);
    connect(mgr,SIGNAL(finished(QNetworkReply*)),this,SLOT(onfinish_chingari(QNetworkReply*)));
    connect(mgr,SIGNAL(finished(QNetworkReply*)),mgr,SLOT(deleteLater()));
    QNetworkRequest request(urllist.chingari_api);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json;charset=utf-8");
    QByteArray data;
    data.append("{\"skip\":0,\"limit\":10,\"language\":\"english\"}");
    mgr->post(request, data);
}

void MainWindow::get_icons(QUrl url)
{
    mgr = new QNetworkAccessManager(this);
    connect(mgr,SIGNAL(finished(QNetworkReply*)),this,SLOT(onfinish_get_icon(QNetworkReply*)));
    connect(mgr,SIGNAL(finished(QNetworkReply*)),mgr,SLOT(deleteLater()));
    QNetworkRequest request(url);
    mgr->get(request);
}

/// on finish handlers
void MainWindow::onfinish_moj(QNetworkReply *rep)
{
    playlist = new QMediaPlaylist(this);
    QByteArray bts = rep->readAll();
    QString str(bts), url, title, username, likes, comments, shares, audio_info, user_img;
    QStringList subcontainer;
    QJsonDocument jsonResponse = QJsonDocument::fromJson(str.toUtf8());
    QJsonObject jsonObject, obj, author, audio_meta;
    jsonObject = jsonResponse.object();
    QJsonValue jsonValue = jsonObject.value("payload").toObject().value("d");
    foreach(QJsonValue value,  jsonValue.toArray()){
        obj = value.toObject();
        audio_meta = obj.value("audioMeta").toObject();
        author = obj.value("ath").toObject();

        audio_info = audio_meta.value("audioName").toString()
        + " - " + audio_meta.value("audioText").toString();
        title = obj.value("c").toString();
        username = obj.value("n").toString();
        likes = author.value("likeCount").toString();
        comments = author.value("pc").toString();
        user_img = author.value("tu").toString();
        shares = obj.value("us").toString();
        url = obj.value("attributedVideoUrl").toString();

        QStringList ssubcontainer = {audio_info, username, likes, comments, user_img, shares, title};
        playlist->addMedia(QUrl(url));
        subcontainer.append(ssubcontainer);
    }
    container.clear();
    container.append(subcontainer);
    player->setPlaylist(playlist);
}

void MainWindow::onfinish_josh(QNetworkReply *rep)
{
    playlist = new QMediaPlaylist(this);
    QByteArray bts = rep->readAll();
    QString str(bts), url, title, username, likes, comments, shares, audio_info, user_img;
    QJsonDocument document = QJsonDocument::fromJson(str.toUtf8());
    QJsonObject jsonObject, author, obj;
    QJsonValue next, data;

    jsonObject = document.object();
    next = jsonObject.value("metadata").toObject().value("next");
    data = jsonObject.value("data");
    urllist.josh_next_url = next.toString();
    QStringList subcontainer;
    foreach(QJsonValue value, data.toArray()){
        obj = value.toObject();
        author = obj.value("user_profile").toObject();

        title = obj.value("content_title").toString();
        url = obj.value("url").toString();
        username = author.value("name").toString();
        user_img = author.value("profile_pic").toString();
        likes = obj.value("like_count").toString();
        shares = obj.value("share_count").toString();
        comments = QString::number(obj.value("comments_count").toInt());
        audio_info = obj.value("audio_track_meta").toObject().value("title").toString();

        QStringList ssubcontainer = {audio_info, username, likes, comments, user_img, shares, title};
        subcontainer.append(ssubcontainer);
        playlist->addMedia(QUrl(url));
    }
    container.clear();
    container.append(subcontainer);
    player->setPlaylist(playlist);
}

void MainWindow::onfinish_chingari(QNetworkReply *rep)
{
    playlist = new QMediaPlaylist(this);
    QByteArray bts = rep->readAll();
    QString str(bts), user_img, url, title, username, likes, comments, shares, audio_info, base, video_suburl;
    QJsonDocument jsonResponse = QJsonDocument::fromJson(str.toUtf8());
    QJsonObject jsonObject, obj, medial, author;
    jsonObject = jsonResponse.object();
    QJsonValue jsonValue = jsonObject.value("data").toObject().value("TrendingFeedData");
    QStringList subcontainer = {};

    foreach(QJsonValue value,  jsonValue.toArray()){
        obj = value.toObject();
        medial = obj.value("mediaLocation").toObject();
        author = obj.value("ownerData").toObject();

        base = medial.value("base").toString();
        video_suburl = medial.value("transcoded").toObject().value("p480").toString();
        username = author.value("name").toString();
        url = base + video_suburl;
        likes = QString::number(obj.value("likeCount").toInt());
        comments = QString::number(obj.value("commentCount").toInt());
        shares = QString::number(obj.value("shareCount").toInt());
        title = obj.value("caption").toString();
        audio_info = obj.value("song").toObject().value("title").toString();
        user_img = base + "/apipublic" + author.value("profilePic").toString();
        QStringList ssubcontainer = {audio_info, username, likes, comments, user_img, shares, title};
        subcontainer.append(ssubcontainer);
        playlist->addMedia(QUrl(url));
    }
    container.clear();
    container.append(subcontainer);
    player->setPlaylist(playlist);
}

void MainWindow::onfinish_get_icon(QNetworkReply *rep)
{
    QByteArray bts = rep->readAll();
    QImage auth_img;
    auth_img.loadFromData(bts);
    QPixmap icon = QPixmap::fromImage(auth_img);
    ui->actionAuthor->setIcon(icon);
}

/// other handlers
void MainWindow::media_changed(QMediaPlayer::MediaStatus)
{
    if(playlist->currentIndex() < 1){
        playlist->setCurrentIndex(1);
    }

    if(playlist->currentIndex() >= player->playlist()->mediaCount()-2){
        func_handler();
        player->playlist()->setCurrentIndex(0);
    }

    int ci = player->playlist()->currentIndex();
    int i = (7 * ci);

    get_icons(QUrl(container[i+4]));
    song_container->setText(container[i]);
    ui->actionAuthorName->setToolTip(container[i+1]);
    ui->actionAuthorName->setText(QString::fromStdString(container[i+1].toStdString().substr(0, 5)));
    ui->actionLikes_2->setText(KiloFormat(container[i+2].toInt()));
    ui->actionComments_2->setText(KiloFormat(container[i+3].toInt()));
    ui->actionShare_2->setText(KiloFormat(container[i+5].toInt()));
    QUrl str = container[i+6];
    str.setQuery(str.query(QUrl::FullyDecoded), QUrl::DecodedMode);
    subtitles_container->setText(str.toString());
    this->setWindowTitle(str.toString());
}

void MainWindow::func_handler()
{
    switch (dropdown->currentIndex()) {
    case (0):
        fetch_moj();
        break;
    case (1):
        fetch_josh();
        break;
    case (2):
        fetch_chingari();
        break;
    }
}

QString MainWindow::KiloFormat(double num)
{
    int iterator = 0;
    QString unit = "";
    while (num > 1000.0) {
        num /= 1000.0;
        iterator+=1;
    }
    switch (iterator) {
    case 1:
        unit = "K";
        break;
    case 2:
        unit = "M";
        break;
    case 3:
        unit = "B";
        break;
    }
    return QString::number(num).left(4) + unit;
}

// auto-functions
void MainWindow::on_actionRefresh_triggered()
{
    func_handler();
    ui->actionPlay->setDisabled(false);
    ui->actionPause->setDisabled(true);
}

void MainWindow::on_actionPlay_triggered()
{
    ui->actionPause->setDisabled(false);
    ui->actionNext->setDisabled(false);
    ui->actionPrevious->setDisabled(false);
    ui->actionPlay->setDisabled(true);
    player->play();
}

void MainWindow::on_actionPause_triggered()
{
    player->pause();
    ui->actionPause->setDisabled(true);
    ui->actionPlay->setDisabled(false);
}

void MainWindow::on_actionNext_triggered()
{
    player->playlist()->next();
}

void MainWindow::on_actionPrevious_triggered()
{
    player->playlist()->previous();
}

void MainWindow::on_actionMute_triggered()
{
    ui->actionMute->setDisabled(true);
    ui->actionUnmute->setDisabled(false);
    player->setMuted(true);
}

void MainWindow::on_actionUnmute_triggered()
{
    ui->actionMute->setDisabled(false);
    ui->actionUnmute->setDisabled(true);
    player->setMuted(false);
}

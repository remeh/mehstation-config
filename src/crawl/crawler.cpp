#include <QRegExp>
#include <QNetworkRequest>
#include "crawler.h"

TheGamesDBCrawler::TheGamesDBCrawler(QObject* parent = NULL) :
	QObject(parent),
	reply(NULL) {

	connect(&qnam, SIGNAL(finished(QNetworkReply*)), this, SLOT(gameCrawled(QNetworkReply*)));
}

TheGamesDBCrawler::~TheGamesDBCrawler() {
}

void TheGamesDBCrawler::crawlGame(QString name) {
	QUrl url = QUrl(QString("http://thegamesdb.net/api/GetGame.php?name=" + 
			QUrl::toPercentEncoding(this->clearName(name))));

	qDebug() << "Launching the request: " << url.url();

	// launch the request
	this->qnam.get(QNetworkRequest(url));
}

void TheGamesDBCrawler::gameCrawled(QNetworkReply* reply) {
	if (reply != NULL) {
		qDebug() << reply->readAll();
	}
}

QString TheGamesDBCrawler::clearName(QString name) {
	// first, remove everything between () [] and {}
	name.replace(QRegExp(" *\\([^)]*\\) *"), "");
	name.replace(QRegExp(" *\\{[^)]*\\} *"), "");
	name.replace(QRegExp(" *\\[[^)]*\\] *"), "");

	// then remove every special chars
	QString specialChars("[]()!.-\"'%#:{}");

	QString res;

	for (int i = 0; i < name.length(); i++) {
		QChar c = name.at(i);
		if (!specialChars.contains(c)) {
				res += c;
		}
	}

	qDebug() << res.simplified();
	// remove double spaces
	return res.simplified();
}

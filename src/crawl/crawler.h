#pragma once

#include <QtNetwork>
#include <QObject>
#include <QString>
#include <QNetworkReply>
#include <QNetworkAccessManager>

class TheGamesDBCrawler : public QObject{
	Q_OBJECT
	public:
		TheGamesDBCrawler(QObject* parent);
		~TheGamesDBCrawler();

		void crawlGame(QString name);

	protected:
	private:
		QNetworkAccessManager qnam;
		QNetworkReply *reply;

		QString clearName(QString name);

	public slots:
		void gameCrawled(QNetworkReply* reply);

};

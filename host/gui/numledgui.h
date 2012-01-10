/*
 * USBNumLED GUI
 * © 2012 Maurice Termeer
 */

#include <cmath>

#include <QCheckBox>
#include <QCloseEvent>
#include <QDialog>
#include <QHttp>
#include <QMenu>
#include <QMessageBox>
#include <QSystemTrayIcon>
#include <QTime>
#include <QTimer>
#include <QUrl>
#include <QWidget>

#include "numled.h"
#include "ui_numled_main.h"

static uint8_t led_font[] = {
	0x00, /*   0 */ 0x00, /*   1 */ 0x00, /*   2 */ 0x00, /*   3 */
	0x00, /*   4 */ 0x00, /*   5 */ 0x00, /*   6 */ 0x00, /*   7 */
	0x00, /*   8 */ 0x00, /*   9 */ 0x00, /*  10 */ 0x00, /*  11 */
	0x00, /*  12 */ 0x00, /*  13 */ 0x00, /*  14 */ 0x00, /*  15 */
	0x00, /*  16 */ 0x00, /*  17 */ 0x00, /*  18 */ 0x00, /*  19 */
	0x00, /*  20 */ 0x00, /*  21 */ 0x00, /*  22 */ 0x00, /*  23 */
	0x00, /*  24 */ 0x00, /*  25 */ 0x00, /*  26 */ 0x00, /*  27 */
	0x00, /*  28 */ 0x00, /*  29 */ 0x00, /*  30 */ 0x00, /*  31 */
	0x00, /*  32 */ 0x00, /*  33 */ 0x05, /*  34 */ 0x00, /*  35 */
	0x00, /*  36 */ 0x00, /*  37 */ 0x00, /*  38 */ 0x01, /*  39 */
	0x00, /*  40 */ 0x00, /*  41 */ 0x00, /*  42 */ 0x00, /*  43 */
	0x00, /*  44 */ 0x08, /*  45 */ 0x40, /*  46 */ 0x00, /*  47 */
	0xb7, /*  48 */ 0x81, /*  49 */ 0x3b, /*  50 */ 0xab, /*  51 */
	0x8d, /*  52 */ 0xae, /*  53 */ 0xbe, /*  54 */ 0x83, /*  55 */
	0xbf, /*  56 */ 0xaf, /*  57 */ 0x00, /*  58 */ 0x00, /*  59 */
	0x00, /*  60 */ 0x28, /*  61 */ 0x00, /*  62 */ 0x00, /*  63 */
	0x00, /*  64 */ 0x9f, /*  65 */ 0xbc, /*  66 */ 0x36, /*  67 */
	0xb9, /*  68 */ 0x3e, /*  69 */ 0x1e, /*  70 */ 0xbe, /*  71 */
	0x9c, /*  72 */ 0x81, /*  73 */ 0xa1, /*  74 */ 0x9d, /*  75 */
	0x34, /*  76 */ 0x98, /*  77 */ 0x98, /*  78 */ 0xb8, /*  79 */
	0x1f, /*  80 */ 0x8f, /*  81 */ 0x18, /*  82 */ 0xae, /*  83 */
	0x88, /*  84 */ 0xb0, /*  85 */ 0xb0, /*  86 */ 0xb0, /*  87 */
	0x9d, /*  88 */ 0x8d, /*  89 */ 0x3b, /*  90 */ 0x00, /*  91 */
	0x00, /*  92 */ 0x00, /*  93 */ 0x00, /*  94 */ 0x20, /*  95 */
	0x00, /*  96 */ 0x00, /*  97 */ 0x00, /*  98 */ 0x00, /*  99 */
	0x00, /* 100 */ 0x00, /* 101 */ 0x00, /* 102 */ 0x00, /* 103 */
	0x00, /* 104 */ 0x00, /* 105 */ 0x00, /* 106 */ 0x00, /* 107 */
	0x00, /* 108 */ 0x00, /* 109 */ 0x00, /* 110 */ 0x00, /* 111 */
	0x00, /* 112 */ 0x00, /* 113 */ 0x00, /* 114 */ 0x00, /* 115 */
	0x00, /* 116 */ 0x00, /* 117 */ 0x00, /* 118 */ 0x00, /* 119 */
	0x00, /* 120 */ 0x00, /* 121 */ 0x00, /* 122 */ 0x00, /* 123 */
	0x00, /* 124 */ 0x00, /* 125 */ 0x00, /* 126 */ 0x00, /* 127 */
};

class NumledGuiWindow : public QDialog
{
	Q_OBJECT

public:
	NumledGuiWindow(QWidget *parent = NULL)
		: QDialog(parent), handle(NULL), offset(0), timer(NULL)
	{
		ui.setupUi(this);
		http = new QHttp(this);
		connect(http, SIGNAL(requestFinished(int,bool)),
			this, SLOT(stockRequestFinished(int,bool)));
		ui.comboBoxContent->setCurrentIndex(0);
		ui.stackedWidget->setCurrentIndex(0);
		ui.textEditStockError->hide();

		menu = new QMenu(this);
		actionMode0 = new QAction(tr("&Raw"), this);
		actionMode0->setCheckable(true);
		actionMode0->setChecked(true);
		connect(actionMode0, SIGNAL(triggered()), this, SLOT(setMode0()));
		menu->addAction(actionMode0);
		actionMode1 = new QAction(tr("&Text"), this);
		actionMode1->setCheckable(true);
		connect(actionMode1, SIGNAL(triggered()), this, SLOT(setMode1()));
		menu->addAction(actionMode1);
		actionMode2 = new QAction(tr("&Stock"), this);
		actionMode2->setCheckable(true);
		connect(actionMode2, SIGNAL(triggered()), this, SLOT(setMode2()));
		menu->addAction(actionMode2);
		actionMode3 = new QAction(tr("T&ime"), this);
		actionMode3->setCheckable(true);
		connect(actionMode3, SIGNAL(triggered()), this, SLOT(setMode3()));
		menu->addAction(actionMode3);

		menu->addSeparator();

		QAction *actionQuit = new QAction(tr("&Quit"), this);
		connect(actionQuit, SIGNAL(triggered()), qApp, SLOT(quit()));
		menu->addAction(actionQuit);

		tray = new QSystemTrayIcon(this);
		tray->setIcon(QIcon(":/icon.svg"));
		tray->setToolTip("USBNumLED");
		tray->setContextMenu(menu);

		connect(tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
			this, SLOT(onTrayActivated(QSystemTrayIcon::ActivationReason)));

		tray->show();

		set_connection_state(false);

		setWindowIcon(QIcon(":/icon.svg"));
		setWindowTitle("USBNumLED");
	}

	~NumledGuiWindow()
	{
		if (handle != NULL) {
			numled_close(handle);
		}

		if (timer != NULL) delete timer;
		delete http;
		delete tray;
	}

protected:
	void closeEvent(QCloseEvent *event)
	{
		if (tray->isVisible()) {
			hide();
			event->ignore();
		}
	}

private slots:
	void on_pushButtonConnect_clicked()
	{
		if (handle == NULL) {
			handle = numled_open(NULL);
			if (handle == NULL) {
				QMessageBox::critical(
					this,
					"Numled error",
					"Device not found."
				);
			} else {
				read_state();
				set_connection_state(true);
			}
		} else {
			numled_close(handle);
			handle = NULL;
			set_connection_state(false);
		}
	}

	void on_comboBoxContent_currentIndexChanged(int index)
	{
		ui.stackedWidget->setCurrentIndex(index);

		actionMode0->setChecked(false);
		actionMode1->setChecked(false);
		actionMode2->setChecked(false);
		actionMode3->setChecked(false);

		switch (index) {
			case 0: actionMode0->setChecked(true); break;
			case 1: actionMode1->setChecked(true); break;
			case 2: actionMode2->setChecked(true); break;
			case 3: actionMode3->setChecked(true); break;
		};

		if (timer != NULL) {
			delete timer;
			timer = NULL;
		}

		if (handle == NULL) return;

		if (index == 0) {
			read_state();
		} else if (index == 1) {
			timer = new QTimer(this);
			connect(timer, SIGNAL(timeout()), this, SLOT(timerText()));
			timer->start(10000 / ui.horizontalSliderSpeed->value());
			timerText();
		} else if (index == 2) {
			timer = new QTimer(this);
			connect(timer, SIGNAL(timeout()), this, SLOT(timerStock()));
			timer->start(15 * 60 * 1000);
			timerStock();
		} else if (index == 3) {
			timer = new QTimer(this);
			connect(timer, SIGNAL(timeout()), this, SLOT(timerTime()));
			timer->start(1000);
			timerTime();
		}
	}

	void on_horizontalSliderBrightness_valueChanged(int value)
	{
		int newBrightness = int(255.0f * powf(float(value) / 255.0f, 2.2f));
		if (state.brightness != newBrightness) {
			state.brightness = newBrightness;
			write_state();
		}
	}

	void on_lineEdit0_textChanged(const QString &text)
	{
		bool ok;
		int value = text.toInt(&ok, 16);

		if (text.length() == 2 && ok && state.digits[0] != value) {
			state.digits[0] = value;
			write_state();
		}
	}

	void on_lineEdit1_textChanged(const QString &text)
	{
		bool ok;
		int value = text.toInt(&ok, 16);

		if (text.length() == 2 && ok && state.digits[1] != value) {
			state.digits[1] = value;
			write_state();
		}
	}

	void on_lineEdit2_textChanged(const QString &text)
	{
		bool ok;
		int value = text.toInt(&ok, 16);

		if (text.length() == 2 && ok && state.digits[2] != value) {
			state.digits[2] = value;
			write_state();
		}
	}

	void on_lineEdit3_textChanged(const QString &text)
	{
		bool ok;
		int value = text.toInt(&ok, 16);

		if (text.length() == 2 && ok && state.digits[3] != value) {
			state.digits[3] = value;
			write_state();
		}
	}

	void on_horizontalSliderSpeed_valueChanged(int value)
	{
		timer->setInterval(1000 - int(1000 * powf(value / 1000.0f, 0.25f)));
	}

	void on_lineEditExchange_textChanged(const QString &text)
	{
		Q_UNUSED(text);
		timerStock();
	}

	void on_lineEditStock_textChanged(const QString &text)
	{
		Q_UNUSED(text);
		timerStock();
	}

	void on_pushButtonUpdateStock_clicked()
	{
		timerStock();
	}

	void on_lineEditTimeOffset_textChanged(const QString &text)
	{
		Q_UNUSED(text);
		timerTime();
	}

	void timerText()
	{
		if (handle == NULL) return;
		set_text(ui.lineEditText->text());
	}

	void timerStock()
	{
		if (handle == NULL) return;

		const QString exchange = ui.lineEditExchange->text();
		const QString stock = ui.lineEditStock->text();
		get_stock_quote(exchange, stock);
	}

	void stockRequestFinished(int id, bool error)
	{
		Q_UNUSED(id);

		if (!error) {
			QString data(http->readAll());
			data = data.remove("\n").simplified();
			QRegExp re("\\/\\/\\s*\\[\\s*\\{\\s*(\\\"([\\w.]+)\\\"\\s*:\\s*\\\"(.?[\\w.,:\\+\\-\\s]+)\\\"\\s*,?\\s*)+\\}\\s*\\]");

			if (re.exactMatch(data)) {
				data = data.remove(QRegExp("[\\/\\[\\]\\{\\}]"));
				QStringList tokens = data.split(",");

				QMap<QString,QString> map;
				for (int i = 0; i < tokens.count(); ++i) {
					QRegExp ret("\\s*\\\"([\\w.]+)\\\"\\s*:\\s*\\\"([\\w.,:\\-\\s]+)\\\"\\s*");
					if (ret.exactMatch(tokens[i])) {
						map[ret.cap(1)] = ret.cap(2);
					}
				}

				QString value = map["l"];
				set_text(value);

				QString time = QTime::currentTime().toString("H:mm:ss");
				QString log = QString("%1: Stock value %2").arg(time).arg(value);
				ui.labelStockStatus->setText(log);
				ui.textEditStockError->hide();
			} else {
				QString time = QTime::currentTime().toString("H:mm:ss");
				QString log = QString("%1: Invalid server response").arg(time);
				ui.labelStockStatus->setText(log);
				ui.textEditStockError->setText(data);
				ui.textEditStockError->show();
			}
		} else {
			QString time = QTime::currentTime().toString("H:mm:ss");
			QString log = QString("%1: Error getting stock quote:\n%2").arg(time).arg(http->errorString());
			ui.labelStockStatus->setText(log);
			ui.textEditStockError->hide();
		}
	}

	void timerTime()
	{
		if (handle == NULL) return;

		QTime time = QTime::currentTime();
		time = time.addSecs(ui.lineEditTimeOffset->text().toInt());

		QString format("HHmm");

		if (QTime::currentTime().second() % 2 == 0) {
			format = "HH.mm";
		}

		set_text(time.toString(format));

		ui.labelTime->setText(QString("Current time: %1").arg(time.toString("HH:mm:ss")));
	}

	void onTrayActivated(QSystemTrayIcon::ActivationReason reason)
	{
		if (reason == QSystemTrayIcon::Context) {
			menu->show();
		} else if (reason == QSystemTrayIcon::DoubleClick) {
			show();
		}
	}

	void setMode0()
	{
		ui.comboBoxContent->setCurrentIndex(0);
	}

	void setMode1()
	{
		ui.comboBoxContent->setCurrentIndex(1);
	}

	void setMode2()
	{
		ui.comboBoxContent->setCurrentIndex(2);
	}

	void setMode3()
	{
		ui.comboBoxContent->setCurrentIndex(3);
	}

private:
	void set_connection_state(bool connected)
	{
		ui.pushButtonConnect->setText(connected ? "Disconnect" : "Connect");
		ui.comboBoxContent->setEnabled(connected);
		ui.horizontalSliderBrightness->setEnabled(connected);
		ui.lineEdit0->setEnabled(connected);
		ui.lineEdit1->setEnabled(connected);
		ui.lineEdit2->setEnabled(connected);
		ui.lineEdit3->setEnabled(connected);
		ui.lineEditText->setEnabled(connected);
		ui.horizontalSliderSpeed->setEnabled(connected);
		ui.lineEditExchange->setEnabled(connected);
		ui.lineEditStock->setEnabled(connected);
		ui.pushButtonUpdateStock->setEnabled(connected);
		ui.lineEditTimeOffset->setEnabled(connected);
		actionMode0->setEnabled(connected);
		actionMode1->setEnabled(connected);
		actionMode2->setEnabled(connected);
		actionMode3->setEnabled(connected);
	}

	void read_state()
	{
		state = numled_read(handle, NULL);
		update_state_ui();
	}

	void update_state_ui()
	{
		ui.lineEdit0->blockSignals(true);
		ui.lineEdit0->setText(QString("%1").arg(state.digits[0], 2, 16, QLatin1Char('0')).right(2));
		ui.lineEdit0->blockSignals(false);

		ui.lineEdit1->blockSignals(true);
		ui.lineEdit1->setText(QString("%1").arg(state.digits[1], 2, 16, QLatin1Char('0')).right(2));
		ui.lineEdit1->blockSignals(false);

		ui.lineEdit2->blockSignals(true);
		ui.lineEdit2->setText(QString("%1").arg(state.digits[2], 2, 16, QLatin1Char('0')).right(2));
		ui.lineEdit2->blockSignals(false);

		ui.lineEdit3->blockSignals(true);
		ui.lineEdit3->setText(QString("%1").arg(state.digits[3], 2, 16, QLatin1Char('0')).right(2));
		ui.lineEdit3->blockSignals(false);

		ui.horizontalSliderBrightness->blockSignals(true);
		int value = int(255.0f * powf(float(state.brightness) / 255.0f, 1.0f / 2.2f));
		ui.horizontalSliderBrightness->setValue(value);
		ui.horizontalSliderBrightness->blockSignals(false);
	}

	void write_state()
	{
		numled_write(handle, &state);
	}

	QString exdot_string(const QString &str)
	{
		QString result;

		for (int i = 0; i < str.length(); ++i) {
			result += str[i];

			if (i != str.length() - 1 && str[i + 1] == '.') {
				result += '.';
				++i;
			} else {
				result += ' ';
			}
		}

		return result;
	}

	void set_text(const QString &rtext)
	{
		QString text = exdot_string(rtext.toUpper());

		if (text.length() <= 8) {
			offset = 0;
		} else {
			offset = (offset + 1) % (text.length() / 2);
		}

		if (text.length() < 8) text = (QString("        ") + text).right(8);
		text = (text + text).mid(2 * offset, 8);

		QByteArray textLatin1 = text.toLatin1();
		const char *textAscii = textLatin1.data();

		for (int i = 0; i < 4; ++i) {
			state.digits[i] = led_font[textAscii[2 * i] & 0x7f] | (textAscii[2 * i + 1] == '.' ? 0x40 : 0);
		}

		write_state();
	}

	void get_stock_quote(const QString &exchange, const QString &stock)
	{
		http->setProxy(ui.lineEditProxyHost->text(), ui.lineEditProxyPort->text().toInt());
		http->setHost("finance.google.com");
		http->get(QString("/finance/info?client=ig&q=%1:%2").arg(exchange).arg(stock));
		//http://finance.google.com/finance/info?client=ig&q=CURRENCY:EURUSD
		//// [ { "id": "-2001" ,"t" : "EURUSD" ,"e" : "CURRENCY" ,"l" : "1.2701" ,"l_cur" : "" ,"s": "0" ,"ltt":"" ,"lt" : "Jan 9, 10:45PM GMT" ,"c" : "-0.00900" ,"cp" : "-0.704" ,"ccol" : "chr" } ] 
		////[{"id":"667416","t": "PHIA","e": "AMS","l" : "15.65","l_cur": "15.65","s": "0","ltt":"1:56PM CET","lt": "Jan 9, 1:56PM CET","c": "0.00","cp": "0.00","ccol": "chb"}]
	}

	NumledHandle handle;
	NumledState state;
	int offset;
	QTimer *timer;
	QHttp *http;
	Ui::Form ui;
	QAction *actionMode0;
	QAction *actionMode1;
	QAction *actionMode2;
	QAction *actionMode3;
	QMenu *menu;
	QSystemTrayIcon *tray;
};


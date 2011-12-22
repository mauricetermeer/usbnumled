#include <QCheckBox>
#include <QMessageBox>
#include <QTimer>
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
	0x00, /*  32 */ 0x00, /*  33 */ 0x0a, /*  34 */ 0x00, /*  35 */
	0x00, /*  36 */ 0x00, /*  37 */ 0x00, /*  38 */ 0x08, /*  39 */
	0x00, /*  40 */ 0x00, /*  41 */ 0x00, /*  42 */ 0x00, /*  43 */
	0x00, /*  44 */ 0x01, /*  45 */ 0x80, /*  46 */ 0x00, /*  47 */
	0x7e, /*  48 */ 0x48, /*  49 */ 0x3d, /*  50 */ 0x6d, /*  51 */
	0x4b, /*  52 */ 0x67, /*  53 */ 0x77, /*  54 */ 0x4c, /*  55 */
	0x7f, /*  56 */ 0x6f, /*  57 */ 0x00, /*  58 */ 0x00, /*  59 */
	0x00, /*  60 */ 0x21, /*  61 */ 0x00, /*  62 */ 0x00, /*  63 */
	0x00, /*  64 */ 0x5f, /*  65 */ 0x73, /*  66 */ 0x36, /*  67 */
	0x79, /*  68 */ 0x37, /*  69 */ 0x17, /*  70 */ 0x77, /*  71 */
	0x5b, /*  72 */ 0x48, /*  73 */ 0x68, /*  74 */ 0x5b, /*  75 */
	0x32, /*  76 */ 0x51, /*  77 */ 0x51, /*  78 */ 0x71, /*  79 */
	0x1f, /*  80 */ 0x4f, /*  81 */ 0x11, /*  82 */ 0x67, /*  83 */
	0x41, /*  84 */ 0x70, /*  85 */ 0x70, /*  86 */ 0x70, /*  87 */
	0x5b, /*  88 */ 0x4b, /*  89 */ 0x3d, /*  90 */ 0x00, /*  91 */
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

class NumledGuiWindow : public QWidget
{
	Q_OBJECT

public:
	NumledGuiWindow(QWidget *parent = NULL)
		: QWidget(parent), handle(NULL), offset(0)
	{
		ui.setupUi(this);
		set_connection_state(false);
		timer = new QTimer(this);
		connect(timer, SIGNAL(timeout()), this, SLOT(timerTimeout()));
		timer->start(ui.horizontalSliderSpeed->value());
	}

	~NumledGuiWindow()
	{
		if (handle != NULL) {
			numled_close(handle);
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
				set_connection_state(true);
				read_state();
			}
		} else {
			numled_close(handle);
			handle = NULL;
			set_connection_state(false);
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

	void on_horizontalSliderBrightness_valueChanged(int value)
	{
		if (state.brightness != value) {
			state.brightness = value;
			write_state();
		}
	}

	void on_checkBoxText_stateChanged(int state)
	{
		ui.lineEditText->setEnabled(state == Qt::Checked);
		ui.horizontalSliderSpeed->setEnabled(state == Qt::Checked);
	}

	void on_horizontalSliderSpeed_valueChanged(int value)
	{
		timer->setInterval(value);
	}

	void timerTimeout()
	{
		if (!ui.checkBoxText->isChecked()) return;

		QString text = exdot_string(ui.lineEditText->text().toUpper());

		if (text.length() <= 8) {
			offset = 0;
		} else {
			offset = (offset + 1) % (text.length() / 2);
		}

		if (text.length() < 8) text = (QString("        ") + text).right(8);

		QByteArray lineLatin1 = (text + text).mid(2 * offset, 8).toLatin1();
		const char *lineAscii = lineLatin1.data();

		for (int i = 0; i < 4; ++i) {
			state.digits[i] = led_font[lineAscii[2 * i] & 0x7f] | (lineAscii[2 * i + 1] == '.' ? 128 : 0);
		}

		update_state_ui();
		write_state();
	}

private:
	void set_connection_state(bool connected)
	{
		ui.pushButtonConnect->setText(connected ? "Disconnect" : "Connect");
		ui.lineEdit0->setEnabled(connected);
		ui.lineEdit1->setEnabled(connected);
		ui.lineEdit2->setEnabled(connected);
		ui.lineEdit3->setEnabled(connected);
		ui.horizontalSliderBrightness->setEnabled(connected);
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
		ui.horizontalSliderBrightness->setValue(state.brightness);
		ui.horizontalSliderBrightness->blockSignals(false);
	}

	void write_state()
	{
		numled_write(handle, state);
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

	NumledHandle handle;
	NumledState state;
	int offset;
	QTimer *timer;
	Ui::Form ui;
};


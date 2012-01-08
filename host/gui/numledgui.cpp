#include <QApplication>

#include "numledgui.h"
#include "numled.h"

int main(int argc, char **argv)
{
	QApplication app(argc, argv);

	QApplication::setQuitOnLastWindowClosed(false);

	NumledGuiWindow window;
	window.show();

	return app.exec();
}


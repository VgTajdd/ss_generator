#include <QApplication>
#include <QPalette>
#include "SSGeneratorUI.h"

int main( int argc, char* argv[] )
{
#if defined(_DEBUG)
    _CrtSetDbgFlag( _CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF );
#endif

    QApplication app( argc, argv );
    app.setPalette( Qt::darkGray );

    SSGeneratorUI ssgenUI;
    ssgenUI.show();

    return app.exec();
}
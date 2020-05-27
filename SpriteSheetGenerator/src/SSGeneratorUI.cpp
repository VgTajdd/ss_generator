#include "SSGeneratorUI.h"

//#include <QDebug>
//#include <QFileDialog>
//#include <QListWidgetItem>

SSGeneratorUI::SSGeneratorUI( QWidget* parent ) :
    QMainWindow( parent ),
    m_ui( new Ui::SSGeneratorUI )
{
    m_ui->setupUi( this );
}

SSGeneratorUI::~SSGeneratorUI()
{
    delete m_ui;
}
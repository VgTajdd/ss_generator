#include "SSGeneratorUI.h"

#include <QFileDialog>
//#include <QDebug>
//#include <QListWidgetItem>

SSGeneratorUI::SSGeneratorUI( QWidget* parent ) :
    QMainWindow( parent ),
    m_ui( new Ui::SSGeneratorUI )
{
    m_ui->setupUi( this );

    // Hiding unnecessary controls.
    m_ui->menubar->setVisible( false );
    m_ui->statusbar->setVisible( false );

    m_ui->folderLabel->setText( "..." );

    m_ui->comboBox->addItem( "256 x 256" );
    m_ui->comboBox->addItem( "512 x 512" );
    m_ui->comboBox->addItem( "1024 x 1024" );
    m_ui->comboBox->addItem( "2048 x 2048" );

    // Connections.
    connect( m_ui->folderButton, &QPushButton::clicked, this, &SSGeneratorUI::btnFolderSlot );
    connect( m_ui->generateButton, &QPushButton::clicked, this, &SSGeneratorUI::btnGenerateSlot );
    connect( m_ui->checkBox, &QCheckBox::toggled, [this]( bool checked )
    {
        m_ui->comboBox->setDisabled( checked );
    } );
}

void SSGeneratorUI::btnFolderSlot()
{
    QString folderPath = QFileDialog::getExistingDirectory( this,
                                                            tr( "Select images folder" ),
                                                            "",
                                                            QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks );
    m_ui->folderLabel->setText( folderPath );

    m_ui->listWidget->clear();

    if ( !folderPath.isEmpty() )
    {
        QDir dir( folderPath );
        QStringList filter;
        filter << QLatin1String( "*.png" );
        filter << QLatin1String( "*.jpeg" );
        filter << QLatin1String( "*.jpg" );
        dir.setNameFilters( filter );
        QFileInfoList filelistinfo = dir.entryInfoList();
        foreach( const QFileInfo& fileinfo, filelistinfo )
        {
            QString imageFile = fileinfo.absoluteFilePath();
            m_ui->listWidget->addItem( imageFile );
        }
    }
}

void SSGeneratorUI::btnGenerateSlot()
{

}

SSGeneratorUI::~SSGeneratorUI()
{
    delete m_ui;
}
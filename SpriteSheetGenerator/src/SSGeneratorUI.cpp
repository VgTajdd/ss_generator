#include "SSGeneratorUI.h"

#include "SSGenerator.h"

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
    m_filenames.clear();

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
            /*QString imageFile = fileinfo.absoluteFilePath();*/
            m_ui->listWidget->addItem( fileinfo.fileName() );
            m_filenames.push_back( fileinfo.fileName() );
        }
    }
}

void SSGeneratorUI::btnGenerateSlot()
{
    if ( m_filenames.empty() || m_ui->folderLabel->text().isEmpty() )
    {
        return;
    }

    QSize size( 256, 256 );
    switch ( m_ui->comboBox->currentIndex() )
    {
        case 1: size = { 512, 512 };    break;
        case 2: size = { 1024, 1024 };  break;
        case 3: size = { 2048, 2048 };  break;
        default:                        break;
    }

    std::vector< QString > spriteSheets;
    bool ok = SSGenerator::generateSpriteSheets( spriteSheets, m_filenames, m_ui->folderLabel->text(), m_ui->checkBox->isChecked(), size );

    if ( ok )
    {
        // Show atlas.
        QImage imageAtlas( spriteSheets[0] );
        QLabel* myLabel = new QLabel;
        myLabel->setPixmap( QPixmap::fromImage( imageAtlas ) );
        QGridLayout* layout = new QGridLayout( m_ui->scrollAreaWidgetContents );
        layout->addWidget( myLabel );
    }
}

SSGeneratorUI::~SSGeneratorUI()
{
    delete m_ui;
}
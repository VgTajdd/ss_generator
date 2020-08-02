#include "SSGeneratorUI.h"

#include "SSGenerator.h"

#include <QFileDialog>

SSGeneratorUI::SSGeneratorUI( QWidget* parent ) :
    QMainWindow( parent ),
    m_ui( new Ui::SSGeneratorUI ),
    m_currentSS( 0 )
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

    std::map< SSGenerator::SORT_BY, const char* > mapItemNames;
    mapItemNames.emplace( SSGenerator::SORT_BY::WIDTH, "Width" );
    mapItemNames.emplace( SSGenerator::SORT_BY::HEIGHT, "Height" );
    mapItemNames.emplace( SSGenerator::SORT_BY::WIDER_SIDE, "Wider side" );
    mapItemNames.emplace( SSGenerator::SORT_BY::AREA, "Area" );

    const int totalSortByItems = (int) SSGenerator::SORT_BY::TOTAL;
    for ( int i = 0; i < totalSortByItems; i++ )
    {
        m_ui->comboBoxSortBy->addItem( mapItemNames[( SSGenerator::SORT_BY )i] );
    }

    m_atlasLabel = new QLabel;
    QGridLayout* layout = new QGridLayout( m_ui->scrollAreaWidgetContents );
    layout->addWidget( m_atlasLabel );

    // Connections.
    connect( m_ui->folderButton, &QPushButton::clicked, this, &SSGeneratorUI::btnFolderSlot );
    connect( m_ui->generateButton, &QPushButton::clicked, this, &SSGeneratorUI::btnGenerateSlot );
    connect( m_ui->checkBox, &QCheckBox::toggled, [this]( bool checked )
    {
        m_ui->comboBox->setDisabled( checked );
    } );
    connect( m_ui->nextAtlasButton, &QPushButton::clicked, [this]()
    {
        if ( this->m_spriteSheets.empty() ) return;
        this->showAtlas( ( this->m_currentSS + 1 ) % (int) this->m_spriteSheets.size() );
    } );
    connect( m_ui->previousAtlasButton, &QPushButton::clicked, [this]()
    {
        if ( this->m_spriteSheets.empty() ) return;
        this->showAtlas( ( this->m_currentSS - 1 ) % (int) this->m_spriteSheets.size() );
    } );
}

void SSGeneratorUI::btnFolderSlot()
{
    QString folderPath = QFileDialog::getExistingDirectory( this,
                                                            tr( "Select images folder" ),
                                                            "",
                                                            QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks );

    if ( folderPath.isEmpty() )
    {
        return;
    }

    m_ui->folderLabel->setText( folderPath );

    // Reset everything.
    m_ui->listWidget->clear();
    m_filenames.clear();
    m_spriteSheets.clear();
    showAtlas( -1 );

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
            QListWidgetItem* item = new QListWidgetItem( m_ui->listWidget );
            item->setSizeHint( QSize( item->sizeHint().width(), 18 ) );

            QWidget* widgetItem = new QWidget( m_ui->listWidget );
            widgetItem->setFixedHeight( 18 );
            QGridLayout* layout = new QGridLayout( widgetItem );
            QLabel* label = new QLabel( fileinfo.fileName(), m_ui->listWidget );
            QPushButton* removeButton = new QPushButton( m_ui->listWidget );
            removeButton->setFixedSize( 18, 18 );
            removeButton->setText( "X" );
            layout->addWidget( label, 0, 0 );
            layout->addItem( new QSpacerItem( QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum ), 1, 1 );
            layout->addWidget( removeButton, 0, 2 );
            layout->setMargin( 0 );
            layout->setSpacing( 0 );

            m_ui->listWidget->setItemWidget( item, widgetItem );

            connect( removeButton, &QPushButton::clicked, [this, item, fileinfo]()
            {
                item->setHidden( true );
                auto it = std::find( m_filenames.begin(), m_filenames.end(), fileinfo.fileName() );
                if ( it != m_filenames.end() )
                {
                    this->m_filenames.erase( it );
                }
            } );

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

    m_spriteSheets.clear();
    showAtlas( -1 );
    m_currentSS = 0;

    SSGenerator::SORT_BY sortBy = ( SSGenerator::SORT_BY ) m_ui->comboBoxSortBy->currentIndex();

    bool ok = SSGenerator::generateSpriteSheets( m_spriteSheets,
                                                 m_filenames,
                                                 m_ui->folderLabel->text(),
                                                 m_ui->checkBox->isChecked(),
                                                 size,
                                                 sortBy );
    if ( ok )
    {
        showAtlas( 0 );
    }
}

void SSGeneratorUI::showAtlas( const int index )
{
    m_atlasLabel->clear();
    m_ui->atlasNameLabel->setText( "" );

    if ( index < 0 || m_spriteSheets.empty() || index >= m_spriteSheets.size() )
    {
        return;
    }

    QImage imageAtlas( m_spriteSheets[index] );
    m_atlasLabel->setPixmap( QPixmap::fromImage( imageAtlas ) );
    m_ui->atlasNameLabel->setText( m_spriteSheets[index] );

    m_currentSS = index;

}

SSGeneratorUI::~SSGeneratorUI()
{
    delete m_ui;
}
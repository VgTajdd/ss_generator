#include "SSGenerator.h"

#include <QImage>
#include <QDebug>
#include <QPainter>
#include <QDateTime>

Node* SSGenerator::root = nullptr;
std::vector< Node* > SSGenerator::roots;

Node::~Node()
{
	if ( right != nullptr )
	{
		delete right;
		right = nullptr;
	}
	if ( down != nullptr )
	{
		delete down;
		down = nullptr;
	}
}

void SSGenerator::fit( std::vector< Image >& images, const bool automaticSize, const QSize& fixedSize )
{
	for ( int n = 0; n < images.size(); n++ )
	{
		auto& block = images[n];
		if ( auto node = findNode( root, block.w, block.h ) )
		{
			block.fit = splitNode( node, block.w, block.h );
		}
		else
		{
			if ( automaticSize )
			{
				block.fit = growNode( block.w, block.h );
			}
			else
			{
				// Create new root.
				roots.push_back( root );
				root = new Node();
				root->w = fixedSize.width();
				root->h = fixedSize.height();

				if ( auto node = findNode( root, block.w, block.h ) )
				{
					block.fit = splitNode( node, block.w, block.h );
				}
				else
				{
					// Something bad happened...
				}
			}
		}
	}
}

Node* SSGenerator::findNode( Node* root, float w, float h )
{
	if ( root->used )
	{
		if ( Node* node = findNode( root->right, w, h ) )
			return node;
		if ( Node* node = findNode( root->down, w, h ) )
			return node;
	}
	else if ( ( w <= root->w ) && ( h <= root->h ) )
	{
		return root;
	}
	else
	{
		return nullptr;
	}
	return nullptr;
}

Node* SSGenerator::splitNode( Node* node, float w, float h )
{
	node->used = true;
	node->down = new Node();
	node->down->x = node->x;
	node->down->y = node->y + h;
	node->down->w = node->w;
	node->down->h = node->h - h;
	node->right = new Node();
	node->right->x = node->x + w;
	node->right->y = node->y;
	node->right->w = node->w - w;
	node->right->h = node->h;
	return node;
}

Node* SSGenerator::growNode( float w, float h )
{
	bool canGrowDown = ( w <= root->w );
	bool canGrowRight = ( h <= root->h );

	bool shouldGrowRight = canGrowRight && ( root->h >= ( root->w + w ) ); // attempt to keep square-ish by growing right when height is much greater than width
	bool shouldGrowDown = canGrowDown && ( root->w >= ( root->h + h ) ); // attempt to keep square-ish by growing down  when width  is much greater than height

	if ( shouldGrowRight ) return growRight( w, h );
	else if ( shouldGrowDown ) return growDown( w, h );
	else if ( canGrowRight ) return growRight( w, h );
	else if ( canGrowDown ) return growDown( w, h );
	else return nullptr; // need to ensure sensible root starting size to avoid this happening
}

Node* SSGenerator::growRight( float w, float h )
{
	Node* newRoot = new Node();
	newRoot->used = true;
	newRoot->x = 0;
	newRoot->y = 0;
	newRoot->w = root->w + w;
	newRoot->h = root->h;
	Node* right = new Node();
	right->x = root->w;
	right->w = w;
	right->h = root->h;
	newRoot->down = root;
	newRoot->right = right;

	root = newRoot;

	if ( auto node = findNode( root, w, h ) )
	{
		return splitNode( node, w, h );
	}
	else
	{
		return nullptr;
	}
}

Node* SSGenerator::growDown( float w, float h )
{
	Node* newRoot = new Node();
	newRoot->used = true;
	newRoot->x = 0;
	newRoot->y = 0;
	newRoot->w = root->w;
	newRoot->h = root->h + h;
	Node* down = new Node();
	down->y = root->h;
	down->w = root->w;
	down->h = h;
	newRoot->down = down;
	newRoot->right = root;

	root = newRoot;

	if ( auto node = findNode( root, w, h ) )
	{
		return splitNode( node, w, h );
	}
	else
	{
		return nullptr;
	}
}

bool SSGenerator::generateSpriteSheets( std::vector< QString >& spriteSheets,
										const std::vector< QString >& filenames,
										const QString& folderPath,
										const bool automaticSize,
										const QSize& fixedSize )
{
	std::vector< Image > images;

	int maxWidth = 0, maxHeight = 0;

	bool loadImagesFailed = false;
	for ( const auto& filename : filenames )
	{
		QString imgPath( folderPath + "/" + filename );
		QImage img( imgPath );
		if ( img.isNull() )
		{
			loadImagesFailed = true;
			break;
		}
		Image imgObject;
		imgObject.filename = filename;
		imgObject.w = img.width();
		imgObject.h = img.height();
		maxWidth = std::max( maxWidth, imgObject.w );
		maxHeight = std::max( maxHeight, imgObject.h );
		images.push_back( imgObject );
	}

	if ( loadImagesFailed )
	{
		return false;
	}

	// Sort by width (descendent).
	std::sort( images.begin(), images.end(), []( const Image& a, const Image& b ) { return a.w > b.w; } );

	if ( root != nullptr )
	{
		delete root;
		root = nullptr;
	}

	root = new Node();
	if ( !automaticSize )
	{
		if ( maxWidth > fixedSize.width() || maxHeight > fixedSize.height() )
		{
			delete root;
			root = nullptr;
			return false;
		}
		root->w = fixedSize.width();
		root->h = fixedSize.height();
	}
	else
	{
		root->w = images[0].w;
		root->h = images[0].h;
	}

	fit( images, automaticSize, fixedSize );

	QString fmt = "_yyyyMMdd_hhmmss";
	QString time = QDateTime::currentDateTime().toString( fmt );

	QImage atlas( QSize( root->w, root->h ), QImage::Format_RGBA8888 );
	atlas.fill( Qt::transparent );

	QPainter painter( &atlas );
	for ( const auto& image : images )
	{
		// Create atlas.
		QString imgPath( folderPath + "/" + image.filename );
		painter.drawImage( QPoint( image.fit->x, image.fit->y ), QImage( imgPath ) );
	}
	painter.end();

	QString atlasFilename = folderPath + "/atlas" + time + ".png";
	atlas.save( atlasFilename, "PNG" );
	spriteSheets.push_back( atlasFilename );

	if ( root != nullptr )
	{
		delete root;
		root = nullptr;
	}

	return true;
}
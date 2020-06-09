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

void SSGenerator::fit( std::vector< Image >& images, 
					   const bool automaticSize, 
					   const QSize& fixedSize )
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
				root = new Node( 0, 0, fixedSize.width(), fixedSize.height() );

				if ( auto node = findNode( root, block.w, block.h ) )
				{
					block.fit = splitNode( node, block.w, block.h );
				}
				else
				{
					// Something bad happened...
					qDebug() << "What happened?";
				}
			}
		}
	}
}

Node* SSGenerator::findNode( Node* nodeRoot, float w, float h )
{
	if ( nodeRoot->used )
	{
		Node* downNode = findNode( nodeRoot->down, w, h );
		Node* rightNode = findNode( nodeRoot->right, w, h );

		if ( downNode != nullptr && rightNode != nullptr )
		{
			int newLocalWidth = rightNode->x + w;
			int newLocalHeight = downNode->y + h;
			if ( newLocalHeight > newLocalWidth )
			{
				return rightNode;
			}
			else
			{
				return downNode;
			}
		}
		else
		{
			if ( rightNode ) return rightNode;
			if ( downNode ) return downNode;
		}
	}
	else if ( ( w <= nodeRoot->w ) && ( h <= nodeRoot->h ) )
	{
		return nodeRoot;
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
	node->down = new Node( node->x , node->y + h, node->w, node->h - h );
	node->right = new Node( node->x + w, node->y, node->w - w, h );
	return node;
}

Node* SSGenerator::growNode( float w, float h )
{
	bool canGrowDown = ( w <= root->w );
	bool canGrowRight = ( h <= root->h );

	bool shouldGrowRight = canGrowRight && ( root->h >= ( root->w + w ) ); // attempt to keep square-ish by growing right when height is much greater than width
	bool shouldGrowDown = canGrowDown && ( root->w >= ( root->h + h ) ); // attempt to keep square-ish by growing down  when width  is much greater than height

	if ( shouldGrowRight )		return growRight( w, h );
	else if ( shouldGrowDown )	return growDown( w, h );
	else if ( canGrowRight )	return growRight( w, h );
	else if ( canGrowDown )		return growDown( w, h );
	else return nullptr; // need to ensure sensible root starting size to avoid this happening
}

Node* SSGenerator::growRight( float w, float h )
{
	Node* newRoot = new Node( 0, 0, root->w + w, root->h );
	newRoot->used = true;
	Node* right = new Node( root->w, 0, w, root->h );
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
	Node* newRoot = new Node( 0, 0, root->w, root->h + h );
	newRoot->used = true;
	Node* down = new Node( 0, root->h, root->w, h );
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

//#define DRAW_DEBUG

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
		Image imgObject( filename, img.width(), img.height() );
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

	if ( !roots.empty() )
	{
		for ( int i = 0; i < roots.size(); i++ )
		{
			delete roots[i];
			roots[i] = nullptr;
		}
		roots.clear();
	}

	if ( !automaticSize )
	{
		if ( maxWidth > fixedSize.width() || maxHeight > fixedSize.height() )
		{
			return false;
		}
		root = new Node( 0, 0, fixedSize.width(), fixedSize.height() );
	}
	else
	{
		root = new Node( 0, 0, images[0].w, images[0].h );
	}

	fit( images, automaticSize, fixedSize );

	QString fmt = "yyyyMMdd_hhmmss";
	QString time = QDateTime::currentDateTime().toString( fmt );

	int indexAtlas = 0;
	int indexImage = 0;
	roots.push_back( root );

#ifdef DRAW_DEBUG
	QPen pen( Qt::red, 2 );
	QFont font;
	font.setBold( true );
#endif

	bool completed = false;
	while ( !completed )
	{
		int imagesInAtlas = 0;

		QImage atlas( QSize( roots[indexAtlas]->w, roots[indexAtlas]->h ), QImage::Format_RGBA8888 );
		atlas.fill( Qt::transparent );

		QString atlasFilename = folderPath + "/atlas_" + QString::number( indexAtlas ) + "_" + time + ".png";

		QPainter painter( &atlas );

#ifdef DRAW_DEBUG
		painter.setPen( pen );
		painter.setFont( font );
#endif
		for ( ; indexImage < images.size(); )
		{
			const auto& image = images[indexImage++];
			QString imgPath( folderPath + "/" + image.filename );
			painter.drawImage( QPoint( image.fit->x, image.fit->y ), QImage( imgPath ) );

#ifdef DRAW_DEBUG
			QString str( image.filename );
			str += " | " + QString::number( image.w ) + " | " + QString::number( image.h );
			painter.drawText( QPoint( image.fit->x, image.fit->y + 10 ), str );
#endif
			imagesInAtlas++;

			if ( indexImage == images.size() )
			{
#ifdef DRAW_DEBUG
				drawTree( &painter, roots[indexAtlas] );
#endif
				completed = true;
				break;
			}

			if ( ( indexImage != images.size() - 1 )
				 && ( images[indexImage].fit->x == 0 )
				 && ( images[indexImage].fit->y == 0 ) )
			{
#ifdef DRAW_DEBUG
				drawTree( &painter, roots[indexAtlas] );
#endif
				indexAtlas++;
				break;
			}
		}
		painter.end();

		if ( imagesInAtlas != 0 )
		{
			atlas.save( atlasFilename, "PNG" );
			spriteSheets.push_back( atlasFilename );
		}
		else
		{
			// Something bad happened...
			qDebug() << "What happened?";
		}
	}

	if ( root != nullptr )
	{
		root = nullptr;
	}

	if ( !roots.empty() )
	{
		for ( int i = 0; i < roots.size(); i++ )
		{
			delete roots[i];
			roots[i] = nullptr;
		}
		roots.clear();
	}

	return true;
}

void SSGenerator::drawTree( QPainter* painter, const Node* node )
{
	if ( node == nullptr )
	{
		return;
	}

	painter->setPen( QPen( node->used? Qt::green : Qt::blue, 2 ) );
	painter->drawRect( node->x, node->y, node->w, node->h );
	painter->setPen( QPen( Qt::red, 2 ) );

	drawTree( painter, node->right );
	drawTree( painter, node->down );
}
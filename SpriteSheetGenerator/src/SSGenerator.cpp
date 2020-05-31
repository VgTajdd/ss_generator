#include "SSGenerator.h"

#include <QImage>
#include <QDebug>

Node* SSGenerator::root = nullptr;

void SSGenerator::fit( std::vector< Image >& images )
{
	for ( int n = 0; n < images.size(); n++ )
	{
		auto& block = images[n];
		if ( auto node = findNode( root, block.w, block.h ) )
		{
			block.fit = splitNode( node, block.w, block.h );
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

bool SSGenerator::generateSpriteSheets( const std::vector< QString >& filenames,
										const QString& folderPath,
										const bool automaticSize,
										const QSize& fixedSize )
{
	// Sort by width.

	std::vector< Image > images;

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
		images.push_back( imgObject );
	}

	if ( loadImagesFailed )
	{
		return false;
	}

	// Sort by width (descendent).
	std::sort( images.begin(), images.end(), []( const Image& a, const Image& b ) { return a.w > b.w; } );

	if ( root )
	{
		delete root;
		root = nullptr;
	}

	root = new Node();
	root->w = 1024;
	root->h = 1024;

	fit( images );

	return true;
}
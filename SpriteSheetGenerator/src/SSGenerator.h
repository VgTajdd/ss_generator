#ifndef SS_GENERATOR_H
#define SS_GENERATOR_H

#include <vector>
#include <QString>

class QSize;
class QPainter;

struct Node
{
	Node( const int _x, const int _y, const int _w, const int _h ) :
		x( _x ), y( _y ), w( _w ), h( _h ), used( false ), down( nullptr ), right( nullptr ) { }
	~Node();
	float x;
	float y;
	float w;
	float h;
	bool used;
	Node* down;
	Node* right;
};

struct Image
{
	Image( const QString& _filename, const int _w, const int _h )
		: filename( _filename ), w( _w ), h( _h ), fit( nullptr ) { }
	QString filename;
	Node* fit;
	int w;
	int h;
};

class SSGenerator
{
	static Node* root;
	static std::vector< Node* > roots;
	static void fit( std::vector< Image >& images,
					 const bool automaticSize,
					 const QSize& fixedSize );
	static Node* findNode( Node* root, float w, float h );
	static Node* splitNode( Node* node, float w, float h );
	static Node* growNode( float w, float h );
	static Node* growRight( float w, float h );
	static Node* growDown( float w, float h );
	static void drawTree( QPainter* painter, const Node* indexRoot );
public:
	static bool generateSpriteSheets( std::vector< QString >& spriteSheetes,
									  const std::vector<QString>& filenames,
									  const QString& folderPath,
									  const bool automaticSize,
									  const QSize& fixedSize );
};

#endif
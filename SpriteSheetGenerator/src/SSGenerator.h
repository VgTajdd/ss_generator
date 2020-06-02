#ifndef SS_GENERATOR_H
#define SS_GENERATOR_H

#include <vector>
#include <QString>
#include <QSize>

struct Node
{
	~Node();
	float x = 0;
	float y = 0;
	float w = 0;
	float h = 0;
	bool used = false;
	Node* down = nullptr;
	Node* right = nullptr;
};

struct Image
{
	QString filename;
	Node* fit = nullptr;
	int w = 0;
	int h = 0;
};

class SSGenerator
{
	static Node* root;
	static void fit( std::vector< Image >& images, const bool automaticSize );
	static Node* findNode( Node* root, float w, float h );
	static Node* splitNode( Node* node, float w, float h );
	static Node* growNode( float w, float h );
	static Node* growRight( float w, float h );
	static Node* growDown( float w, float h );
public:
	static bool generateSpriteSheets( std::vector< QString >& spriteSheetes, const std::vector<QString>& filenames, const QString& folderPath, const bool automaticSize, const QSize& fixedSize );
};

#endif
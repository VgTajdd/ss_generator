#ifndef SS_GENERATOR_H
#define SS_GENERATOR_H

#include <vector>
#include <QString>
#include <QSize>

struct Node
{
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
	float w = 0;
	float h = 0;
};

class SSGenerator
{
	static Node* root;
	static void fit( std::vector< Image >& images );
	static Node* findNode( Node* root, float w, float h );
	static Node* splitNode( Node* node, float w, float h );
public:
	static bool generateSpriteSheets( const std::vector<QString>& filenames, const QString& folderPath, const bool automaticSize, const QSize& fixedSize );
};

#endif
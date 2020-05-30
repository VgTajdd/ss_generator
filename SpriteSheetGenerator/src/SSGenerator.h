#ifndef SS_GENERATOR_H
#define SS_GENERATOR_H

#include <vector>
#include <QString>
#include <QSize>

class SSGenerator
{
public:
	static bool generateSpriteSheets( const std::vector<QString>& filenames, const QString& folderPath, const bool automaticSize, const QSize& fixedSize );
};

#endif
#pragma once

#include <map>
#include <string>
#include <vector>
#include "ZFile.h"

class ZRoom;

enum class VerbosityLevel
{
	VERBOSITY_SILENT,
	VERBOSITY_INFO,
	VERBOSITY_DEBUG
};

struct TexturePoolEntry
{
	fs::path path = "";  // Path to Shared Texture
};

class GameConfig
{
public:
	std::map<int32_t, std::string> segmentRefs;
	std::map<int32_t, ZFile*> segmentRefFiles;
	std::map<uint32_t, std::string> symbolMap;
	std::vector<std::string> actorList;
	std::vector<std::string> objectList;
	std::map<uint32_t, TexturePoolEntry> texturePool;  // Key = CRC

	// ZBackground
	uint32_t bgScreenWidth = 320, bgScreenHeight = 240;

	GameConfig() = default;
};

typedef void (*ExporterSetFunc)(ZFile*);
typedef bool (*ExporterSetFuncBool)(ZFileMode fileMode);
typedef void (*ExporterSetFuncVoid)(int argc, char* argv[], int& i);
typedef void (*ExporterSetFuncVoid2)(std::string buildMode, ZFileMode& fileMode);
typedef void (*ExporterSetFuncVoid3)();

class ExporterSet
{
public:
	std::map<ZResourceType, ZResourceExporter*> exporters;
	ExporterSetFuncVoid parseArgsFunc = nullptr;
	ExporterSetFuncVoid2 parseFileModeFunc = nullptr;
	ExporterSetFuncBool processFileModeFunc = nullptr;
	ExporterSetFunc beginFileFunc = nullptr;
	ExporterSetFunc endFileFunc = nullptr;
	ExporterSetFuncVoid3 beginXMLFunc = nullptr;
	ExporterSetFuncVoid3 endXMLFunc = nullptr;
};

class Globals
{
public:
	static Globals* Instance;

	bool genSourceFile;  // Used for extraction
	bool useExternalResources;
	bool testMode;  // Enables certain experimental features
	bool outputCrc = false;
	bool profile;  // Measure performance of certain operations
	bool useLegacyZDList;
	VerbosityLevel verbosity;  // ZAPD outputs additional information
	ZFileMode fileMode;
	fs::path baseRomPath, inputPath, outputPath, sourceOutputPath, cfgPath;
	TextureType texType;
	ZGame game;
	GameConfig cfg;
	bool verboseUnaccounted = false;
	bool gccCompat = false;
	bool forceStatic = false;

	std::vector<ZFile*> files;
	std::vector<int32_t> segments;
	std::map<int32_t, std::string> segmentRefs;
	std::map<int32_t, ZFile*> segmentRefFiles;
	ZRoom* lastScene;
	std::map<uint32_t, std::string> symbolMap;

	std::string currentExporter;
	static std::map<std::string, ExporterSet*>* GetExporterMap();
	static void AddExporter(std::string exporterName, ExporterSet* exporterSet);

	Globals();
	std::string FindSymbolSegRef(int32_t segNumber, uint32_t symbolAddress);
	void ReadConfigFile(const std::string& configFilePath);
	void ReadTexturePool(const std::string& texturePoolXmlPath);
	void GenSymbolMap(const std::string& symbolMapPath);
	void AddSegment(int32_t segment, ZFile* file);
	bool HasSegment(int32_t segment);
	ZResourceExporter* GetExporter(ZResourceType resType);
	ExporterSet* GetExporterSet();
};
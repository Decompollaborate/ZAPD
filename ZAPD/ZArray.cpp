#include "ZArray.h"

#include <cassert>

#include "Globals.h"
#include "Utils/StringHelper.h"
#include "WarningHandler.h"
#include "ZFile.h"

REGISTER_ZFILENODE(Array, ZArray);

ZArray::ZArray(ZFile* nParent) : ZResource(nParent)
{
	canHaveInner = true;
	RegisterRequiredAttribute("Count");
}

ZArray::~ZArray()
{
	for (auto res : resList)
		delete res;
}

void ZArray::ParseXML(tinyxml2::XMLElement* reader)
{
	ZResource::ParseXML(reader);

	arrayCnt = reader->IntAttribute("Count", 0);
	if (arrayCnt <= 0)
	{
		HANDLE_ERROR_RESOURCE(WarningType::InvalidAttributeValue, parent, this, rawDataIndex,
		                      "invalid value found for 'Count' attribute", "");
	}

	tinyxml2::XMLElement* child = reader->FirstChildElement();
	if (child == nullptr)
	{
		HANDLE_ERROR_RESOURCE(WarningType::InvalidXML, parent, this, rawDataIndex,
		                      "<Array> needs one sub-element", "");
	}

	childName = child->Name();

	auto nodeMap = ZFile::GetNodeMap();
	size_t childIndex = rawDataIndex;
	for (size_t i = 0; i < arrayCnt; i++)
	{
		ZResource* res = nodeMap->at(childName)(parent);
		if (!res->DoesSupportArray())
		{
			std::string errorHeader = StringHelper::Sprintf(
				"resource <%s> does not support being wrapped in an <Array>", childName.c_str());
			HANDLE_ERROR_RESOURCE(WarningType::InvalidXML, parent, this, rawDataIndex, errorHeader,
			                      "");
		}
		res->parent = parent;
		res->SetInnerNode(true);
		res->ExtractFromXML(child, childIndex);

		childIndex += res->GetRawDataSize();
		resList.push_back(res);
	}
}

std::string ZArray::GetSourceOutputCode([[maybe_unused]] const std::string& prefix)
{
	std::string output = "";

	for (size_t i = 0; i < arrayCnt; i++)
	{
		output += resList.at(i)->GetBodySourceCode();
		output += ",";

		if (i < arrayCnt - 1)
			output += "\n";
	}

	Declaration* decl =
		parent->AddDeclarationArray(rawDataIndex, DeclarationAlignment::Align4, GetRawDataSize(),
	                                resList.at(0)->GetSourceTypeName(), name, arrayCnt, output);
	decl->staticConf = staticConf;

	return "";
}

std::string ZArray::GetSourceTypeName() const
{
	return resList.at(0)->GetSourceTypeName();
}

size_t ZArray::GetRawDataSize() const
{
	size_t size = 0;
	for (auto res : resList)
		size += res->GetRawDataSize();
	return size;
}

ZResourceType ZArray::GetResourceType() const
{
	return ZResourceType::Array;
}

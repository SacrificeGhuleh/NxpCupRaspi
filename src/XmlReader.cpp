//
// Created by Richard Zvonek on 07-Nov-18.
//
#if defined(__linux__) || defined(WIN32)
#include "pch.h"
#include "XmlReader.h"

namespace nxpbc {
    XmlReader::XmlReader(std::string filename) :
            filename_{filename},
            xmlFile_{readFile(filename_)} {
    }

    std::string XmlReader::readFile(const std::string &filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw "Nelze otevrit XML soubor";
        }
        std::stringstream fileData;
        fileData << file.rdbuf();
        file.close();
        return fileData.str();
    }

    std::string XmlReader::getChildString(const std::string &qXmlString, const std::string &name) {
        std::string begName = "<" + name + ">";
        const std::string endName = "</" + name + ">";

        std::string xmlString = qXmlString;
        long long beg = xmlString.find(begName);
        if (beg < 0)
            return "";
        beg += std::strlen(begName.data());

        const long long end = xmlString.find(endName);
        if (end < 0)
            return "";
        return xmlString.substr(beg, end - beg);
    }

    std::vector<std::string> XmlReader::getChildVector(const std::string &qXmlString, const std::string &name) {
        std::vector<std::string> children;

        std::string begName = "<" + name + ">";
        std::string endName = "</" + name + ">";

        std::string xmlString = qXmlString;

        while (true) {
            long long beg = xmlString.find(begName);
            if (beg < 0)
                break;
            beg += std::strlen(begName.data());

            const long long end = xmlString.find(endName);
            if (end < 0)
                break;
            const std::string child = xmlString.substr(beg, end - beg);
            children.emplace_back(child);
            xmlString = xmlString.substr(end + std::strlen(endName.data()));
        }
        return children;
    }

    float XmlReader::getChildFloat(const std::string &qXmlString, const std::string &name) {
        return std::strtof(getChildString(qXmlString, name).c_str(), nullptr);

    }

    int XmlReader::getChildInt(const std::string &qXmlString, const std::string &name) {
        return std::strtol(getChildString(qXmlString, name).c_str(), nullptr, 0);
    }
}
#endif

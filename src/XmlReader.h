//
// Created by Richard Zvonek on 07-Nov-18.
//

#ifndef NXPCUPRASPI_XMLREADER_H
#define NXPCUPRASPI_XMLREADER_H

#include <string>
#include <vector>


namespace nxpbc {
    /// \brief Obecný XML reader
    class XmlReader {
    public:
        /**
         * @brief Konstruktor třídy XmlReader
         * @param filename Nazev souboru
         */
        XmlReader(std::string filename);

    private:
        /**
         * @brief Metoda pro načtení souboru do proměnné
         * @param filename Název souboru
         * @return Obsah souboru
         */
        static std::string readFile(const std::string &filename);

    protected:
        const std::string filename_;
        const std::string xmlFile_;

        /**
         * @brief Metoda pro získání obsahu elementu
         * @param qXmlString XML string
         * @param name Název elementu
         * @return Obsah elementu
         */
        static std::string getChildString(const std::string &qXmlString, const std::string &name);

        /**
         * @brief Metoda pro získání desetinného čísla v elementu
         * @param qXmlString XML string
         * @param name Název elementu
         * @return Prvek elementu
         */
        static float getChildFloat(const std::string &qXmlString, const std::string &name);

        /**
         * @brief Metoda pro získání celého čísla v elementu
         * @param qXmlString XML string
         * @param name Název elementu
         * @return Prvek elementu
         */
        static int getChildInt(const std::string &qXmlString, const std::string &name);

        /**
         * @brief Metoda pro získání prvků elementu
         * @param qXmlString XML string
         * @param name Název elementu
         * @return Pole prvků
         */
        static std::vector<std::string> getChildVector(const std::string &qXmlString, const std::string &name);
    };
}
#endif //NXPCUPRASPI_XMLREADER_H

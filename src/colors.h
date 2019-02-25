//
// Created by
// https://github.com/gon1332/fort320/blob/master/include/Utils/colors.h
//

#ifndef NXPCUPRASPI_COLORS_H
#define NXPCUPRASPI_COLORS_H

namespace nxpbc {
/* FOREGROUND */
#define RST   "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

/**
 * @brief Funkce pro cervenou barvu textu
 */
#define FRED(x) KRED x RST

/**
 * @brief Funkce pro zelenou barvu textu
 */
#define FGRN(x) KGRN x RST

/**
 * @brief Funkce pro zlutou barvu textu
 */
#define FYEL(x) KYEL x RST

/**
 * @brief Funkce pro modrou barvu textu
 */
#define FBLU(x) KBLU x RST

/**
 * @brief Funkce pro fialovou barvu textu
 */
#define FMAG(x) KMAG x RST

/**
 * @brief Funkce pro azurovou barvu textu
 */
#define FCYN(x) KCYN x RST

/**
 * @brief Funkce pro bilou barvu textu
 */
#define FWHT(x) KWHT x RST

/**
 * @brief Funkce pro tucny text
 */
#define BOLD(x) "\x1B[1m" x RST

/**
 * @brief Funkce pro italicky text
 */
#define UNDL(x) "\x1B[4m" x RST

}

#endif //NXPCUPRASPI_COLORS_H

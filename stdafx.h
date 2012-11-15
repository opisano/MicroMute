// stdafx.h : fichier Include pour les fichiers Include système standard,
// ou les fichiers Include spécifiques aux projets qui sont utilisés fréquemment,
// et sont rarement modifiés
//

#pragma once

#include "targetver.h"

#define WINVER 0x0600
#define WIN32_LEAN_AND_MEAN             // Exclure les en-têtes Windows rarement utilisés
// Fichiers d'en-tête Windows :
#include <windows.h>
#include <shellapi.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>

// Fichiers d'en-tête C RunTime
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


// TODO: faites référence ici aux en-têtes supplémentaires nécessaires au programme

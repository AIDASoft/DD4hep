#ifndef GAUDIPLUGINSERVICE_GAUDI_PLUGINSERVICE_H
#define GAUDIPLUGINSERVICE_GAUDI_PLUGINSERVICE_H
/*****************************************************************************\
* (c) Copyright 2013 CERN                                                     *
*                                                                             *
* This software is distributed under the terms of the GNU General Public      *
* Licence version 3 (GPL Version 3), copied verbatim in the file "LICENCE".   *
*                                                                             *
* In applying this licence, CERN does not waive the privileges and immunities *
* granted to it by virtue of its status as an Intergovernmental Organization  *
* or submit itself to any jurisdiction.                                       *
\*****************************************************************************/

/// @author Marco Clemencic <marco.clemencic@cern.ch>
/// See @ref GaudiPluginService-readme

#include "Gaudi/Details/PluginServiceCommon.h"

#if GAUDI_PLUGIN_SERVICE_USE_V2
#  include "Gaudi/PluginServiceV2.h"
#else
#  include "Gaudi/PluginServiceV1.h"
#endif

#endif

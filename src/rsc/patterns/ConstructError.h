/* ============================================================
 *
 * This file is part of the RSC project
 *
 * Copyright (C) 2010, 2011 Jan Moringen
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#pragma once

#include <stdexcept>
#include <string>

#include "rsc/rscexports.h"

namespace rsc {
namespace patterns {

/**
 * @author Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
 */
class RSC_EXPORT ConstructError: public std::runtime_error {
public:
    ConstructError(const std::string& message);
};

}
}

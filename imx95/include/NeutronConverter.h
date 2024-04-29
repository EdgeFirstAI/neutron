/*
 * Copyright 2022-2024 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef NEUTRON_CONVERTER_H
#define NEUTRON_CONVERTER_H

#include <string>
#include <vector>

namespace converter {

/// Takes an input model as a raw buffer \p modelBuffer and converts it for Neutron for the given \p target.
std::vector<uint8_t> convertModel(const std::vector<uint8_t> &modelBuffer, const std::string &target);

} // namespace converter

#endif // NEUTRON_CONVERTER_H

#pragma once

#include <pd/base/Allocator.hpp>
#include <pd/Error.hpp>
#include <pd/base/FileSystem.hpp>
#include <pd/Hid.hpp>
#include <pd/Image.hpp>
#include <pd/Installer.hpp>
#include <pd/Lithium.hpp>
#include <pd/Message.hpp>
#include <pd/Net.hpp>
#include <pd/Overlays.hpp>
#include <pd/Rubidium.hpp>
#include <pd/Sound.hpp>
#include <pd/Texture.hpp>
#include <pd/Sheet.hpp>
#include <pd/Timer.hpp>
#include <pd/UI7.hpp>
#include <pd/global_db.hpp>
#include <pd/palladium.hpp>

namespace Palladium {
using Lithium = LI;
using RB = Rubidium;
}  // namespace Palladium

namespace PD = Palladium;

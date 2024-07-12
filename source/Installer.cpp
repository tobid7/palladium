#include <3ds.h>

#include <pd/Installer.hpp>
#include <pd/internal_db.hpp>

namespace Palladium {
Result DeletePrevious(u64 id, FS_MediaType mt) {
  if (!pdi_is_am_init) return -1;
  u32 num_titles = 0;
  Result ret = AM_GetTitleCount(mt, &num_titles);
  if (R_FAILED(ret)) {
    return ret;
  }

  u32 read_titles = 0;
  auto ids = new u64[num_titles];
  ret = AM_GetTitleList(&read_titles, mt, num_titles, ids);

  if (R_FAILED(ret)) {
    delete[] ids;
    return ret;
  }

  for (u32 i = 0; i < read_titles; i++) {
    if (ids[i] == id) {
      ret = AM_DeleteAppTitle(mt, id);
      break;
    }
  }

  delete[] ids;
  if (R_FAILED(ret)) {
    return ret;
  }

  return 0;
}

FS_MediaType GetTitleDest(u64 id) {
  u16 platform = (u16)((id >> 48) & 0xFFFF);
  u16 category = (u16)((id >> 32) & 0xFFFF);
  u8 variation = (u8)(id & 0xFF);

  /*     DSiWare                3DS                    DSiWare, System, DLP
   * Application           System Title */
  return platform == 0x0003 || (platform == 0x0004 &&
                                ((category & 0x8011) != 0 ||
                                 (category == 0x0000 && variation == 0x02)))
             ? MEDIATYPE_NAND
             : MEDIATYPE_SD;
}

InstallerInfo pdi_installer_info;

InstallerInfo InstallGetInfo() { return pdi_installer_info; }

void InstallSetBuffersSize(unsigned int bytes) {
  if (pdi_installer_info.active) return;
  if (bytes == 0) return;
  if (bytes >= 0x200000) bytes = 0x200000;
  pdi_installer_info.mem_size = bytes;
}

Result InstallCia(const std::string& path, bool self) {
  if (!pdi_is_am_init) return -1;
  if (pdi_installer_info.active) return -1;
  u32 bytes_read = 0;
  u32 bytes_written = 0;
  pdi_installer_info.current = 0;
  // Set 1 to avoid div0 error
  pdi_installer_info.total = 1;
  u64 size = 0;
  Handle cia, file;
  AM_TitleEntry info;
  Result ret = 0;
  FS_MediaType media = MEDIATYPE_SD;

  std::string pth;
  if (path[0] == '/') {
    pth = path;
  } else if (path.substr(0, 5) == "sdmc:") {
    pth = path.substr(5);
  } else {
    return -1;
  }

  ret = FSUSER_OpenFileDirectly(&file, ARCHIVE_SDMC, fsMakePath(PATH_EMPTY, ""),
                                fsMakePath(PATH_ASCII, pth.c_str()),
                                FS_OPEN_READ, 0);

  if (R_FAILED(ret)) {
    return ret;
  }

  ret = AM_GetCiaFileInfo(media, &info, file);

  if (R_FAILED(ret)) {
    return ret;
  }

  media = GetTitleDest(info.titleID);

  if (!self) {
    ret = DeletePrevious(info.titleID, media);
    if (R_FAILED(ret)) return ret;
  }

  ret = FSFILE_GetSize(file, &size);

  if (R_FAILED(ret)) {
    return ret;
  }

  ret = AM_StartCiaInstall(media, &cia);

  if (R_FAILED(ret)) {
    return ret;
  }

  std::vector<unsigned char> buf(pdi_installer_info.mem_size);
  pdi_installer_info.total = size;

  do {
    FSFILE_Read(file, &bytes_read, pdi_installer_info.current, &buf[0],
                buf.size());
    FSFILE_Write(cia, &bytes_written, pdi_installer_info.current, &buf[0],
                 buf.size(), FS_WRITE_FLUSH);
    pdi_installer_info.current += bytes_read;
  } while (pdi_installer_info.current < pdi_installer_info.total);

  ret = AM_FinishCiaInstall(cia);

  if (R_FAILED(ret)) {
    return ret;
  }

  ret = FSFILE_Close(file);

  if (R_FAILED(ret)) {
    return ret;
  }

  if (self) {
    aptSetChainloaderToSelf();
  }

  return 0;
}
}  // namespace Palladium
#pragma once

#include "namespace.h"

namespace TOPNSPC {

class BlueFS {
public:
  // 文件系统支持不同种类的块设备
  static constexpr unsigned MAX_BDEV = 5;
  static constexpr unsigned BDEV_WAL = 0;
  static constexpr unsigned BDEV_DB = 1;
  static constexpr unsigned BDEV_SLOW = 2;
  static constexpr unsigned BDEV_NEWWAL = 3;
  static constexpr unsigned BDEV_NEWDB = 4;

  enum {
    WRITER_UNKNOWN,
    WRITER_WAL,     // RocksDB 的 log 文件
    WRITER_SST,     // RocksDB 的 sst 文件
  };

  BlueFS();
  ~BlueFS();
};

}
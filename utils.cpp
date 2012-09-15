#include "stdafx.h"
#include "utils.hpp"

bool fileExists(const char *filename)
{
  struct _stat status;
  return _access(filename, 0) == 0 && _stat(filename, &status) == 0 && (status.st_mode & _S_IFREG);
}

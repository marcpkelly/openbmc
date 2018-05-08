#include <iostream>
#include <sstream>
#include <string>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <openbmc/pal.h>
#include "fw-util.h"

#define PAGE_SIZE                     0x1000
#define VERIFIED_BOOT_STRUCT_BASE     0x1E720000
#define VERIFIED_BOOT_HARDWARE_ENFORCE(base) \
  *((uint8_t *)(base + 0x215))

using namespace std;

int System::runcmd(const string &cmd)
{
#ifdef DEBUG
  cout << "Executing: " << cmd << endl;
#endif
  int status = system(cmd.c_str());
  if (status == -1) {
    return 127;
  }
  if (WIFEXITED(status) && (WEXITSTATUS(status) == 0))
    return FW_STATUS_SUCCESS;
  return FW_STATUS_FAILURE;
}

bool System::vboot_hardware_enforce(void)
{
  int mem_fd;
  uint8_t *vboot_base;
  uint8_t hw_enforce_flag;

  mem_fd = open("/dev/mem", O_RDWR | O_SYNC);
  if (mem_fd < 0) {
    cerr << "Error opening /dev/mem" << endl;
    return false;
  }

  vboot_base = (uint8_t *)mmap(NULL, PAGE_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, mem_fd, VERIFIED_BOOT_STRUCT_BASE);
  if (!vboot_base) {
    cerr << "Error mapping VERIFIED_BOOT_STRUCT_BASE" << endl;
    close(mem_fd);
    return false;
  }

  hw_enforce_flag = VERIFIED_BOOT_HARDWARE_ENFORCE(vboot_base);

  munmap(vboot_base, PAGE_SIZE);
  close(mem_fd);

  return hw_enforce_flag == 1 ? true : false;
}
bool System::get_mtd_name(const char* name, char* dev)
{
  FILE* partitions = fopen("/proc/mtd", "r");
  char line[256], mnt_name[32];
  unsigned int mtdno;
  bool found = false;

  if (!partitions) {
    return false;
  }
  if (dev) {
    dev[0] = '\0';
  }
  while (fgets(line, sizeof(line), partitions)) {
    if(sscanf(line, "mtd%d: %*x %*x %s",
                &mtdno, mnt_name) == 2) {
      if(!strcmp(name, mnt_name)) {
        if (dev) {
          sprintf(dev, "/dev/mtd%d", mtdno);
        }
        found = true;
        break;
      }
    }
  }
  fclose(partitions);
  return found;
}

string& System::name()
{
  static string ret = "";
  if (ret == "") {
    char machine[128] = "NA";
    FILE *fp = fopen("/etc/issue", "r");
    if (fp) {
      if (fscanf(fp, "OpenBMC Release %[^ \t\n-]", machine) == 1) {
        ret = machine;
      }
      fclose(fp);
    }
  }
  return ret;
}

string& System::partition_conf()
{
  static string parts = "/etc/image_parts.json";
  return parts;
}

uint8_t System::get_fru_id(string &name)
{
  uint8_t fru_id;
  if (pal_get_fru_id((char *)name.c_str(), &fru_id))
  {

    // Set to some default FRU which should be present
    // in the system.
    fru_id = 1;
  }
  return fru_id;
}

void System::set_update_ongoing(uint8_t fru_id, int timeo)
{
  pal_set_fw_update_ongoing(fru_id, timeo);
}

string& System::lock_file(string &name)
{
  static string f = "/var/run/fw-util-" + name + ".lock";
  return f;
}

#include <kernel/device.h>
#include <kernel/error.h>
#include <string.h>

PD_SLIST_HEAD(devices, pd_dev_t);

static struct devices devices;

pd_dev_t* pd_dev_fromname(const char* name) {
  pd_dev_t* device = NULL;
  PD_SLIST_FOREACH(device, &devices, d_list) {
    if (!strcmp(device->name, name)) return device;
  }
  return NULL;
}

int pd_dev_register(pd_dev_t* device) {
  if (pd_dev_fromname(device->name) != NULL) return -EEXIST;
  PD_SLIST_INSERT_HEAD(&devices, device, d_list);
  return 0;
}
